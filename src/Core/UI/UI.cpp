#include "UI/UI.hpp"
#include "Application.hpp"
#include "PreferencesManager.hpp"
#include "Logger.hpp"
#include <imgui.h>

void Gump::renderUI(Gump::Application &app)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
    static bool first = true;
    auto& prefMgr = app.getPreferencesManager();

    if (first) {
        Gump::UI::setupDockLayout(dockspaceId);
        first = false;
    }

    ImGui::DockSpaceOverViewport(dockspaceId, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

    // Render top menu
    Gump::UI::renderTopMenu(app);

    // Render tools panel - only if enabled
    if (prefMgr.isModuleVisible("Tools", true)) {
        Gump::UI::renderTools(app);
    }

    // Render tool settings panel - only if enabled
    if (prefMgr.isModuleVisible("ToolSettings", true)) {
        Gump::UI::renderToolSettings(app);
    }

    // Render brush settings panel - only if enabled
    if (prefMgr.isModuleVisible("BrushSettings", true)) {
        Gump::UI::renderBrushSettings(app);
    }

    // Render layers panel - only if enabled
    if (prefMgr.isModuleVisible("Layers", true)) {
        Gump::UI::renderLayers(app);
    }

    // Render debug info - only if enabled
    if (prefMgr.isModuleVisible("DebugInfo", false)) {
        Gump::UI::renderDebugInfo(app);
    }

    // Always render dialogs (they control their own visibility)
    Gump::UI::renderResizeCanvasDialog(app);
    Gump::UI::renderManualResizeDialog(app);
    Gump::UI::renderPreferences(app);
    LOG_DEBUG("A");
    Gump::UI::renderTutorials(app);
    LOG_DEBUG("B");
}