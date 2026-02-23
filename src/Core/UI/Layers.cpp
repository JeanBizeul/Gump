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

    // Button to add new empty layer
    if (ImGui::Button("Add Empty Layer")) {
        // Generate unique name for new layer
        std::string newLayerName = "Layer " + std::to_string(app.getLayerCount() + 1);
        auto canvasSize = app.getCanvasSize();
        app.addEmptyLayer(newLayerName, canvasSize.x, canvasSize.y);
    }
    
    ImGui::Separator();

    // Track previous layer names to validate changes
    static std::vector<std::string> previousNames;
    if (previousNames.size() != app.getLayerCount()) {
        previousNames.clear();
        for (size_t i = 0; i < app.getLayerCount(); i++) {
            previousNames.push_back(app.getLayer(i).name);
        }
    }

    for (size_t i = 0; i < app.getLayerCount(); i++) {
        Layer* layer = &app.getLayer(i);

        ImGui::PushID(static_cast<int>(i));

        // Calculate the size of the layer item first
        float itemHeight = 80.0f; // Approximate height for the layer item
        float itemWidth = ImGui::GetContentRegionAvail().x;

        // Create an invisible button that covers the entire layer item for drag-and-drop
        ImGui::InvisibleButton(("##dragarea" + std::to_string(i)).c_str(), ImVec2(itemWidth, itemHeight));
        
        // Drag-and-drop source: make this layer draggable
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            // Set payload to carry the layer index
            ImGui::SetDragDropPayload("LAYER_REORDER", &i, sizeof(size_t));
            ImGui::Text("Reordering: %s", layer->name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drag-and-drop target: accept drops to reorder
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LAYER_REORDER")) {
                size_t draggedIdx = *(const size_t*)payload->Data;
                if (draggedIdx != i) {
                    // Swap the layers
                    std::swap(app.getLayers()[draggedIdx], app.getLayers()[i]);
                    LOG_INFO("Moved layer from {} to {}", draggedIdx, i);
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Draw the actual layer content over the invisible button
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - itemHeight);

        // Start a group for the entire layer item (for drag-and-drop)
        ImGui::BeginGroup();

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

        // Layer name input with validation
        std::string oldName = layer->name;
        ImGui::InputText(("##Name" + std::to_string(i)).c_str(), &layer->name);
        
        // Validate name change
        if (layer->name != oldName) {
            // Check if the new name is already taken by another layer
            if (app.isLayerNameTaken(layer->name, i)) {
                // Name is taken, revert to old name or generate unique name
                layer->name = app.generateUniqueLayerName(layer->name);
                LOG_WARNING("Layer name already taken, renamed to '{}'", layer->name);
            }
            previousNames[i] = layer->name;
        }
        
        // Show warning if name was changed due to conflict
        if (ImGui::IsItemDeactivatedAfterEdit() && layer->name != oldName && app.isLayerNameTaken(oldName, i)) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "!");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Name was already taken, automatically renamed");
            }
        }

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
        
        // Disable delete button if this is the only layer
        bool canDelete = app.getLayerCount() > 1;
        if (!canDelete) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,100,100,255));
        if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
            ImGui::PopStyleColor();
            if (!canDelete) {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
            app.getLayers().erase(app.getLayers().begin() + i);
            ImGui::End();
            return; // Avoid going to bad layers ids
        }
        ImGui::PopStyleColor();
        
        if (!canDelete) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            if (!canDelete) {
                ImGui::SetTooltip("Cannot delete the only layer");
            } else {
                ImGui::SetTooltip("Delete this layer");
            }
        }

        ImGui::SameLine();
        if (ImGui::Button(("Reset##" + std::to_string(i)).c_str())) {
            layer->position = {0.0f, 0.0f};
            layer->updateMesh();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Resets translations & rotations");
        }

        // Clear layer button
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 100, 255));
        if (ImGui::Button(("Clear##" + std::to_string(i)).c_str())) {
            // Get the texture page for this layer
            auto pageTexIdOpt = app.getTextureAtlas().getPageTextureID(layer->texturePageIndex);
            if (pageTexIdOpt) {
                GLuint textureID = *pageTexIdOpt;
                
                // Get texture dimensions
                glBindTexture(GL_TEXTURE_2D, textureID);
                GLint texWidth, texHeight;
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
                
                // Get layer's position in the texture atlas
                glm::vec2 layerUVMin = layer->getUVMin();
                int layerTexX = static_cast<int>(layerUVMin.x * texWidth);
                int layerTexY = static_cast<int>(layerUVMin.y * texHeight);
                
                int layerWidth = static_cast<int>(layer->getWidth());
                int layerHeight = static_cast<int>(layer->getHeight());
                
                // Create transparent pixels
                std::vector<unsigned char> transparentPixels(layerWidth * layerHeight * 4, 0);
                
                // Update the texture with transparent pixels
                glTexSubImage2D(GL_TEXTURE_2D, 0, layerTexX, layerTexY, layerWidth, layerHeight,
                               GL_RGBA, GL_UNSIGNED_BYTE, transparentPixels.data());
                
                LOG_INFO("Cleared layer '{}'", layer->name);
            } else {
                LOG_ERROR("Failed to get texture page for layer '{}'", layer->name);
            }
        }
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Clear this layer (fill with transparent pixels)");
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

        ImGui::EndGroup();

        ImGui::PopID();
        ImGui::Separator();
    }

    ImGui::End();
}
