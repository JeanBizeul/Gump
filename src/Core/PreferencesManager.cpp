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

bool PreferencesManager::saveToFile(const std::string& filepath) const
{
    std::ofstream file(filepath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open preferences file for writing: {}", filepath);
        return false;
    }

    // Save current theme
    file << "[Theme]\n";
    file << "current=" << _currentTheme << "\n\n";

    // Save shortcuts
    file << "[Shortcuts]\n";
    for (const auto& [actionId, keyCombo] : _shortcuts) {
        file << actionId << "=" << keyCombo << "\n";
    }
    file << "\n";

    // Save current style colors
    file << "[StyleColors]\n";
    for (const auto& [name, color] : _styleColors) {
        file << name << "=" << color.r << "," << color.g << "," << color.b << "," << color.a << "\n";
    }
    file << "\n";

    // Save current style values
    file << "[StyleValues]\n";
    for (const auto& [name, value] : _styleValues) {
        file << name << "=" << value << "\n";
    }
    file << "\n";

    // Save all style presets
    for (const auto& [presetName, preset] : _stylePresets) {
        file << "[StylePreset:" << presetName << "]\n";
        
        // Save preset colors
        for (const auto& [name, color] : preset.colors) {
            file << "color_" << name << "=" << color.r << "," << color.g << "," << color.b << "," << color.a << "\n";
        }
        
        // Save preset values
        for (const auto& [name, value] : preset.values) {
            file << "value_" << name << "=" << value << "\n";
        }
        
        file << "\n";
    }

    // Save module visibility
    file << "[Modules]\n";
    for (const auto& [name, visible] : _moduleVisibility) {
        file << name << "=" << (visible ? "true" : "false") << "\n";
    }

    file.close();
    LOG_INFO("Saved preferences to: {}", filepath);
    return true;
}

bool PreferencesManager::loadFromFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LOG_WARNING("Preferences file not found: {}. Using defaults.", filepath);
        return false;
    }

    std::string line;
    std::string currentSection;
    StylePreset* currentPreset = nullptr;

    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Check for section header
        if (line[0] == '[' && line[line.length() - 1] == ']') {
            currentSection = line.substr(1, line.length() - 2);
            
            // Check if this is a style preset section
            if (currentSection.find("StylePreset:") == 0) {
                std::string presetName = currentSection.substr(12); // Skip "StylePreset:"
                _stylePresets[presetName] = StylePreset();
                _stylePresets[presetName].name = presetName;
                currentPreset = &_stylePresets[presetName];
            } else {
                currentPreset = nullptr;
            }
            continue;
        }

        // Parse key=value pairs
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);

        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Store based on current section
        if (currentSection == "Theme") {
            if (key == "current") {
                _currentTheme = value;
            }
        } else if (currentSection == "Shortcuts") {
            _shortcuts[key] = value;
        } else if (currentSection == "StyleColors") {
            // Parse color: r,g,b,a
            std::istringstream ss(value);
            float r, g, b, a;
            char comma;
            if (ss >> r >> comma >> g >> comma >> b >> comma >> a) {
                _styleColors[key] = glm::vec4(r, g, b, a);
            }
        } else if (currentSection == "StyleValues") {
            _styleValues[key] = std::stof(value);
        } else if (currentPreset != nullptr) {
            // Loading a style preset
            if (key.find("color_") == 0) {
                std::string colorName = key.substr(6); // Skip "color_"
                std::istringstream ss(value);
                float r, g, b, a;
                char comma;
                if (ss >> r >> comma >> g >> comma >> b >> comma >> a) {
                    currentPreset->colors[colorName] = glm::vec4(r, g, b, a);
                }
            } else if (key.find("value_") == 0) {
                std::string valueName = key.substr(6); // Skip "value_"
                currentPreset->values[valueName] = std::stof(value);
            }
        } else if (currentSection == "Modules") {
            _moduleVisibility[key] = (value == "true");
        }
    }

    file.close();
    LOG_INFO("Loaded preferences from: {}", filepath);
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
    _styleColors[key] = glm::vec4(r, g, b, a);
}

bool PreferencesManager::getStyleColor(const std::string& key, float& r, float& g, float& b, float& a) const {
    auto it = _styleColors.find(key);
    if (it == _styleColors.end()) {
        return false;
    }
    
    r = it->second.r;
    g = it->second.g;
    b = it->second.b;
    a = it->second.a;
    
    return true;
}

void PreferencesManager::setTheme(const std::string& themeName) {
    _currentTheme = themeName;
}

std::string PreferencesManager::getTheme() const {
    return _currentTheme;
}

void PreferencesManager::saveCurrentStyleAsPreset(const std::string& presetName) {
    StylePreset preset;
    preset.name = presetName;
    preset.colors = _styleColors;
    preset.values = _styleValues;
    
    _stylePresets[presetName] = preset;
    LOG_INFO("Saved style preset: {}", presetName);
}

void PreferencesManager::loadStylePreset(const std::string& presetName) {
    auto it = _stylePresets.find(presetName);
    if (it != _stylePresets.end()) {
        _styleColors = it->second.colors;
        _styleValues = it->second.values;
        _currentTheme = presetName;
        LOG_INFO("Loaded style preset: {}", presetName);
    } else {
        LOG_WARNING("Style preset not found: {}", presetName);
    }
}

std::vector<std::string> PreferencesManager::getStylePresetNames() const {
    std::vector<std::string> names;
    for (const auto& [name, preset] : _stylePresets) {
        names.push_back(name);
    }
    return names;
}

void PreferencesManager::deleteStylePreset(const std::string& presetName) {
    _stylePresets.erase(presetName);
    LOG_INFO("Deleted style preset: {}", presetName);
}

bool PreferencesManager::hasStylePreset(const std::string& presetName) const {
    return _stylePresets.find(presetName) != _stylePresets.end();
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
