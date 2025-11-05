#include "UI/UI.hpp"

#include <imgui/imgui_internal.h>

#include "Logger.hpp"

void Gump::renderUI(Gump::Application &app)
{
    int dockspaceId = ImGui::DockSpaceOverViewport();
    static bool first = true;

    if (first) {
        Gump::UI::setupDockLayout(dockspaceId);
        first = false;
    }

    Gump::UI::renderTopMenu(app);
    Gump::UI::renderTools(app);
    Gump::UI::renderToolSettings(app);
    Gump::UI::renderBrushSettings(app);
    Gump::UI::renderLayers(app);
}
