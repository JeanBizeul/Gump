#include "UI/UI.hpp"

#include <imgui/imgui.h>

#include "Application.hpp"
#include "Logger.hpp"

void Gump::UI::renderManualResizeDialog(Gump::Application &app)
{
    auto& request = app.getResizeCanvasRequest();
    
    if (!request.isRequested) {
        return;
    }
    
    // Open popup on first frame when isRequested is set
    static bool popupOpened = false;
    if (!popupOpened) {
        ImGui::OpenPopup("Resize Canvas");
        popupOpened = true;
    }
    
    // Center the popup
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (ImGui::BeginPopupModal("Resize Canvas", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        auto currentSize = app.getCanvasSize();
        
        // Static variables to hold input values
        static int newWidth = 800;
        static int newHeight = 600;
        
        // Initialize with current canvas size when dialog opens
        if (ImGui::IsWindowAppearing()) {
            newWidth = static_cast<int>(currentSize.x);
            newHeight = static_cast<int>(currentSize.y);
        }
        
        ImGui::Text("Current canvas size: %ux%u", currentSize.x, currentSize.y);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Width input
        ImGui::Text("New canvas size:");
        ImGui::PushItemWidth(150);
        ImGui::InputInt("Width", &newWidth, 1, 100);
        
        // Height input
        ImGui::InputInt("Height", &newHeight, 1, 100);
        ImGui::PopItemWidth();
        
        // Clamp values to reasonable ranges
        if (newWidth < 1) newWidth = 1;
        if (newWidth > 8192) newWidth = 8192;
        if (newHeight < 1) newHeight = 1;
        if (newHeight > 8192) newHeight = 8192;
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Buttons
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            // Apply the new canvas size
            app.setCanvasSize(glm::uvec2(newWidth, newHeight));
            
            LOG_INFO("Canvas manually resized to {}x{}", newWidth, newHeight);
            
            // Clear request state
            request.isRequested = false;
            popupOpened = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            LOG_INFO("Canvas resize cancelled by user");
            
            // Clear request state
            request.isRequested = false;
            popupOpened = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    } else {
        // If popup is closed by other means, reset the flag
        if (popupOpened) {
            popupOpened = false;
            request.isRequested = false;
        }
    }
}
