#include "UI/UI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

void Gump::UI::renderBrushSettings(Gump::Application &app) {
    // ImGuiWindowClass windowClass;
    // windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
    // ImGui::SetNextWindowClass(&windowClass);
    ImGui::Begin("Brush Settings");

    ImGui::Text("Settings for brushes");
    ImGui::End();
}