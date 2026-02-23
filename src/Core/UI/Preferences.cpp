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
                
                // Theme/Preset selection
                ImGui::Text("Style Preset:");
                ImGui::SameLine();
                
                static std::string currentPresetName = "";
                static int currentPresetIndex = 0;
                
                // Get all preset names from config file only
                std::vector<std::string> allPresets = app.getPreferencesManager().getStylePresetNames();
                
                // Initialize current preset name from preferences
                if (currentPresetName.empty()) {
                    currentPresetName = app.getPreferencesManager().getTheme();
                    auto it = std::find(allPresets.begin(), allPresets.end(), currentPresetName);
                    if (it != allPresets.end()) {
                        currentPresetIndex = std::distance(allPresets.begin(), it);
                    }
                }
                
                // Convert to const char* array for ImGui
                std::vector<const char*> presetCStrings;
                for (const auto& name : allPresets) {
                    presetCStrings.push_back(name.c_str());
                }
                
                if (ImGui::Combo("##StylePreset", &currentPresetIndex, presetCStrings.data(), presetCStrings.size())) {
                    currentPresetName = allPresets[currentPresetIndex];
                    LOG_INFO("Style preset changed to: {}", currentPresetName);
                    
                    // Load the preset
                    app.getPreferencesManager().loadStylePreset(currentPresetName);
                    app.applyStyleSettingsFromPreferences();
                    
                    // Force reload of UI colors
                    static bool colorsInitialized = false;
                    colorsInitialized = false;
                    static bool valuesInitialized = false;
                    valuesInitialized = false;
                }
                
                ImGui::SameLine();
                
                // Save current settings as new preset
                static char newPresetName[128] = "";
                ImGui::SetNextItemWidth(150);
                ImGui::InputTextWithHint("##NewPresetName", "New preset name...", newPresetName, sizeof(newPresetName));
                ImGui::SameLine();
                if (ImGui::Button("Save As")) {
                    if (strlen(newPresetName) > 0) {
                        app.getPreferencesManager().saveCurrentStyleAsPreset(newPresetName);
                        LOG_INFO("Saved current style as preset: {}", newPresetName);
                        currentPresetName = newPresetName;
                        newPresetName[0] = '\0'; // Clear input
                    }
                }
                
                ImGui::SameLine();
                
                // Delete current preset
                if (ImGui::Button("Delete")) {
                    app.getPreferencesManager().deleteStylePreset(currentPresetName);
                    LOG_INFO("Deleted style preset: {}", currentPresetName);
                    // Reload presets and select first one
                    allPresets = app.getPreferencesManager().getStylePresetNames();
                    if (!allPresets.empty()) {
                        currentPresetIndex = 0;
                        currentPresetName = allPresets[0];
                        app.getPreferencesManager().loadStylePreset(currentPresetName);
                        app.applyStyleSettingsFromPreferences();
                    }
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Color customization
                ImGui::Text("Colors:");
                ImGui::Spacing();
                
                // Initialize static colors from saved preferences on first use
                static bool colorsInitialized = false;
                static ImVec4 bgColor = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
                static ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                static ImVec4 accentColor = ImVec4(0.0f, 0.47f, 0.84f, 1.0f);
                static ImVec4 buttonColor = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
                
                if (!colorsInitialized) {
                    float r, g, b, a;
                    if (app.getPreferencesManager().getStyleColor("Background", r, g, b, a)) {
                        bgColor = ImVec4(r, g, b, a);
                    }
                    if (app.getPreferencesManager().getStyleColor("Text", r, g, b, a)) {
                        textColor = ImVec4(r, g, b, a);
                    }
                    if (app.getPreferencesManager().getStyleColor("Accent", r, g, b, a)) {
                        accentColor = ImVec4(r, g, b, a);
                    }
                    if (app.getPreferencesManager().getStyleColor("Button", r, g, b, a)) {
                        buttonColor = ImVec4(r, g, b, a);
                    }
                    colorsInitialized = true;
                }
                
                if (ImGui::ColorEdit3("Background Color", (float*)&bgColor)) {
                    app.getPreferencesManager().setStyleColor("Background", bgColor.x, bgColor.y, bgColor.z, bgColor.w);
                    app.applyStyleSettingsFromPreferences();
                }
                if (ImGui::ColorEdit3("Text Color", (float*)&textColor)) {
                    app.getPreferencesManager().setStyleColor("Text", textColor.x, textColor.y, textColor.z, textColor.w);
                    app.applyStyleSettingsFromPreferences();
                }
                if (ImGui::ColorEdit3("Accent Color", (float*)&accentColor)) {
                    app.getPreferencesManager().setStyleColor("Accent", accentColor.x, accentColor.y, accentColor.z, accentColor.w);
                    app.applyStyleSettingsFromPreferences();
                }
                if (ImGui::ColorEdit3("Button Color", (float*)&buttonColor)) {
                    app.getPreferencesManager().setStyleColor("Button", buttonColor.x, buttonColor.y, buttonColor.z, buttonColor.w);
                    app.applyStyleSettingsFromPreferences();
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Style parameters
                ImGui::Text("Style Parameters:");
                ImGui::Spacing();
                
                // Initialize static values from saved preferences on first use
                static bool valuesInitialized = false;
                static float rounding = 0.0f;
                static float spacing = 8.0f;
                static float padding = 8.0f;
                
                if (!valuesInitialized) {
                    rounding = app.getPreferencesManager().getStyleValue("WindowRounding", 0.0f);
                    spacing = app.getPreferencesManager().getStyleValue("ItemSpacing", 8.0f);
                    padding = app.getPreferencesManager().getStyleValue("WindowPadding", 8.0f);
                    valuesInitialized = true;
                }
                
                if (ImGui::SliderFloat("Window Rounding", &rounding, 0.0f, 12.0f)) {
                    app.getPreferencesManager().setStyleValue("WindowRounding", rounding);
                    app.applyStyleSettingsFromPreferences();
                }
                if (ImGui::SliderFloat("Item Spacing", &spacing, 2.0f, 20.0f)) {
                    app.getPreferencesManager().setStyleValue("ItemSpacing", spacing);
                    app.applyStyleSettingsFromPreferences();
                }
                if (ImGui::SliderFloat("Window Padding", &padding, 2.0f, 20.0f)) {
                    app.getPreferencesManager().setStyleValue("WindowPadding", padding);
                    app.applyStyleSettingsFromPreferences();
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                // Reset button
                if (ImGui::Button("Reset Current Preset")) {
                    LOG_INFO("Reset current preset to saved values");
                    // Reload the current preset from file
                    app.getPreferencesManager().loadStylePreset(currentPresetName);
                    app.applyStyleSettingsFromPreferences();
                    
                    // Force UI to refresh
                    colorsInitialized = false;
                    valuesInitialized = false;
                }
                
                break;
            }
                
            case PreferenceSection::Modules: {
                ImGui::TextColored(ImVec4(0.0f, 0.47f, 0.84f, 1.0f), "Modules & Windows");
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Text("Enable or disable UI elements");
                ImGui::Spacing();
                
                // Load visibility settings from preferences manager directly (no static cache)
                auto& prefMgr = app.getPreferencesManager();
                
                bool showTools = prefMgr.isModuleVisible("Tools", true);
                bool showToolSettings = prefMgr.isModuleVisible("ToolSettings", true);
                bool showBrushSettings = prefMgr.isModuleVisible("BrushSettings", true);
                bool showLayers = prefMgr.isModuleVisible("Layers", true);
                bool showDebugInfo = prefMgr.isModuleVisible("DebugInfo", false);
                
                // Checkboxes for each UI module - save immediately on change
                if (ImGui::Checkbox("Tools Panel", &showTools)) {
                    prefMgr.setModuleVisible("Tools", showTools);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide the tools panel with brush, eraser, selection, etc.");
                }
                
                if (ImGui::Checkbox("Tool Settings Panel", &showToolSettings)) {
                    prefMgr.setModuleVisible("ToolSettings", showToolSettings);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide tool-specific settings");
                }
                
                if (ImGui::Checkbox("Brush Settings Panel", &showBrushSettings)) {
                    prefMgr.setModuleVisible("BrushSettings", showBrushSettings);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide brush configuration options");
                }
                
                if (ImGui::Checkbox("Layers Panel", &showLayers)) {
                    prefMgr.setModuleVisible("Layers", showLayers);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide the layers management panel");
                }
                
                if (ImGui::Checkbox("Debug Information", &showDebugInfo)) {
                    prefMgr.setModuleVisible("DebugInfo", showDebugInfo);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Show/hide performance and debug info");
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                if (ImGui::Button("Reset to Defaults")) {
                    LOG_INFO("Reset module visibility to defaults");
                    prefMgr.setModuleVisible("Tools", true);
                    prefMgr.setModuleVisible("ToolSettings", true);
                    prefMgr.setModuleVisible("BrushSettings", true);
                    prefMgr.setModuleVisible("Layers", true);
                    prefMgr.setModuleVisible("DebugInfo", false);
                }
                
                break;
            }
        }
        
        ImGui::EndChild();
        
    }
    ImGui::End();
}
