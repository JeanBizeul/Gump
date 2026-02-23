#include "Actions.hpp"
#include "ActionRegistry.hpp"
#include "Application.hpp"
#include "Logger.hpp"
#include "Actions/TopMenuActions.hpp"

#include <fstream>
#include <filesystem>
#include <vector>
#include <cstring>
#include <map>
#include "Utils/Utils.hpp"

// Use libzip for compression
#include <zip.h>

// Forward declare stb_image_write functions (implementation is in another TU)
extern "C" {
    int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
    unsigned char* stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
    void stbi_image_free(void *retval_from_stbi_load);
}

// Helper function to parse config file
static std::map<std::string, std::map<std::string, std::string>> parseConfigFile(const std::string& configPath) {
    std::map<std::string, std::map<std::string, std::string>> config;
    std::ifstream file(configPath);
    
    if (!file.is_open()) {
        LOG_ERROR("Failed to open config file: {}", configPath);
        return config;
    }
    
    std::string currentSection;
    std::string line;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // Skip empty lines
        if (line.empty()) continue;
        
        // Check for section header
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            config[currentSection] = std::map<std::string, std::string>();
            LOG_DEBUG("Found config section: '{}'", currentSection);
        }
        // Parse key=value pair
        else if (!currentSection.empty()) {
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                config[currentSection][key] = value;
                LOG_DEBUG("  {}={}", key, value);
            }
        }
    }
    
    file.close();
    LOG_INFO("Parsed {} sections from config file", config.size());
    return config;
}

// Helper function to extract zip archive
static bool extractZipArchive(const std::string& zipPath, const std::string& extractPath) {
    namespace fs = std::filesystem;
    
    // Open the zip archive
    int error;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);
    if (!archive) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        LOG_ERROR("Failed to open zip archive: {}", zip_error_strerror(&zip_error));
        zip_error_fini(&zip_error);
        return false;
    }
    
    // Create extraction directory
    try {
        if (fs::exists(extractPath)) {
            fs::remove_all(extractPath);
        }
        fs::create_directories(extractPath);
    } catch (const fs::filesystem_error& e) {
        LOG_ERROR("Failed to create extraction directory: {}", e.what());
        zip_close(archive);
        return false;
    }
    
    // Get number of files in archive
    zip_int64_t numFiles = zip_get_num_entries(archive, 0);
    
    // Extract each file
    for (zip_int64_t i = 0; i < numFiles; i++) {
        // Get file name
        const char* filename = zip_get_name(archive, i, 0);
        if (!filename) {
            LOG_ERROR("Failed to get filename for entry {}", i);
            continue;
        }
        
        // Open file in archive
        zip_file_t* file = zip_fopen_index(archive, i, 0);
        if (!file) {
            LOG_ERROR("Failed to open file in archive: {}", filename);
            continue;
        }
        
        // Get file size
        zip_stat_t stat;
        if (zip_stat_index(archive, i, 0, &stat) != 0) {
            LOG_ERROR("Failed to get file stats: {}", filename);
            zip_fclose(file);
            continue;
        }
        
        // Read file contents
        std::vector<char> buffer(stat.size);
        zip_int64_t bytesRead = zip_fread(file, buffer.data(), stat.size);
        zip_fclose(file);
        
        if (bytesRead != static_cast<zip_int64_t>(stat.size)) {
            LOG_ERROR("Failed to read complete file: {}", filename);
            continue;
        }
        
        // Write to disk
        fs::path outputPath = fs::path(extractPath) / filename;
        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile.is_open()) {
            LOG_ERROR("Failed to create output file: {}", outputPath.string());
            continue;
        }
        
        outFile.write(buffer.data(), buffer.size());
        outFile.close();
        
        LOG_DEBUG("Extracted: {}", filename);
    }
    
    zip_close(archive);
    LOG_INFO("Successfully extracted {} files from archive", numFiles);
    return true;
}

// Helper function to add a file to a zip archive using libzip
static bool addFileToZip(zip_t* archive, const std::string& filepath, const std::string& archiveName) {
    // Read file contents
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open file for zipping: {}", filepath);
        return false;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Allocate buffer that will be managed by libzip
    char* buffer = new char[fileSize];
    file.read(buffer, fileSize);
    file.close();

    // Create zip source from buffer - libzip will take ownership and free the buffer
    zip_source_t* source = zip_source_buffer(archive, buffer, fileSize, 1); // 1 = freep (libzip will free the buffer)
    if (source == nullptr) {
        LOG_ERROR("Failed to create zip source: {}", zip_strerror(archive));
        delete[] buffer;
        return false;
    }

    // Add file to archive
    zip_int64_t index = zip_file_add(archive, archiveName.c_str(), source, ZIP_FL_ENC_UTF_8);
    if (index < 0) {
        LOG_ERROR("Failed to add file to zip: {}", zip_strerror(archive));
        zip_source_free(source);
        return false;
    }

    return true;
}

// Helper function to export a layer as PNG
static bool exportLayerAsPNG(Gump::Application& app, Gump::Layer& layer, const std::string& filepath) {
    auto& textureAtlas = app.getTextureAtlas();
    
    // Get the texture page for this layer
    auto pageTexIdOpt = textureAtlas.getPageTextureID(layer.texturePageIndex);
    if (!pageTexIdOpt) {
        LOG_ERROR("Failed to get texture page for layer");
        return false;
    }

    GLuint textureID = *pageTexIdOpt;

    // Get texture dimensions
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    // Get layer's position in the texture atlas
    glm::vec2 layerUVMin = layer.getUVMin();
    int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
    int layerTexY = static_cast<int>(layerUVMin.y * texHeight);

    int layerWidth = static_cast<int>(layer.getWidth());
    int layerHeight = static_cast<int>(layer.getHeight());

    // Read pixels from the GPU texture
    std::vector<unsigned char> pixels(layerWidth * layerHeight * 4);

    // Create a framebuffer to read from the texture
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        glReadPixels(layerTexX, layerTexY, layerWidth, layerHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    } else {
        LOG_ERROR("Framebuffer incomplete, cannot read pixels");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    // Write PNG directly without flipping - the texture atlas already stores layers correctly
    if (!stbi_write_png(filepath.c_str(), layerWidth, layerHeight, 4, pixels.data(), layerWidth * 4)) {
        LOG_ERROR("Failed to write PNG file: {}", filepath);
        return false;
    }

    return true;
}

// Helper function to perform the actual save operation
static bool performSave(Gump::Application& app, const std::string& filepath) {
    namespace fs = std::filesystem;
    
    // Extract directory name from filepath (remove extension if present)
    fs::path path(filepath);
    std::string dirName = path.stem().string();
    fs::path saveDir = path.parent_path() / (dirName + "_temp");
    
    // Create the temporary save directory
    try {
        if (fs::exists(saveDir)) {
            LOG_WARNING("Temporary save directory already exists, removing: {}", saveDir.string());
            fs::remove_all(saveDir);
        }
        fs::create_directories(saveDir);
    } catch (const fs::filesystem_error& e) {
        LOG_ERROR("Failed to create temporary save directory: {}", e.what());
        return false;
    }

    LOG_INFO("Saving project to temporary directory: {}", saveDir.string());

    // Create config file
    fs::path configPath = saveDir / "project.cfg";
    std::ofstream configFile(configPath);
    if (!configFile.is_open()) {
        LOG_ERROR("Failed to create config file: {}", configPath.string());
        return false;
    }

    // Write metadata
    auto& layers = app.getLayers();
    auto canvasSize = app.getCanvasSize();
    
    configFile << "[Project]\n";
    configFile << "CanvasWidth=" << canvasSize.x << "\n";
    configFile << "CanvasHeight=" << canvasSize.y << "\n";
    configFile << "LayerCount=" << layers.size() << "\n";
    configFile << "\n";

    // Export each layer
    std::vector<fs::path> layerPaths;
    for (size_t i = 0; i < layers.size(); i++) {
        auto& layer = layers[i];
        
        configFile << "[Layer" << i << "]\n";
        configFile << "Name=" << layer->name << "\n";
        configFile << "Width=" << layer->getWidth() << "\n";
        configFile << "Height=" << layer->getHeight() << "\n";
        configFile << "PositionX=" << layer->position.x << "\n";
        configFile << "PositionY=" << layer->position.y << "\n";
        configFile << "Transparency=" << layer->transparency << "\n";
        configFile << "Visible=" << (layer->isVisible ? "1" : "0") << "\n";
        
        // Generate filename for this layer
        std::string layerFilename = "layer_" + std::to_string(i) + ".png";
        configFile << "File=" << layerFilename << "\n";
        configFile << "\n";
        
        // Export layer as PNG
        fs::path layerPath = saveDir / layerFilename;
        layerPaths.push_back(layerPath);
        
        if (!exportLayerAsPNG(app, *layer, layerPath.string())) {
            LOG_ERROR("Failed to export layer {}: {}", i, layer->name);
            configFile.close();
            fs::remove_all(saveDir);
            return false;
        }
        
        LOG_INFO("Exported layer {}: {} ({}x{})", i, layer->name, layer->getWidth(), layer->getHeight());
    }

    configFile.close();
    LOG_INFO("Project files created successfully");
    
    // Create zip archive using libzip
    LOG_INFO("Creating zip archive: {}", filepath);
    
    int error;
    zip_t* archive = zip_open(filepath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    if (archive == nullptr) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        LOG_ERROR("Failed to create zip archive: {}", zip_error_strerror(&zip_error));
        zip_error_fini(&zip_error);
        fs::remove_all(saveDir);
        return false;
    }
    
    // Add config file to zip
    if (!addFileToZip(archive, configPath.string(), "project.cfg")) {
        zip_close(archive);
        fs::remove_all(saveDir);
        return false;
    }
    
    // Add all layer files to zip
    for (size_t i = 0; i < layerPaths.size(); i++) {
        std::string archiveName = "layer_" + std::to_string(i) + ".png";
        if (!addFileToZip(archive, layerPaths[i].string(), archiveName)) {
            zip_close(archive);
            fs::remove_all(saveDir);
            return false;
        }
    }
    
    // Finalize and close the archive
    if (zip_close(archive) < 0) {
        LOG_ERROR("Failed to finalize zip archive: {}", zip_strerror(archive));
        fs::remove_all(saveDir);
        return false;
    }
    
    LOG_INFO("Zip archive created successfully");
    
    // Delete temporary directory
    try {
        fs::remove_all(saveDir);
        LOG_INFO("Temporary directory cleaned up");
    } catch (const fs::filesystem_error& e) {
        LOG_WARNING("Failed to remove temporary directory: {}", e.what());
        // Don't fail the save operation if cleanup fails
    }
    
    // Update the current file path
    app.setCurrentFilePath(filepath);
    
    LOG_INFO("Project saved successfully to: {}", filepath);
    return true;
}

namespace Gump {
namespace Actions {

// File Actions
void newFile(Application& app) {
    LOG_INFO("Action: New File");
    TopMenu::newFile(app);
}

void openFile(Application& app) {
    LOG_INFO("Action: Open File");
    TopMenu::openFile(app);
}

void loadFile(Application& app) {
    LOG_INFO("Action: Load File");
    
    namespace fs = std::filesystem;
    
    // Prompt user for file to load
    std::string filepath = Gump::Utils::openFilePickerDialog(
        "Open Gump Project",
        "Gump Project|*.gump|All Files|*.*"
    );
    
    if (filepath.empty()) {
        LOG_INFO("Load cancelled by user");
        return;
    }
    
    LOG_INFO("Loading project from: {}", filepath);
    
    // Step 0: Reset the project - clear all layers
    LOG_INFO("Resetting project...");
    app.getLayers().clear();
    
    // Clear selection state
    app.getSelectionState().hasSelection = false;
    app.getSelectionState().clearMask();
    app.updateSelectionMesh();
    
    // Step 1: Create temporary extraction directory
    fs::path path(filepath);
    std::string dirName = path.stem().string();
    fs::path tempDir = path.parent_path() / (dirName + "_load_temp");
    
    LOG_INFO("Extracting to temporary directory: {}", tempDir.string());
    
    // Step 2: Extract the zip archive
    if (!extractZipArchive(filepath, tempDir.string())) {
        LOG_ERROR("Failed to extract archive");
        // Create a default layer on failure
        app.addEmptyLayer("Layer 1", 800, 600);
        return;
    }
    
    // Step 3: Load the config file
    fs::path configPath = tempDir / "project.cfg";
    if (!fs::exists(configPath)) {
        LOG_ERROR("Config file not found in archive");
        fs::remove_all(tempDir);
        app.addEmptyLayer("Layer 1", 800, 600);
        return;
    }
    
    auto config = parseConfigFile(configPath.string());
    
    // Parse project metadata
    if (config.find("Project") == config.end()) {
        LOG_ERROR("Project section not found in config");
        fs::remove_all(tempDir);
        app.addEmptyLayer("Layer 1", 800, 600);
        return;
    }
    
    auto& projectSection = config["Project"];
    int canvasWidth = std::stoi(projectSection["CanvasWidth"]);
    int canvasHeight = std::stoi(projectSection["CanvasHeight"]);
    int layerCount = std::stoi(projectSection["LayerCount"]);
    
    LOG_INFO("Loading project: {}x{} canvas with {} layers", canvasWidth, canvasHeight, layerCount);
    
    // Set canvas size
    app.setCanvasSize(glm::uvec2(canvasWidth, canvasHeight));
    
    // Step 4: Load each layer
    for (int i = 0; i < layerCount; i++) {
        std::string layerSection = "Layer" + std::to_string(i);
        
        if (config.find(layerSection) == config.end()) {
            LOG_ERROR("Layer section {} not found in config", layerSection);
            continue;
        }
        
        auto& layer = config[layerSection];
        
        std::string layerName = layer["Name"];
        int width = std::stoi(layer["Width"]);
        int height = std::stoi(layer["Height"]);
        float posX = std::stof(layer["PositionX"]);
        float posY = std::stof(layer["PositionY"]);
        float transparency = std::stof(layer["Transparency"]);
        bool visible = (layer["Visible"] == "1");
        std::string filename = layer["File"];
        
        LOG_INFO("Loading layer {}: {} ({}x{})", i, layerName, width, height);
        
        // Load the layer image file
        fs::path layerPath = tempDir / filename;
        if (!fs::exists(layerPath)) {
            LOG_ERROR("Layer file not found: {}", layerPath.string());
            continue;
        }
        
        // Load image using stb_image
        int imgWidth, imgHeight, imgChannels;
        unsigned char* imgData = stbi_load(layerPath.string().c_str(), &imgWidth, &imgHeight, &imgChannels, 4);
        
        if (!imgData) {
            LOG_ERROR("Failed to load layer image: {}", layerPath.string());
            continue;
        }
        
        // Convert to vector
        std::vector<unsigned char> pixels(imgData, imgData + (imgWidth * imgHeight * 4));
        stbi_image_free(imgData);
        
        // Add to texture atlas with original layer name
        std::string atlasName = layerName + "_loaded_" + std::to_string(i);
        if (app.getTextureAtlas().addImageFromPixels(atlasName, imgWidth, imgHeight, pixels)) {
            auto uvRectOpt = app.getTextureAtlas().getUVRect(atlasName);
            if (uvRectOpt) {
                // Create the layer
                app.addLayer(layerName, width, height, uvRectOpt->uvMin, uvRectOpt->uvMax, uvRectOpt->pageIndex);
                
                // Restore layer properties
                auto& loadedLayer = app.getLayer(app.getLayerCount() - 1);
                loadedLayer.position = glm::vec2(posX, posY);
                loadedLayer.transparency = transparency;
                loadedLayer.isVisible = visible;
                loadedLayer.updateMesh(); // Update mesh with position
                
                LOG_INFO("Successfully loaded layer: {}", layerName);
            } else {
                LOG_ERROR("Failed to get UV coordinates for loaded layer");
            }
        } else {
            LOG_ERROR("Failed to add loaded layer to texture atlas");
        }
    }
    
    // Step 5: Clean up temporary directory
    LOG_INFO("Cleaning up temporary directory...");
    try {
        fs::remove_all(tempDir);
        LOG_INFO("Temporary directory cleaned up");
    } catch (const fs::filesystem_error& e) {
        LOG_WARNING("Failed to remove temporary directory: {}", e.what());
    }
    
    // If no layers were loaded, create a default one
    if (app.getLayerCount() == 0) {
        LOG_WARNING("No layers were loaded, creating default layer");
        app.addEmptyLayer("Layer 1", canvasWidth, canvasHeight);
    }
    
    // Update the current file path
    app.setCurrentFilePath(filepath);
    
    LOG_INFO("Project loaded successfully!");
}

void saveFile(Application& app) {
    LOG_INFO("Action: Save File");
    
    // If no current file path, behave like Save As
    if (app.getCurrentFilePath().empty()) {
        saveFileAs(app);
        return;
    }
    
    // Save to the current file path
    if (!performSave(app, app.getCurrentFilePath())) {
        LOG_ERROR("Failed to save project");
    }
}

void saveFileAs(Application& app) {
    LOG_INFO("Action: Save File As");
    
    // Prompt user for save location
    std::string filepath = Gump::Utils::saveFilePickerDialog(
        "Save Project As",
        "Gump Project|*.gump|All Files|*.*"
    );
    
    if (filepath.empty()) {
        LOG_INFO("Save cancelled by user");
        return;
    }
    
    // Ensure the file has a .gump extension
    if (filepath.find(".gump") == std::string::npos) {
        filepath += ".gump";
    }
    
    // Perform the save
    if (!performSave(app, filepath)) {
        LOG_ERROR("Failed to save project");
    }
}

void exportFile(Application& app) {
    LOG_INFO("Action: Export File");
    TopMenu::exportFile(app);
}

// Edit Actions
void undo(Application& app) {
    LOG_INFO("Action: Undo");
    // TODO: Implement undo system
}

void redo(Application& app) {
    LOG_INFO("Action: Redo");
    // TODO: Implement redo system
}

void cut(Application& app) {
    if (!app.getSelectionState().hasSelection) {
        LOG_WARNING("No selection to cut");
        return;
    }
    LOG_INFO("Action: Cut");
    app.cutSelection();
}

void copy(Application& app) {
    if (!app.getSelectionState().hasSelection) {
        LOG_WARNING("No selection to copy");
        return;
    }
    LOG_INFO("Action: Copy");
    app.copySelection();
}

void paste(Application& app) {
    if (!app.getClipboard().hasData) {
        LOG_WARNING("Clipboard is empty");
        return;
    }
    LOG_INFO("Action: Paste");
    app.pasteClipboard();
}

// Selection Actions
void selectAll(Application& app) {
    LOG_INFO("Action: Select All");
    if (app.getLayerCount() == 0) {
        LOG_WARNING("No layers to select");
        return;
    }
    
    // Select the entire top layer
    auto& topLayer = app.getLayer(app.getLayerCount() - 1);
    auto& selState = app.getSelectionState();
    selState.hasSelection = true;
    selState.startPos = glm::vec2(0, 0);
    selState.endPos = glm::vec2(topLayer.getWidth(), topLayer.getHeight());
    selState.offset = glm::vec2(0, 0);
    selState.clearMask();
    app.updateSelectionMesh();
}

void deselectAll(Application& app) {
    LOG_INFO("Action: Deselect");
    auto& selState = app.getSelectionState();
    selState.hasSelection = false;
    selState.clearMask();
    app.updateSelectionMesh();
}

// Tool Actions
void selectPencilTool(Application& app) {
    LOG_INFO("Action: Select Pencil Tool");
    app.setSelectedTool("pencil");
}

void selectEraserTool(Application& app) {
    LOG_INFO("Action: Select Eraser Tool");
    app.setSelectedTool("eraser");
}

void selectSelectionTool(Application& app) {
    LOG_INFO("Action: Select Selection Tool");
    app.setSelectedTool("selection");
}

void selectMoveTool(Application& app) {
    LOG_INFO("Action: Select Move Tool");
    app.setSelectedTool("move");
}

void selectFuzzySelectTool(Application& app) {
    LOG_INFO("Action: Select Fuzzy Select Tool");
    app.setSelectedTool("fuzzy_select");
}

// View Actions
void zoomIn(Application& app) {
    LOG_INFO("Action: Zoom In");
    app.getCamera().zoom(1.1f);
}

void zoomOut(Application& app) {
    LOG_INFO("Action: Zoom Out");
    app.getCamera().zoom(0.9f);
}

void resetZoom(Application& app) {
    LOG_INFO("Action: Reset Zoom");
    app.getCamera().resetZoom();
}

// App Actions
void openPreferences(Application& app) {
    LOG_INFO("Action: Open Preferences");
    app.openPreferences();
}

// Register all actions
void registerAllActions() {
    auto& registry = ActionRegistry::instance();
    
    // File
    registry.registerAction("file.new", newFile);
    registry.registerAction("file.open", openFile);
    registry.registerAction("file.load", loadFile);
    registry.registerAction("file.save", saveFile);
    registry.registerAction("file.save_as", saveFileAs);
    registry.registerAction("file.export", exportFile);
    
    // Edit
    registry.registerAction("edit.undo", undo);
    registry.registerAction("edit.redo", redo);
    registry.registerAction("edit.cut", cut);
    registry.registerAction("edit.copy", copy);
    registry.registerAction("edit.paste", paste);
    
    // Selection
    registry.registerAction("select.all", selectAll);
    registry.registerAction("select.none", deselectAll);
    
    // Tools
    registry.registerAction("tool.pencil", selectPencilTool);
    registry.registerAction("tool.eraser", selectEraserTool);
    registry.registerAction("tool.selection", selectSelectionTool);
    registry.registerAction("tool.move", selectMoveTool);
    registry.registerAction("tool.fuzzy_select", selectFuzzySelectTool);
    
    // View
    registry.registerAction("view.zoom_in", zoomIn);
    registry.registerAction("view.zoom_out", zoomOut);
    registry.registerAction("view.zoom_reset", resetZoom);
    
    // App
    registry.registerAction("app.preferences", openPreferences);
    
    LOG_INFO("Registered all application actions");
}

} // namespace Actions
} // namespace Gump
