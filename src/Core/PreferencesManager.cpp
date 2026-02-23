#include "PreferencesManager.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

namespace Gump {

PreferencesManager::PreferencesManager() {
    // Set default module visibility
    _moduleVisibility["Tools"] = true;
    _moduleVisibility["ToolSettings"] = true;
    _moduleVisibility["BrushSettings"] = true;
    _moduleVisibility["Layers"] = true;
    _moduleVisibility["ColorSelector"] = true;
    _moduleVisibility["DebugInfo"] = false;
    _moduleVisibility["TopMenu"] = true;
}

bool PreferencesManager::saveToFile(const std::string& filepath) {
    LOG_INFO("Saving preferences to: {}", filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open preferences file for writing: {}", filepath);
        return false;
    }
    
    file << "# Gump Preferences Configuration File\n";
    file << "# Auto-generated - Do not edit manually unless you know what you're doing\n\n";
    
    // Save theme
    if (!_theme.empty()) {
        file << "[Theme]\n";
        file << "current=" << _theme << "\n\n";
    }
    
    // Save shortcuts
    if (!_shortcuts.empty()) {
        file << "[Shortcuts]\n";
        for (const auto& [actionId, keyCombo] : _shortcuts) {
            file << actionId << "=" << keyCombo << "\n";
        }
        file << "\n";
    }
    
    // Save module visibility
    file << "[Modules]\n";
    for (const auto& [moduleName, visible] : _moduleVisibility) {
        file << moduleName << "=" << (visible ? "true" : "false") << "\n";
    }
    file << "\n";
    
    // Save style values
    if (!_styleValues.empty()) {
        file << "[StyleValues]\n";
        for (const auto& [key, value] : _styleValues) {
            file << key << "=" << value << "\n";
        }
        file << "\n";
    }
    
    // Save style colors
    if (!_styleColors.empty()) {
        file << "[StyleColors]\n";
        for (const auto& [key, color] : _styleColors) {
            file << key << "=" << color << "\n";
        }
        file << "\n";
    }
    
    file.close();
    LOG_INFO("Successfully saved preferences");
    return true;
}

bool PreferencesManager::loadFromFile(const std::string& filepath) {
    LOG_INFO("Loading preferences from: {}", filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LOG_WARNING("Preferences file not found: {}", filepath);
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check for section headers
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key=value pairs
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Store based on current section
        if (currentSection == "Theme") {
            if (key == "current") {
                _theme = value;
            }
        } else if (currentSection == "Shortcuts") {
            _shortcuts[key] = value;
        } else if (currentSection == "Modules") {
            _moduleVisibility[key] = (value == "true");
        } else if (currentSection == "StyleValues") {
            try {
                _styleValues[key] = std::stof(value);
            } catch (...) {
                LOG_WARNING("Invalid style value for {}: {}", key, value);
            }
        } else if (currentSection == "StyleColors") {
            _styleColors[key] = value;
        }
    }
    
    file.close();
    LOG_INFO("Successfully loaded preferences");
    return true;
}

void PreferencesManager::saveShortcuts(const ShortcutManager& manager) {
    _shortcuts.clear();
    for (const auto& shortcut : manager.getShortcuts()) {
        if (shortcut.key != -1) { // Only save bound shortcuts
            _shortcuts[shortcut.actionId] = serializeShortcut(shortcut.key, shortcut.modifiers);
        }
    }
}

void PreferencesManager::loadShortcuts(ShortcutManager& manager) {
    for (const auto& [actionId, keyCombo] : _shortcuts) {
        int key;
        KeyModifier mods;
        if (deserializeShortcut(keyCombo, key, mods)) {
            manager.updateShortcut(actionId, key, mods);
        }
    }
}

void PreferencesManager::setModuleVisible(const std::string& moduleName, bool visible) {
    _moduleVisibility[moduleName] = visible;
}

bool PreferencesManager::isModuleVisible(const std::string& moduleName, bool defaultValue) const {
    auto it = _moduleVisibility.find(moduleName);
    if (it != _moduleVisibility.end()) {
        return it->second;
    }
    return defaultValue;
}

void PreferencesManager::setStyleValue(const std::string& key, float value) {
    _styleValues[key] = value;
}

float PreferencesManager::getStyleValue(const std::string& key, float defaultValue) const {
    auto it = _styleValues.find(key);
    if (it != _styleValues.end()) {
        return it->second;
    }
    return defaultValue;
}

void PreferencesManager::setStyleColor(const std::string& key, float r, float g, float b, float a) {
    std::stringstream ss;
    ss << r << "," << g << "," << b << "," << a;
    _styleColors[key] = ss.str();
}

bool PreferencesManager::getStyleColor(const std::string& key, float& r, float& g, float& b, float& a) const {
    auto it = _styleColors.find(key);
    if (it == _styleColors.end()) {
        return false;
    }
    
    std::stringstream ss(it->second);
    std::string component;
    
    if (!std::getline(ss, component, ',')) return false;
    r = std::stof(component);
    
    if (!std::getline(ss, component, ',')) return false;
    g = std::stof(component);
    
    if (!std::getline(ss, component, ',')) return false;
    b = std::stof(component);
    
    if (!std::getline(ss, component, ',')) return false;
    a = std::stof(component);
    
    return true;
}

void PreferencesManager::setTheme(const std::string& themeName) {
    _theme = themeName;
}

std::string PreferencesManager::getTheme(const std::string& defaultTheme) const {
    return _theme.empty() ? defaultTheme : _theme;
}

std::string PreferencesManager::serializeShortcut(int key, KeyModifier mods) const {
    std::stringstream ss;
    
    // Save modifiers as flags
    int modFlags = static_cast<int>(mods);
    ss << modFlags << "," << key;
    
    return ss.str();
}

bool PreferencesManager::deserializeShortcut(const std::string& str, int& key, KeyModifier& mods) const {
    std::stringstream ss(str);
    std::string component;
    
    // Read modifier flags
    if (!std::getline(ss, component, ',')) {
        return false;
    }
    int modFlags = std::stoi(component);
    mods = static_cast<KeyModifier>(modFlags);
    
    // Read key
    if (!std::getline(ss, component, ',')) {
        return false;
    }
    key = std::stoi(component);
    
    return true;
}

} // namespace Gump
