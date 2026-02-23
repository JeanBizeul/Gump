#pragma once

#include <string>
#include <map>
#include <vector>
#include "ShortcutManager.hpp"
#include <glm/vec4.hpp>

namespace Gump {

// Manages saving and loading of all application preferences
class PreferencesManager {
public:
    PreferencesManager();
    ~PreferencesManager() = default;

    // File operations
    bool loadFromFile(const std::string& filepath = "preferences.cfg");
    bool saveToFile(const std::string& filepath = "preferences.cfg") const;

    // Shortcut management
    void saveShortcuts(const ShortcutManager& shortcutManager);
    void loadShortcuts(ShortcutManager& shortcutManager);

    // Style management
    void setTheme(const std::string& themeName);
    std::string getTheme() const;
    
    // Style presets
    void saveCurrentStyleAsPreset(const std::string& presetName);
    void loadStylePreset(const std::string& presetName);
    std::vector<std::string> getStylePresetNames() const;
    void deleteStylePreset(const std::string& presetName);
    bool hasStylePreset(const std::string& presetName) const;
    
    void setStyleColor(const std::string& name, float r, float g, float b, float a);
    bool getStyleColor(const std::string& name, float& r, float& g, float& b, float& a) const;
    
    void setStyleValue(const std::string& name, float value);
    float getStyleValue(const std::string& name, float defaultValue = 0.0f) const;

    // Module visibility
    void setModuleVisible(const std::string& moduleName, bool visible);
    bool isModuleVisible(const std::string& moduleName, bool defaultValue = true) const;

private:
    struct StylePreset {
        std::string name;
        std::map<std::string, glm::vec4> colors;  // name -> rgba
        std::map<std::string, float> values;      // name -> value
    };
    
    std::string serializeShortcut(int key, KeyModifier mods) const;
    bool deserializeShortcut(const std::string& str, int& key, KeyModifier& mods) const;
    
    std::map<std::string, std::string> _shortcuts;
    std::string _currentTheme = "Professional (Light)";
    
    // Current style settings (for the active preset)
    std::map<std::string, glm::vec4> _styleColors;
    std::map<std::string, float> _styleValues;
    
    // All saved presets
    std::map<std::string, StylePreset> _stylePresets;
    
    std::map<std::string, bool> _moduleVisibility;
};

} // namespace Gump
