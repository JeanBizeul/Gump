#include "UI/UI.hpp"
#include "UI/UIStyle.hpp"

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "Logger.hpp"

#include "Application.hpp"
#include "Input.hpp"
#include <GLFW/glfw3.h>
#include <sstream>

enum class PreferenceSection {
    Shortcuts,
    Style,
    Modules
};

// State for shortcut editing
struct ShortcutEditState {
    bool isEditing = false;
    std::string editingActionId = "";
    int capturedKey = -1;
    Gump::KeyModifier capturedModifiers = Gump::KeyModifier::None;
};

static ShortcutEditState s_shortcutEditState;

void Gump::UI::renderPreferences(Gump::Application &app) {
    if (!app.isPreferencesOpen()) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    
    // Custom close behavior to save preferences
    bool prefsOpen = true;
    if (ImGui::Begin("Preferences", &prefsOpen, ImGuiWindowFlags_NoCollapse)) {
        
        // If window was closed via X button, save preferences
        if (!prefsOpen) {
            app.closePreferences();
        }
        
        // State for selected section and search
        static PreferenceSection selectedSection = PreferenceSection::Shortcuts;
        static std::string searchQuery = "";
        
        // Left sidebar - Navigation tree with search
        ImGui::BeginChild("PreferencesNav", ImVec2(250, 0), true);
        
        // Search bar
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputTextWithHint("##PreferencesSearch", "Search settings...", &searchQuery)) {
            // Filter tree based on search query (just update the search string for now)
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Navigation tree
        bool showShortcuts = searchQuery.empty() || 
            std::string("shortcuts").find(searchQuery) != std::string::npos ||
            std::string("keybindings").find(searchQuery) != std::string::npos;
        bool showStyle = searchQuery.empty() || 
            std::string("style").find(searchQuery) != std::string::npos ||
            std::string("theme").find(searchQuery) != std::string::npos ||
            std::string("appearance").find(searchQuery) != std::string::npos;
        bool showModules = searchQuery.empty() || 
            std::string("modules").find(searchQuery) != std::string::npos ||
            std::string("windows").find(searchQuery) != std::string::npos ||
            std::string("panels").find(searchQuery) != std::string::npos;
        
        // Shortcuts section
        if (showShortcuts) {
            if (ImGui::TreeNodeEx("Shortcuts", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Selectable("Keybindings", selectedSection == PreferenceSection::Shortcuts)) {
                    selectedSection = PreferenceSection::Shortcuts;
                }
                ImGui::TreePop();
            }
        }
        
        // Style section
        if (showStyle) {
            if (ImGui::TreeNodeEx("Style", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Selectable("Appearance", selectedSection == PreferenceSection::Style)) {
                    selectedSection = PreferenceSection::Style;
                }
                ImGui::TreePop();
            }
        }
        
        // Modules section
        if (showModules) {
            if (ImGui::TreeNodeEx("Modules / Windows", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (ImGui::Selectable("Display Elements", selectedSection == PreferenceSection::Modules)) {
                    selectedSection = PreferenceSection::Modules;
                }
                ImGui::TreePop();
            }
        }
        
        // Show message if search has no results
        if (!showShortcuts && !showStyle && !showModules && !searchQuery.empty()) {
            ImGui::Spacing();
            ImGui::TextDisabled("No results found");
        }
        
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        // Right panel - Settings content
        ImGui::BeginChild("PreferencesContent", ImVec2(0, 0), false);
        
        switch (selectedSection) {
            case PreferenceSection::Shortcuts: {
                ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Shortcuts");
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Text("Configure keyboard shortcuts for all actions");
                ImGui::Spacing();
                
                // Capture keyboard input when editing a shortcut
                if (s_shortcutEditState.isEditing) {
                    // Get current modifiers
                    KeyModifier currentMods = KeyModifier::None;
                    if (Input::isKeyHeld(GLFW_KEY_LEFT_CONTROL) || Input::isKeyHeld(GLFW_KEY_RIGHT_CONTROL)) {
                        currentMods = currentMods | KeyModifier::Ctrl;
                    }
                    if (Input::isKeyHeld(GLFW_KEY_LEFT_SHIFT) || Input::isKeyHeld(GLFW_KEY_RIGHT_SHIFT)) {
                        currentMods = currentMods | KeyModifier::Shift;
                    }
                    if (Input::isKeyHeld(GLFW_KEY_LEFT_ALT) || Input::isKeyHeld(GLFW_KEY_RIGHT_ALT)) {
                        currentMods = currentMods | KeyModifier::Alt;
                    }
                    if (Input::isKeyHeld(GLFW_KEY_LEFT_SUPER) || Input::isKeyHeld(GLFW_KEY_RIGHT_SUPER)) {
                        currentMods = currentMods | KeyModifier::Super;
                    }
                    
                    s_shortcutEditState.capturedModifiers = currentMods;
                    
                    // Check for key press (exclude modifier keys themselves)
                    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; key++) {
                        // Skip modifier keys
                        if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT ||
                            key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL ||
                            key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT ||
                            key == GLFW_KEY_LEFT_SUPER || key == GLFW_KEY_RIGHT_SUPER) {
                            continue;
                        }
                        
                        if (Input::isKeyPressed(key)) {
                            s_shortcutEditState.capturedKey = key;
                            
                            // Check if combination is already used
                            std::string usedBy = app.getShortcutManager().isKeyCombinationUsed(
                                key, currentMods, s_shortcutEditState.editingActionId);
                            
                            if (!usedBy.empty()) {
                                LOG_WARNING("Key combination already used by: {}", usedBy);
                                // TODO: Show warning dialog
                            } else {
                                // Update the shortcut
                                app.getShortcutManager().updateShortcut(
                                    s_shortcutEditState.editingActionId, key, currentMods);
                            }
                            
                            // Stop editing
                            s_shortcutEditState.isEditing = false;
                            s_shortcutEditState.editingActionId = "";
                            break;
                        }
                    }
                    
                    // Cancel with Escape
                    if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
                        s_shortcutEditState.isEditing = false;
                        s_shortcutEditState.editingActionId = "";
                    }
                }
                
                // Table for shortcuts
                if (ImGui::BeginTable("ShortcutsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                    ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Shortcut", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                    ImGui::TableHeadersRow();
                    
                    auto& shortcuts = app.getShortcutManager().getShortcuts();
                    for (size_t i = 0; i < shortcuts.size(); i++) {
                        const auto& shortcut = shortcuts[i];
                        
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", shortcut.actionName.c_str());
                        
                        ImGui::TableSetColumnIndex(1);
                        
                        // Show shortcut button or "Press key(s)..." when editing
                        bool isEditingThis = s_shortcutEditState.isEditing && 
                                            s_shortcutEditState.editingActionId == shortcut.actionId;
                        
                        std::string buttonLabel;
                        if (isEditingThis) {
                            // Show current modifiers while editing
                            std::stringstream modDisplay;
                            bool hasAnyMod = false;
                            
                            if (hasModifier(s_shortcutEditState.capturedModifiers, KeyModifier::Ctrl)) {
                                modDisplay << "Ctrl";
                                hasAnyMod = true;
                            }
                            if (hasModifier(s_shortcutEditState.capturedModifiers, KeyModifier::Shift)) {
                                if (hasAnyMod) modDisplay << "+";
                                modDisplay << "Shift";
                                hasAnyMod = true;
                            }
                            if (hasModifier(s_shortcutEditState.capturedModifiers, KeyModifier::Alt)) {
                                if (hasAnyMod) modDisplay << "+";
                                modDisplay << "Alt";
                                hasAnyMod = true;
                            }
                            if (hasModifier(s_shortcutEditState.capturedModifiers, KeyModifier::Super)) {
                                if (hasAnyMod) modDisplay << "+";
                                modDisplay << "Super";
                                hasAnyMod = true;
                            }
                            
                            if (hasAnyMod) {
                                buttonLabel = modDisplay.str() + "+...";
                            } else {
                                buttonLabel = "Press key(s)...";
                            }
                        } else {
                            buttonLabel = shortcut.toString();
                        }
                        
                        ImGui::SetNextItemWidth(-1);
                        if (ImGui::Button((buttonLabel + "##" + shortcut.actionId).c_str(), ImVec2(-1, 0))) {
                            if (!isEditingThis) {
                                // Start editing this shortcut
                                s_shortcutEditState.isEditing = true;
                                s_shortcutEditState.editingActionId = shortcut.actionId;
                                s_shortcutEditState.capturedKey = -1;
                                s_shortcutEditState.capturedModifiers = KeyModifier::None;
                            }
                        }
                        
                        if (isEditingThis) {
                            ImGui::SetItemTooltip("Press Escape to cancel");
                        }
                        
                        ImGui::TableSetColumnIndex(2);
                        
                        // Reset to default button
                        if (ImGui::SmallButton(("R##reset_" + shortcut.actionId).c_str())) {
                            app.getShortcutManager().resetShortcutToDefault(shortcut.actionId);
                        }
                        if (ImGui::IsItemHovered()) {
                            // Show what the default is in the tooltip
                            const auto* defaultShortcut = app.getShortcutManager().getDefaultShortcut(shortcut.actionId);
                            if (defaultShortcut) {
                                ImGui::SetTooltip("Reset to default: %s", defaultShortcut->toString().c_str());
                            } else {
                                ImGui::SetTooltip("Reset to default");
                            }
                        }
                        
                        ImGui::SameLine();
                        
                        // Clear shortcut button
                        if (ImGui::SmallButton(("X##clear_" + shortcut.actionId).c_str())) {
                            // Unbind shortcut completely
                            app.getShortcutManager().updateShortcut(shortcut.actionId, -1, KeyModifier::None);
                            LOG_INFO("Unbound shortcut: {}", shortcut.actionName);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Unbind shortcut");
                        }
                    }
                    
                    ImGui::EndTable();
                }
                
                ImGui::Spacing();
                if (ImGui::Button("Reset to Defaults")) {
                    app.getShortcutManager().resetToDefaults();
                    LOG_INFO("Reset shortcuts to defaults");
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Save Now")) {
                    app.getPreferencesManager().saveShortcuts(app.getShortcutManager());
                    app.getPreferencesManager().saveToFile();
                    LOG_INFO("Preferences saved");
                }
                
                break;
            }
                
            case PreferenceSection::Style: {
                ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Style & Appearance");
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Text("Customize the look and feel of the application");
                ImGui::Spacing();
                
                // Theme selection
                ImGui::Text("Theme:");
                ImGui::SameLine();
                static int currentTheme = 0;
                const char* themes[] = { "Professional (Light)", "Dark", "Classic", "High Contrast", "Custom" };
                if (ImGui::Combo("##Theme", &currentTheme, themes, IM_ARRAYSIZE(themes))) {
                    LOG_INFO("Theme changed to: {}", themes[currentTheme]);
                    // TODO: Apply theme
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Color customization
                ImGui::Text("Colors:");
                ImGui::Spacing();
                
                static ImVec4 bgColor = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
                static ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                static ImVec4 accentColor = ImVec4(0.0f, 0.47f, 0.84f, 1.0f);
                static ImVec4 buttonColor = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
                
                ImGui::ColorEdit3("Background Color", (float*)&bgColor);
                ImGui::ColorEdit3("Text Color", (float*)&textColor);
                ImGui::ColorEdit3("Accent Color", (float*)&accentColor);
                ImGui::ColorEdit3("Button Color", (float*)&buttonColor);
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Style parameters
                ImGui::Text("Style Parameters:");
                ImGui::Spacing();
                
                static float rounding = 0.0f;
                static float spacing = 8.0f;
                static float padding = 8.0f;
                
                ImGui::SliderFloat("Window Rounding", &rounding, 0.0f, 12.0f);
                ImGui::SliderFloat("Item Spacing", &spacing, 2.0f, 20.0f);
                ImGui::SliderFloat("Window Padding", &padding, 2.0f, 20.0f);
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Import/Export buttons
                ImGui::Text("Theme Management:");
                ImGui::Spacing();
                
                if (ImGui::Button("Export Theme...")) {
                    LOG_INFO("Export theme");
                    // TODO: Open file dialog to save theme
                }
                ImGui::SameLine();
                if (ImGui::Button("Import Theme...")) {
                    LOG_INFO("Import theme");
                    // TODO: Open file dialog to load theme
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset to Default")) {
                    LOG_INFO("Reset theme to default");
                    // TODO: Reset theme
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Save Now")) {
                    app.getPreferencesManager().saveToFile();
                    LOG_INFO("Preferences saved");
                }
                
                break;
            }
                
            case PreferenceSection::Modules: {
                ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Modules & Windows");
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Text("Enable or disable UI elements");
                ImGui::Spacing();
                
                // Checkboxes for each UI module
                static bool showTools = true;
                static bool showToolSettings = true;
                static bool showBrushSettings = true;
                static bool showLayers = true;
                static bool showColorSelector = true;
                static bool showDebugInfo = false;
                static bool showTopMenu = true;
                
                ImGui::Checkbox("Tools Panel", &showTools);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide the tools panel with brush, eraser, selection, etc.");
                }
                
                ImGui::Checkbox("Tool Settings Panel", &showToolSettings);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide tool-specific settings");
                }
                
                ImGui::Checkbox("Brush Settings Panel", &showBrushSettings);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide brush configuration options");
                }
                
                ImGui::Checkbox("Layers Panel", &showLayers);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide the layers management panel");
                }
                
                ImGui::Checkbox("Color Selector", &showColorSelector);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide the color picker");
                }
                
                ImGui::Checkbox("Debug Information", &showDebugInfo);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide performance and debug info");
                }
                
                ImGui::Checkbox("Top Menu Bar", &showTopMenu);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide the main menu bar (File, Edit, View, etc.)");
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.0f, 1.0f), "Note:");
                ImGui::TextWrapped("Some changes may require restarting the application to take full effect.");
                
                ImGui::Spacing();
                
                if (ImGui::Button("Reset to Defaults")) {
                    LOG_INFO("Reset module visibility to defaults");
                    showTools = true;
                    showToolSettings = true;
                    showBrushSettings = true;
                    showLayers = true;
                    showColorSelector = true;
                    showDebugInfo = false;
                    showTopMenu = true;
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Save Now")) {
                    // Save module visibility settings
                    auto& prefMgr = app.getPreferencesManager();
                    prefMgr.setModuleVisible("Tools", showTools);
                    prefMgr.setModuleVisible("ToolSettings", showToolSettings);
                    prefMgr.setModuleVisible("BrushSettings", showBrushSettings);
                    prefMgr.setModuleVisible("Layers", showLayers);
                    prefMgr.setModuleVisible("ColorSelector", showColorSelector);
                    prefMgr.setModuleVisible("DebugInfo", showDebugInfo);
                    prefMgr.setModuleVisible("TopMenu", showTopMenu);
                    prefMgr.saveToFile();
                    LOG_INFO("Preferences saved");
                }
                
                break;
            }
        }
        
        ImGui::EndChild();
        
    }
    ImGui::End();
}
