#include "UI/UI.hpp"

#include <imgui.h>

#include "Application.hpp"

#include "Logger.hpp"

inline static std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(wstr);
}


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
            if (ImGui::MenuItem("Export", "Ctrl+E")) {
                // Handle export
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

        if (ImGui::BeginMenu("Image")) {
            if (ImGui::MenuItem("Import", "Ctrl+I")) {
                const std::wstring filepath = Gump::UI::openFilePickerDialog("Import Image", "*.*");

                if (!filepath.empty()) {
                    LOG_INFO("Import image: {}", filepath);
                    if (app.getTextureAtlas().addImageFromFile(filepath)) {
                        LOG_INFO("Image imported successfully");
                    } else {
                        LOG_ERROR("Failed to import image");
                    }
                    LOG_DEBUG("Current texture atlas page count: {}", app.getTextureAtlas().getPageCount());
                }
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
