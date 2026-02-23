#pragma once

#include <string>
#include <map>
#include "ShortcutManager.hpp"

namespace Gump {

// Manages saving and loading of all application preferences
class PreferencesManager {
public:
    PreferencesManager();
    
    // Save all preferences to file
    bool saveToFile(const std::string& filepath = "preferences.cfg");
    
    // Load all preferences from file
    bool loadFromFile(const std::string& filepath = "preferences.cfg");
    
    // Shortcut management
    void saveShortcuts(const ShortcutManager& manager);
    void loadShortcuts(ShortcutManager& manager);
    
    // Module visibility settings
    void setModuleVisible(const std::string& moduleName, bool visible);
    bool isModuleVisible(const std::string& moduleName, bool defaultValue = true) const;
    
    // Style settings
    void setStyleValue(const std::string& key, float value);
    float getStyleValue(const std::string& key, float defaultValue) const;
    
    void setStyleColor(const std::string& key, float r, float g, float b, float a = 1.0f);
    bool getStyleColor(const std::string& key, float& r, float& g, float& b, float& a) const;
    
    // Theme selection
    void setTheme(const std::string& themeName);
    std::string getTheme(const std::string& defaultTheme = "Professional") const;

private:
    std::map<std::string, std::string> _shortcuts; // action_id -> key_combo
    std::map<std::string, bool> _moduleVisibility;
    std::map<std::string, float> _styleValues;
    std::map<std::string, std::string> _styleColors; // stored as "r,g,b,a"
    std::string _theme;
    
    // Helper functions
    std::string serializeShortcut(int key, KeyModifier mods) const;
    bool deserializeShortcut(const std::string& str, int& key, KeyModifier& mods) const;
};

} // namespace Gump
