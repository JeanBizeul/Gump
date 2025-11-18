#include "Actions/TopMenuActions.hpp"

#include "Application.hpp"
#include "Utils/Utils.hpp"

#include "Logger.hpp"

using namespace Gump;

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
    LOG_DEBUG("Action: Export File");
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
    const std::string filepath = Gump::Utils::openFilePickerDialog("Import Image", "*.*");

    if (!filepath.empty()) {
        LOG_INFO("Importing image: {}", filepath);
        if (app.getTextureAtlas().addImageFromFile(filepath)) {
            LOG_INFO("Image imported successfully");
            LOG_DEBUG("Creating new Layer from imported image");
            auto imageDataOpt = app.getTextureAtlas().getImageData(filepath);
            auto textureOpt = app.getTextureAtlas().getUVRect(filepath);
            if (imageDataOpt) {
                const auto &imageData = imageDataOpt->get();
                std::string name = "L " + std::to_string(app.getLayerCount() + 1);
                app.addLayer(name,
                    static_cast<size_t>(imageData.width),
                    static_cast<size_t>(imageData.height),
                    textureOpt->uvMin,
                    textureOpt->uvMax,
                    textureOpt->pageIndex);
                LOG_INFO("New layer created for image: {}", filepath);
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
