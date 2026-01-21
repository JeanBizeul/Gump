#include "UI/UI.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "Application.hpp"

void Gump::UI::renderLayers(Gump::Application &app) {
    // ImGuiWindowClass windowClass;
    // windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
    // ImGui::SetNextWindowClass(&windowClass);
    static bool popupOpen = false;
    ImGui::Begin("Layers");

    for (size_t i = 0; i < app.getLayerCount(); i++) {
        Layer* layer = &app.getLayer(i);

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

        // Layer name
        ImGui::SameLine();
        ImGui::Text("%s", layer->name.c_str());
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Right click for Layer settings");
        }
        ImGui::OpenPopupOnItemClick("Layer settings", 1);

        if (!popupOpen)
            ImGui::SetNextWindowPos({ImGui::GetMousePos().x - 300, ImGui::GetMousePos().y - 75});

        if (ImGui::BeginPopup("Layer settings")) {
            popupOpen = true;

            if (ImGui::InputText("Rename Layer", &layer->name, ImGuiInputTextFlags_EnterReturnsTrue)) {
                popupOpen = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::FocusItem();


            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,100,100,255));
            if (ImGui::Button("Close") || Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
                ImGui::CloseCurrentPopup();
                popupOpen = false;
            }
            ImGui::PopStyleColor();

            ImGui::Separator();

            if (ImGui::Button("Delete Layer")) {
                app.getLayers().erase(app.getLayers().begin() + i);
                popupOpen = false;
                ImGui::EndPopup();
                ImGui::End();
                return; // Avoid going to bad layers ids
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset Layer Transformations")) {
                layer->position = {0.0f, 0.0f};
                layer->updateMesh();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Resets translations & rotations");
            }

            ImGui::EndPopup();
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
