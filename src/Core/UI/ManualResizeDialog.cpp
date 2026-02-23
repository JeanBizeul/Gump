#include "UI/UI.hpp"

#include <imgui/imgui.h>

#include "Application.hpp"
#include "Logger.hpp"

void Gump::UI::renderManualResizeDialog(Gump::Application& app) {
    auto& resizeRequest = app.getResizeCanvasRequest();
    if (!resizeRequest.isRequested) {
        return;
    }

    // State for the dialog
    static int newWidth = 800;
    static int newHeight = 600;
    static bool dialogOpen = false;
    
    // Initialize dialog values from current canvas size when first opened
    if (resizeRequest.isRequested && !dialogOpen) {
        auto currentSize = app.getCanvasSize();
        newWidth = currentSize.x;
        newHeight = currentSize.y;
        dialogOpen = true;
    }

    // Center the dialog window
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Always);

    bool isOpen = true;
    if (ImGui::Begin("Canvas Size", &isOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
        
        ImGui::Text("Enter the size for your canvas:");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Width input
        ImGui::Text("Width:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputInt("##Width", &newWidth);
        newWidth = glm::clamp(newWidth, 1, 8192);

        // Height input
        ImGui::Text("Height:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputInt("##Height", &newHeight);
        newHeight = glm::clamp(newHeight, 1, 8192);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Buttons
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 220) * 0.5f);
        if (ImGui::Button("Create", ImVec2(100, 0))) {
            // Set the new canvas size
            app.setCanvasSize(glm::uvec2(newWidth, newHeight));
            LOG_INFO("Canvas resized to {}x{}", newWidth, newHeight);
            
            // Create a default layer if there are no layers
            if (app.getLayerCount() == 0) {
                app.addEmptyLayer("Layer 1", newWidth, newHeight);
                LOG_INFO("Created default layer");
            }
            
            // Close dialog
            resizeRequest.isRequested = false;
            dialogOpen = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(100, 0))) {
            // If there are no layers (new file was cancelled), create a default canvas
            if (app.getLayerCount() == 0) {
                app.setCanvasSize(glm::uvec2(800, 600));
                app.addEmptyLayer("Layer 1", 800, 600);
                LOG_INFO("New file cancelled, created default 800x600 canvas");
            }
            
            resizeRequest.isRequested = false;
            dialogOpen = false;
        }
    }
    ImGui::End();
    
    // Handle dialog close button (X)
    if (!isOpen) {
        // If there are no layers (new file was cancelled), create a default canvas
        if (app.getLayerCount() == 0) {
            app.setCanvasSize(glm::uvec2(800, 600));
            app.addEmptyLayer("Layer 1", 800, 600);
            LOG_INFO("New file cancelled via close button, created default 800x600 canvas");
        }
        
        resizeRequest.isRequested = false;
        dialogOpen = false;
    }
}
