#include "UI/UI.hpp"
#include "Application.hpp"

#include <imgui.h>

void Gump::UI::renderTopMenu(Application &app)
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("file")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                // Create new file
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                // Open save file
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                // Handle save
            }
            if (ImGui::MenuItem("Exit")) {
                app.stop();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {

            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {

            }

            ImGui::Separator();
            
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {

            }
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {

            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {

            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
