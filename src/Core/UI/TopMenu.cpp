#include "UI/UI.hpp"

#include <imgui.h>

#include "Application.hpp"

#include "Logger.hpp"

#include "Actions/TopMenuActions.hpp"

void Gump::UI::renderTopMenu(Application &app)
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("file")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                auto result = Gump::Actions::TopMenu::newFile(app);
                if (!result) {
                    LOG_ERROR("Failed to create new file: {}", result.error());
                }
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                auto result = Gump::Actions::TopMenu::openFile(app);
                if (!result) {
                    LOG_ERROR("Failed to open file: {}", result.error());
                }
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                auto result = Gump::Actions::TopMenu::saveFile(app);
                if (!result) {
                    LOG_ERROR("Failed to save file: {}", result.error());
                }
            }
            if (ImGui::MenuItem("Export", "Ctrl+E")) {
                auto result = Gump::Actions::TopMenu::exportFile(app);
                if (!result) {
                    LOG_ERROR("Failed to export file: {}", result.error());
                }
            }
            if (ImGui::MenuItem("Exit")) {
                auto result = Gump::Actions::TopMenu::exitApplication(app);
                if (!result) {
                    LOG_ERROR("Failed to exit application: {}", result.error());
                }
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                auto result = Gump::Actions::TopMenu::undoAction(app);
                if (!result) {
                    LOG_ERROR("Failed to undo action: {}", result.error());
                }
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                auto result = Gump::Actions::TopMenu::redoAction(app);
                if (!result) {
                    LOG_ERROR("Failed to redo action: {}", result.error());
                }
            }

            ImGui::Separator();
            
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {
                auto result = Gump::Actions::TopMenu::cutAction(app);
                if (!result) {
                    LOG_ERROR("Failed to cut action: {}", result.error());
                }
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                auto result = Gump::Actions::TopMenu::copyAction(app);
                if (!result) {
                    LOG_ERROR("Failed to copy action: {}", result.error());
                }
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                auto result = Gump::Actions::TopMenu::pasteAction(app);
                if (!result) {
                    LOG_ERROR("Failed to paste action: {}", result.error());
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Image")) {
            if (ImGui::MenuItem("Import", "Ctrl+I")) {
                auto result = Gump::Actions::TopMenu::importImage(app);
                if (!result) {
                    LOG_ERROR("Failed to import image: {}", result.error());
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug")) {
            if (ImGui::MenuItem("Dump Texture Atlas")) {
                auto result = Gump::Actions::TopMenu::dumpTextureAtlas(app);
                if (!result) {
                    LOG_ERROR("Failed to dump texture atlas: {}", result.error());
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
