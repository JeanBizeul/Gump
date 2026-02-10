#include "UI/UI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Application.hpp"

static void renderMoveToolSettings(Gump::Application &app);
static void renderSelectionToolSettings(Gump::Application &app);
static void renderFuzzySelectToolSettings(Gump::Application &app);
static void renderEffectsSettings(Gump::Application &app);

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

    // Show effects when selection exists
    if (app.getSelectionState().hasSelection) {
        ImGui::Separator();
        renderEffectsSettings(app);
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

static void renderEffectsSettings(Gump::Application &app)
{
    ImGui::SeparatorText("Effects");

    auto& effects = app.getEffects();
    int selectedIndex = app.getSelectedEffectIndex();

    // Effect selection list
    if (ImGui::BeginListBox("##EffectsList", ImVec2(-1, 150))) {
        for (size_t i = 0; i < effects.size(); i++) {
            bool isSelected = (selectedIndex == static_cast<int>(i));
            if (ImGui::Selectable(effects[i]->getName().c_str(), isSelected)) {
                app.setSelectedEffectIndex(static_cast<int>(i));
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    // Show parameters and apply button for selected effect
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(effects.size())) {
        auto& selectedEffect = effects[selectedIndex];
        auto& params = selectedEffect->getParameters();

        ImGui::Spacing();
        ImGui::Text("Parameters:");
        ImGui::Separator();

        // Render parameter controls
        for (auto& param : params) {
            switch (param.type) {
                case Gump::EffectParameter::Type::Float:
                    ImGui::SliderFloat(param.name.c_str(), &param.floatValue,
                                      param.minValue, param.maxValue, "%.2f");
                    break;

                case Gump::EffectParameter::Type::Int:
                    ImGui::SliderInt(param.name.c_str(), &param.intValue,
                                    static_cast<int>(param.minValue),
                                    static_cast<int>(param.maxValue));
                    break;

                case Gump::EffectParameter::Type::Bool:
                    ImGui::Checkbox(param.name.c_str(), &param.boolValue);
                    break;

                case Gump::EffectParameter::Type::Color:
                    ImGui::ColorEdit4(param.name.c_str(), param.colorValue,
                                     ImGuiColorEditFlags_AlphaPreviewHalf);
                    break;
            }
        }

        ImGui::Spacing();

        // Apply button
        if (ImGui::Button("Apply Effect", ImVec2(-1, 0))) {
            app.applySelectedEffect();
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Apply '%s' to the current selection", selectedEffect->getName().c_str());
        }
    } else {
        ImGui::TextDisabled("Select an effect from the list");
    }
}
