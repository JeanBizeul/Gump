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
    void renderResizeCanvasDialog(Gump::Application &app);
    void renderManualResizeDialog(Gump::Application &app);
}
}
