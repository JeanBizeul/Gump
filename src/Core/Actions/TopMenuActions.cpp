#include "Actions/TopMenuActions.hpp"

#include "Application.hpp"
#include "Utils/Utils.hpp"

#include "Logger.hpp"
#include "TopMenuActions.hpp"

using namespace Gump;

static std::string ensureExtension(const std::string& path, const std::string& ext)
{
    // ext must begin with "."
    std::string dotExt = ext;
    if (dotExt.front() != '.')
        dotExt = "." + dotExt;

    // Find last dot after the last slash → checks if user already provided an extension
    size_t slashPos = path.find_last_of("/\\");
    size_t dotPos   = path.find_last_of('.');

    bool hasExt = (dotPos != std::string::npos && (slashPos == std::string::npos || dotPos > slashPos));

    if (!hasExt)
        return path + dotExt;

    return path;
}


std::expected<void, std::string> Actions::TopMenu::newFile(Application &app)
{
    LOG_DEBUG("Action: New File");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::openFile(Application &app)
{
    LOG_DEBUG("Action: Open File");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::saveFile(Application &app)
{
    LOG_DEBUG("Action: Save File");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::exportFile(Application &app)
{
    std::string filepath = Utils::saveFilePickerDialog("Save Image", "PNG Files (*.png)\0*.png\0");

    if (filepath.empty()) {
        LOG_INFO("Export cancelled by user");
        return std::unexpected(std::string("Export cancelled by user"));
    } else {
        filepath = ensureExtension(filepath, ".png");
     Utils::exportPNG(filepath, app.getLayers(), app.getShader(), app.getTextureAtlas(), app.getWindowSize());
    }
    return {};
}

std::expected<void, std::string> Gump::Actions::TopMenu::resizeCanva(Application &app)
{
    LOG_DEBUG("Action: Resize Canvas");
    app.getResizeCanvasRequest().isRequested = true;
    return {};
}

std::expected<void, std::string> Actions::TopMenu::exitApplication(Application &app)
{
    LOG_DEBUG("Action: Exit Application");
    app.stop();
    return {};
}

std::expected<void, std::string> Actions::TopMenu::undoAction(Application &app)
{
    LOG_DEBUG("Action: Undo");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::redoAction(Application &app)
{
    LOG_DEBUG("Action: Redo");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::cutAction(Application &app)
{
    LOG_DEBUG("Action: Cut");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::copyAction(Application &app)
{
    LOG_DEBUG("Action: Copy");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::pasteAction(Application &app)
{
    LOG_DEBUG("Action: Paste");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::importImage(Application &app)
{
    const std::string filepath = Gump::Utils::openFilePickerDialog("Import Image", "PNG Files (*.png)\0*.png\0");

    if (!filepath.empty()) {
        LOG_INFO("Importing image: {}", filepath);
        if (app.getTextureAtlas().addImageFromFile(filepath)) {
            LOG_INFO("Done");
            LOG_DEBUG("Creating new Layer from imported image");
            auto imageDataOpt = app.getTextureAtlas().getImageData(filepath);
            auto textureOpt = app.getTextureAtlas().getUVRect(filepath);
            if (imageDataOpt) {
                const auto &imageData = imageDataOpt->get();

                // Check if image is larger than current canvas
                glm::uvec2 currentCanvasSize = app.getCanvasSize();
                bool needsResize = imageData.width > currentCanvasSize.x ||
                                   imageData.height > currentCanvasSize.y;

                if (needsResize) {
                    // Calculate new canvas size (max of current and new image)
                    glm::uvec2 newCanvasSize = glm::uvec2(
                        std::max(static_cast<unsigned int>(imageData.width), currentCanvasSize.x),
                        std::max(static_cast<unsigned int>(imageData.height), currentCanvasSize.y)
                    );

                    // Set up pending import for UI to handle
                    auto& pending = app.getPendingImport();
                    pending.isPending = true;
                    pending.layerName = "L " + std::to_string(app.getLayerCount() + 1);
                    pending.width = imageData.width;
                    pending.height = imageData.height;
                    pending.uvMin = textureOpt->uvMin;
                    pending.uvMax = textureOpt->uvMax;
                    pending.texturePageIndex = textureOpt->pageIndex;
                    pending.suggestedCanvasSize = newCanvasSize;

                    LOG_INFO("Image ({}x{}) is larger than canvas ({}x{}). User prompt required.",
                        imageData.width, imageData.height,
                        currentCanvasSize.x, currentCanvasSize.y);
                } else {
                    // Image fits in current canvas, add layer directly
                    std::string name = "L " + std::to_string(app.getLayerCount() + 1);
                    app.addLayer(name,
                        static_cast<size_t>(imageData.width),
                        static_cast<size_t>(imageData.height),
                        textureOpt->uvMin,
                        textureOpt->uvMax,
                        textureOpt->pageIndex);
                    LOG_INFO("New layer created for image: {}", filepath);
                    LOG_DEBUG("Layer name: {}, size: {}x{}, texture page index: {}",
                        name,
                        imageData.width,
                        imageData.height,
                        textureOpt->pageIndex);
                }
            }
        } else {
            LOG_ERROR("Failed to import image");
        }
    }
    return {};
}

std::expected<void, std::string> Actions::TopMenu::aboutDialog(Application &app)
{
    LOG_DEBUG("Action: About Dialog");
    return {};
}

std::expected<void, std::string> Actions::TopMenu::dumpTextureAtlas(Application &app)
{
    LOG_INFO("Dumping Texture Atlas...");
    app.getTextureAtlas().dumpAtlas("texture_atlas_dump.png");
    return {};
}
