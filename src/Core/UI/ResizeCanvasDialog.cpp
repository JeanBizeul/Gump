#include "UI/UI.hpp"

#include <imgui/imgui.h>

#include "Application.hpp"
#include "Logger.hpp"

void Gump::UI::renderResizeCanvasDialog(Gump::Application &app)
{
    auto& pending = app.getPendingImport();
    
    if (!pending.isPending) {
        return;
    }
    
    // Open popup on first frame when isPending is set
    static bool popupOpened = false;
    if (!popupOpened) {
        ImGui::OpenPopup("Resize Canvas?");
        popupOpened = true;
    }
    
    // Center the popup
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (ImGui::BeginPopupModal("Resize Canvas?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        auto currentSize = app.getCanvasSize();
        
        ImGui::Text("The imported image is larger than the current canvas.");
        ImGui::Spacing();
        ImGui::Text("Current canvas size: %ux%u", currentSize.x, currentSize.y);
        ImGui::Text("Image size: %zux%zu", pending.width, pending.height);
        ImGui::Text("Suggested canvas size: %ux%u", 
            pending.suggestedCanvasSize.x, pending.suggestedCanvasSize.y);
        ImGui::Spacing();
        ImGui::Text("Would you like to resize the canvas to fit the image?");
        ImGui::Spacing();
        
        ImGui::Separator();
        
        // Buttons
        if (ImGui::Button("Resize Canvas", ImVec2(150, 0))) {
            // Resize canvas and add layer
            app.setCanvasSize(pending.suggestedCanvasSize);
            app.addLayer(pending.layerName, pending.width, pending.height,
                pending.uvMin, pending.uvMax, pending.texturePageIndex);
            
            LOG_INFO("Canvas resized to {}x{} and layer added", 
                pending.suggestedCanvasSize.x, pending.suggestedCanvasSize.y);
            
            // Clear pending state
            pending.isPending = false;
            popupOpened = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Keep Canvas Size", ImVec2(150, 0))) {
            // Add layer without resizing canvas (image may be clipped)
            app.addLayer(pending.layerName, pending.width, pending.height,
                pending.uvMin, pending.uvMax, pending.texturePageIndex);
            
            LOG_INFO("Layer added without resizing canvas (may be clipped)");
            
            // Clear pending state
            pending.isPending = false;
            popupOpened = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(100, 0))) {
            // Don't add the layer
            LOG_INFO("Import cancelled by user");
            
            // Clear pending state
            pending.isPending = false;
            popupOpened = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    } else {
        // If popup is closed by other means, reset the flag
        if (popupOpened) {
            popupOpened = false;
            pending.isPending = false;
        }
    }
}
