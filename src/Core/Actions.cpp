#include "Actions.hpp"
#include "ActionRegistry.hpp"
#include "Application.hpp"
#include "Logger.hpp"
#include "Actions/TopMenuActions.hpp"

#include <fstream>
#include <filesystem>
#include <vector>
#include <cstring>
#include "Utils/Utils.hpp"

// Use libzip for compression
#include <zip.h>

// Forward declare stb_image_write functions (implementation is in another TU)
extern "C" {
    int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
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
    
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();

    // Create zip source from buffer
    zip_source_t* source = zip_source_buffer(archive, buffer.data(), fileSize, 0);
    if (source == nullptr) {
        LOG_ERROR("Failed to create zip source: {}", zip_strerror(archive));
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

    // Flip pixels vertically (OpenGL reads bottom-to-top)
    std::vector<unsigned char> flippedPixels(layerWidth * layerHeight * 4);
    for (int y = 0; y < layerHeight; y++) {
        std::memcpy(
            &flippedPixels[y * layerWidth * 4],
            &pixels[(layerHeight - 1 - y) * layerWidth * 4],
            layerWidth * 4
        );
    }

    // Write PNG using stb_image_write
    if (!stbi_write_png(filepath.c_str(), layerWidth, layerHeight, 4, flippedPixels.data(), layerWidth * 4)) {
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
    configFile << "LayerCount=" << layers.size() << "\n\n";

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
        configFile << "File=" << layerFilename << "\n\n";
        
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
