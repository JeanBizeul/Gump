#include "UI/UI.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "Logger.hpp"

#include "Application.hpp"

void Gump::UI::renderLayers(Gump::Application &app) {
    // ImGuiWindowClass windowClass;
    // windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
    // ImGui::SetNextWindowClass(&windowClass);
    ImGui::Begin("Layers");

    for (size_t i = 0; i < app.getLayerCount(); i++) {
        Layer* layer = &app.getLayer(i);

        // Layer thumbnail preview
        auto texIdOpt = app.getTextureAtlas().getPageTextureID(layer->texturePageIndex);
        if (texIdOpt) {
            ImTextureID texId = (ImTextureID)(intptr_t)(*texIdOpt);
            glm::vec2 uvMin = layer->getUVMin();
            glm::vec2 uvMax = layer->getUVMax();

            // Calculate thumbnail size (maintain aspect ratio, max 48px)
            float thumbSize = 48.0f;
            float aspect = (float)layer->getWidth() / (float)layer->getHeight();
            ImVec2 thumbnailSize;

            if (aspect > 1.0f) {
                thumbnailSize = ImVec2(thumbSize, thumbSize / aspect);
            } else {
                thumbnailSize = ImVec2(thumbSize * aspect, thumbSize);
            }

            // ImGui uses standard UV coordinates (top-left is min, bottom-right is max)
            // Our layer UVs are already set up correctly, just use them directly
            ImGui::Image(texId, thumbnailSize,
                        ImVec2(uvMin.x, uvMin.y),  // Top-left UV
                        ImVec2(uvMax.x, uvMax.y)); // Bottom-right UV

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Layer preview\n%zux%zu pixels", layer->getWidth(), layer->getHeight());
            }
            ImGui::SameLine();
        }

        // Layer name
        ImGui::InputText("##Name", &layer->name);

        // Move up button
        if (!app.canLayerMoveUp(i)) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        if (ImGui::Button(("Up##" + std::to_string(i)).c_str())) {
            app.moveLayerUp(i);
        }
        if (!app.canLayerMoveUp(i)) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        // Move down button
        ImGui::SameLine();
        if (!app.canLayerMoveDown(i)) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        if (ImGui::Button(("Down##" + std::to_string(i)).c_str())) {
            app.moveLayerDown(i);
        }
        if (!app.canLayerMoveDown(i)) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,100,100,255));
        if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
            ImGui::PopStyleColor();
            app.getLayers().erase(app.getLayers().begin() + i);
            ImGui::End();
            return; // Avoid going to bad layers ids
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
        if (ImGui::Button(("Reset##" + std::to_string(i)).c_str())) {
            layer->position = {0.0f, 0.0f};
            layer->updateMesh();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Resets translations & rotations");
        }

        // Visibility checkbox
        ImGui::Checkbox(("##visible" + std::to_string(i)).c_str(), &layer->isVisible);
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Visible");
        }

        // Transparency slider
        ImGui::SameLine();
        ImGui::SliderFloat(("##transparency" + std::to_string(i)).c_str(), &layer->transparency, 0.0f, 1.0f);
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Transparency");
        }
    }

    ImGui::End();
}
