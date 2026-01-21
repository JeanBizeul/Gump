#include "UI/UI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Application.hpp"

static void renderMoveToolSettings(Gump::Application &app);
static void renderSelectionToolSettings(Gump::Application &app);

void Gump::UI::renderToolSettings(Gump::Application &app) {
    // ImGuiWindowClass windowClass;
    // windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
    // ImGui::SetNextWindowClass(&windowClass);
    ImGui::Begin("Tool Settings");

    if (app.getSelectedTool() == "move") {
        renderMoveToolSettings(app);
    } else if (app.getSelectedTool() == "selection") {
        renderSelectionToolSettings(app);
    }
    ImGui::End();
}

static void renderMoveToolSettings(Gump::Application &app)
{
    renderSelectionToolSettings(app); // The move tool shares selection settings
}

static void renderSelectionToolSettings(Gump::Application &app)
{
    bool isThereASelection = app.getSelectionState().hasSelection;
    bool isThereALayer = app.getLayerCount() > 0;

    ImGui::BeginDisabled(!isThereALayer);

    if (ImGui::Button("Paste selection")) {
        app.pasteClipboard();
    }

    ImGui::EndDisabled();

    ImGui::BeginDisabled(!isThereASelection);

    if (ImGui::Button("Copy selection")) {
        app.copySelection();
    }

    if (ImGui::Button("Cut selection")) {
        app.cutSelection();
    }

    if (ImGui::Button("Clear selection")) {
        app.getSelectionState().hasSelection = false;
        app.updateSelectionMesh();
    }

    ImGui::EndDisabled();

    ImGui::BeginDisabled(!isThereASelection || !isThereALayer);

    if (ImGui::Button("Send selection to new layer")) {
        app.sendSelectionToNewLayer();
    }

    ImGui::EndDisabled();
}

