#pragma once

#include <string>

namespace Gump {
class Application;

void renderUI(Gump::Application &app);

namespace UI {
    void setupDockLayout(int dockspaceId);
    void renderTopMenu(Gump::Application &app);
    void renderTools(Gump::Application &app);
    void renderToolSettings(Gump::Application &app);
    void renderBrushSettings(Gump::Application &app);
    void renderLayers(Gump::Application &app);

    std::wstring openFilePickerDialog(const std::string& title = "Select a file",
        const std::string& filter = "*.*");
}
}
