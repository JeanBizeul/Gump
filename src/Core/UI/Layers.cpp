#include "UI/UI.hpp"
#include "UI/UIStyle.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "Logger.hpp"

#include "Application.hpp"

void Gump::UI::renderLayers(Gump::Application &app) {
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

    // Iterate in reverse order so the top layer (last in vector) appears at the top of the UI
    for (int idx = app.getLayerCount() - 1; idx >= 0; idx--) {
        size_t i = static_cast<size_t>(idx);
        Layer* layer = &app.getLayer(i);

        ImGui::PushID(static_cast<int>(i));

        // Start a group for the entire layer item
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
        
        // Use DangerButton style for delete
        {
            auto dangerStyle = UI::StylePreset::DangerButton();
            if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                // Mark this layer for deletion instead of deleting immediately
                // We need to finish the current frame's UI rendering first
                LOG_INFO("Marking layer '{}' for deletion", layer->name);
                
                // Store the index to delete after the UI loop
                static size_t layerToDelete = static_cast<size_t>(-1);
                layerToDelete = i;
                
                // Set a flag to delete after EndChild
                ImGui::GetIO().UserData = (void*)layerToDelete;
            }
        }
        
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

        // Clear layer button with warning style
        ImGui::SameLine();
        {
            auto warningStyle = UI::StylePreset::WarningText();
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
        } // StyleScope automatically pops here
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
        
        // Get the height of the layer group for the drag handle
        float layerItemHeight = ImGui::GetItemRectSize().y;

        // Add drag handle on the right side - spans full height
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.47f, 0.84f, 0.3f)); // Blue tint
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.00f, 0.47f, 0.84f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.00f, 0.47f, 0.84f, 0.7f));
        ImGui::Button(("⋮##drag_" + std::to_string(i)).c_str(), ImVec2(20, layerItemHeight));
        ImGui::PopStyleColor(3);
        
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Drag to reorder");
        }

        // Drag-and-drop source: make this drag handle draggable
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            // Set payload to carry the layer index
            ImGui::SetDragDropPayload("LAYER_REORDER", &i, sizeof(size_t));
            ImGui::Text("Reordering: %s", layer->name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drag-and-drop target: accept drops on the entire layer row
        ImGui::SameLine(0, 0); // No spacing
        ImGui::SetCursorPosX(0); // Go back to start of line
        ImGui::InvisibleButton(("##drop_target_" + std::to_string(i)).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetItemRectSize().y));
        
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

        ImGui::PopID();
        ImGui::Separator();
    }

    // Handle layer deletion after the UI loop completes
    if (ImGui::GetIO().UserData != nullptr) {
        size_t indexToDelete = reinterpret_cast<size_t>(ImGui::GetIO().UserData);
        if (indexToDelete < app.getLayerCount()) {
            LOG_INFO("Deleting layer at index {}", indexToDelete);
            app.getLayers().erase(app.getLayers().begin() + indexToDelete);
        }
        ImGui::GetIO().UserData = nullptr; // Clear the flag
    }

    ImGui::End();
}
