#pragma once

#include <string>
#include <expected>

namespace Gump {

class Application;

namespace Actions {

namespace TopMenu {

std::expected<void, std::string> newFile(Application &app);
std::expected<void, std::string> openFile(Application &app);
std::expected<void, std::string> saveFile(Application &app);
std::expected<void, std::string> exportFile(Application &app);
std::expected<void, std::string> exitApplication(Application &app);

std::expected<void, std::string> undoAction(Application &app);
std::expected<void, std::string> redoAction(Application &app);

std::expected<void, std::string> cutAction(Application &app);
std::expected<void, std::string> copyAction(Application &app);
std::expected<void, std::string> pasteAction(Application &app);

std::expected<void, std::string> importImage(Application &app);
std::expected<void, std::string> aboutDialog(Application &app);

std::expected<void, std::string> dumpTextureAtlas(Application &app);

} // namespace TopMenu

} // namespace Actions

} // namespace Gump
