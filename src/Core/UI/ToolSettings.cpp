#include "UI/UI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Application.hpp"

static void renderMoveToolSettings(Gump::Application &app);
static void renderSelectionToolSettings(Gump::Application &app);
static void renderFuzzySelectToolSettings(Gump::Application &app);

void Gump::UI::renderToolSettings(Gump::Application &app) {
    // ImGuiWindowClass windowClass;
    // windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
    // ImGui::SetNextWindowClass(&windowClass);
    ImGui::Begin("Tool Settings");

    if (app.getSelectedTool() == "move") {
        renderMoveToolSettings(app);
    } else if (app.getSelectedTool() == "selection") {
        renderSelectionToolSettings(app);
    } else if (app.getSelectedTool() == "fuzzy-select") {
        renderFuzzySelectToolSettings(app);
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

    ImGui::Text("Selection Modifiers:");

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
        app.getSelectionState().clearMask();
        app.updateSelectionMesh();
    }

    ImGui::EndDisabled();

    ImGui::BeginDisabled(!isThereASelection || !isThereALayer);

    if (ImGui::Button("Send selection to new layer")) {
        app.sendSelectionToNewLayer();
    }

    ImGui::EndDisabled();
}

static void renderFuzzySelectToolSettings(Gump::Application &app)
{
    auto& settings = app.getFuzzySelectSettings();
    bool isThereASelection = app.getSelectionState().hasSelection;
    bool isThereALayer = app.getLayerCount() > 0;

    ImGui::SeparatorText("Fuzzy Select Settings");

    // Tolerance slider
    ImGui::SliderFloat("Tolerance", &settings.tolerance, 0.0f, 255.0f, "%.0f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Color similarity threshold (0-255).\nLower = stricter matching, Higher = more similar colors selected");
    }

    // Contiguous mode checkbox
    ImGui::Checkbox("Contiguous", &settings.contiguous);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Only select connected pixels of similar color.\nUncheck to select all similar colors in the image");
    }

    // Anti-aliasing checkbox
    ImGui::Checkbox("Anti-alias", &settings.antiAlias);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Smooth the edges of the selection");
    }

    // Sample merged checkbox
    ImGui::Checkbox("Sample merged", &settings.sampleMerged);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Sample colors from all visible layers.\nUncheck to sample only from the active layer");
    }

    ImGui::Separator();

    renderSelectionToolSettings(app);
}
