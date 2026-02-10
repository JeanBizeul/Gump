#include "UI/UI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Application.hpp"

void Gump::UI::renderBrushSettings(Gump::Application &app) {
    // ImGuiWindowClass windowClass;
    // windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
    // ImGui::SetNextWindowClass(&windowClass);
    ImGui::Begin("Brush Settings");

    auto& brushSettings = app.getBrushSettings();

    ImGui::SeparatorText("Brush Properties");

    // Brush shape selector
    const char* shapeNames[] = { "Circle", "Square", "Texture" };
    int currentShape = static_cast<int>(brushSettings.shape);
    if (ImGui::Combo("Shape", &currentShape, shapeNames, IM_ARRAYSIZE(shapeNames))) {
        brushSettings.shape = static_cast<BrushSettings::Shape>(currentShape);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Brush shape\nCircle: Round procedural brush\nSquare: Square procedural brush\nTexture: Use loaded brush texture");
    }

    // Brush size slider
    ImGui::SliderFloat("Size", &brushSettings.size, 1.0f, 100.0f, "%.1f px");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Brush diameter in pixels");
    }

    // Brush hardness slider
    ImGui::SliderFloat("Hardness", &brushSettings.hardness, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Edge hardness\n0 = soft brush, 1 = hard brush");
    }

    // Brush opacity slider
    ImGui::SliderFloat("Opacity", &brushSettings.opacity, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Brush opacity\n0 = transparent, 1 = opaque");
    }

    // Brush spacing slider
    ImGui::SliderFloat("Spacing", &brushSettings.spacing, 0.01f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Spacing between brush stamps\nRelative to brush size (0.1 = 10%% of size)");
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Color");

    // Color picker
    ImGui::ColorEdit4("Brush Color", &brushSettings.color.r, 
        ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_DisplayRGB);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose brush color and alpha");
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Display current tool info
    const std::string& currentTool = app.getSelectedTool();
    if (currentTool == "pencil") {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "Pencil Tool Active");
        ImGui::Text("Left click and drag to draw");
    } else if (currentTool == "eraser") {
        ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.4f, 1.0f), "Eraser Tool Active");
        ImGui::Text("Left click and drag to erase");
    } else {
        ImGui::TextDisabled("Select pencil or eraser tool");
    }

    // Show active stroke info if drawing
    if (app.hasActiveStroke()) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "Drawing in progress...");
        auto& stroke = app.getCurrentStroke();
        if (stroke) {
            ImGui::Text("Points: %zu", stroke->getPointCount());
        }
    }

    ImGui::End();
}