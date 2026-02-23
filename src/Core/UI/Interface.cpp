#include "UI/UI.hpp"

#include <imgui/imgui_internal.h>

#include "Logger.hpp"

void Gump::renderUI(Gump::Application &app)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
    static bool first = true;

    if (first) {
        Gump::UI::setupDockLayout(dockspaceId);
        first = false;
    }

    ImGui::DockSpaceOverViewport(dockspaceId, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

    Gump::UI::renderTopMenu(app);
    Gump::UI::renderTools(app);
    Gump::UI::renderToolSettings(app);
    Gump::UI::renderBrushSettings(app);
    Gump::UI::renderLayers(app);
    Gump::UI::renderDebugInfo(app);
    Gump::UI::renderResizeCanvasDialog(app);
    Gump::UI::renderManualResizeDialog(app);
    Gump::UI::renderPreferences(app);
}
