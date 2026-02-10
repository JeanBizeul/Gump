#include "UI/UI.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Application.hpp"

void Gump::UI::renderBrushSettings(Gump::Application &app) {
    ImGui::Begin("Brush Settings");

    auto& brushSettings = app.getBrushSettings();

    ImGui::SeparatorText("Brush Properties");

    // Color picker
    ImGui::ColorEdit4("Color", &brushSettings.color.r);

    // Brush size slider
    ImGui::SliderFloat("Size", &brushSettings.baseSize, 1.0f, 100.0f, "%.1f px");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Base brush size in pixels");
    }

    // Hardness slider
    ImGui::SliderFloat("Hardness", &brushSettings.hardness, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("0.0 = soft, 1.0 = hard edge");
    }

    // Opacity slider
    ImGui::SliderFloat("Opacity", &brushSettings.opacity, 0.0f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Brush opacity (transparency)");
    }

    // Spacing slider
    ImGui::SliderFloat("Spacing", &brushSettings.spacing, 0.01f, 1.0f, "%.2f");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Distance between brush dabs as ratio of size\nLower = smoother, Higher = faster");
    }

    ImGui::End();
}