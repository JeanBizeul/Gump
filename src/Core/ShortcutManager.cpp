#include "ShortcutManager.hpp"
#include <GLFW/glfw3.h>
#include <sstream>
#include "Logger.hpp"

namespace Gump {

std::string Shortcut::toString() const {
    // Handle unbound shortcuts
    if (key == -1) {
        return "None";
    }
    
    std::stringstream ss;
    
    if (hasModifier(modifiers, KeyModifier::Ctrl)) {
        ss << "Ctrl+";
    }
    if (hasModifier(modifiers, KeyModifier::Shift)) {
        ss << "Shift+";
    }
    if (hasModifier(modifiers, KeyModifier::Alt)) {
        ss << "Alt+";
    }
    if (hasModifier(modifiers, KeyModifier::Super)) {
        ss << "Super+";
    }
    
    // Convert GLFW key code to string
    const char* keyName = glfwGetKeyName(key, 0);
    if (keyName) {
        std::string keyStr = keyName;
        // Capitalize first letter
        if (!keyStr.empty()) {
            keyStr[0] = toupper(keyStr[0]);
        }
        ss << keyStr;
    } else {
        // Handle special keys
        switch (key) {
            case GLFW_KEY_SPACE: ss << "Space"; break;
            case GLFW_KEY_ENTER: ss << "Enter"; break;
            case GLFW_KEY_TAB: ss << "Tab"; break;
            case GLFW_KEY_BACKSPACE: ss << "Backspace"; break;
            case GLFW_KEY_DELETE: ss << "Delete"; break;
            case GLFW_KEY_ESCAPE: ss << "Escape"; break;
            case GLFW_KEY_F1: ss << "F1"; break;
            case GLFW_KEY_F2: ss << "F2"; break;
            case GLFW_KEY_F3: ss << "F3"; break;
            case GLFW_KEY_F4: ss << "F4"; break;
            case GLFW_KEY_F5: ss << "F5"; break;
            case GLFW_KEY_F6: ss << "F6"; break;
            case GLFW_KEY_F7: ss << "F7"; break;
            case GLFW_KEY_F8: ss << "F8"; break;
            case GLFW_KEY_F9: ss << "F9"; break;
            case GLFW_KEY_F10: ss << "F10"; break;
            case GLFW_KEY_F11: ss << "F11"; break;
            case GLFW_KEY_F12: ss << "F12"; break;
            case GLFW_KEY_UP: ss << "Up"; break;
            case GLFW_KEY_DOWN: ss << "Down"; break;
            case GLFW_KEY_LEFT: ss << "Left"; break;
            case GLFW_KEY_RIGHT: ss << "Right"; break;
            default: ss << "Key" << key; break;
        }
    }
    
    return ss.str();
}

bool Shortcut::matches(int pressedKey, KeyModifier currentMods) const {
    // Unbound shortcuts never match
    if (key == -1) {
        return false;
    }
    return key == pressedKey && modifiers == currentMods;
}

ShortcutManager::ShortcutManager() {
    initializeDefaults();
    _defaultShortcuts = _shortcuts; // Store defaults for reset
}

void ShortcutManager::initializeDefaults() {
    _shortcuts.clear();
    
    // File operations
    _shortcuts.emplace_back("New File", "file.new", GLFW_KEY_N, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Open File", "file.open", GLFW_KEY_O, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Save File", "file.save", GLFW_KEY_S, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Save As", "file.save_as", GLFW_KEY_S, KeyModifier::Ctrl | KeyModifier::Shift);
    _shortcuts.emplace_back("Export", "file.export", GLFW_KEY_E, KeyModifier::Ctrl);
    
    // Edit operations
    _shortcuts.emplace_back("Undo", "edit.undo", GLFW_KEY_Z, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Redo", "edit.redo", GLFW_KEY_Y, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Cut", "edit.cut", GLFW_KEY_X, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Copy", "edit.copy", GLFW_KEY_C, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Paste", "edit.paste", GLFW_KEY_V, KeyModifier::Ctrl);
    
    // Selection operations
    _shortcuts.emplace_back("Select All", "select.all", GLFW_KEY_A, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Deselect", "select.none", GLFW_KEY_D, KeyModifier::Ctrl);
    
    // Tools
    _shortcuts.emplace_back("Pencil Tool", "tool.pencil", GLFW_KEY_B, KeyModifier::None);
    _shortcuts.emplace_back("Eraser Tool", "tool.eraser", GLFW_KEY_E, KeyModifier::None);
    _shortcuts.emplace_back("Selection Tool", "tool.selection", GLFW_KEY_M, KeyModifier::None);
    _shortcuts.emplace_back("Move Tool", "tool.move", GLFW_KEY_V, KeyModifier::None);
    _shortcuts.emplace_back("Fuzzy Select Tool", "tool.fuzzy_select", GLFW_KEY_W, KeyModifier::None);
    
    // View operations
    _shortcuts.emplace_back("Zoom In", "view.zoom_in", GLFW_KEY_EQUAL, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Zoom Out", "view.zoom_out", GLFW_KEY_MINUS, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Reset Zoom", "view.zoom_reset", GLFW_KEY_0, KeyModifier::Ctrl);
    
    // Preferences
    _shortcuts.emplace_back("Preferences", "app.preferences", GLFW_KEY_COMMA, KeyModifier::Ctrl);
    _shortcuts.emplace_back("Tutorials", "app.tutorials", GLFW_KEY_F1, KeyModifier::None);
}

void ShortcutManager::updateShortcut(const std::string& actionId, int newKey, KeyModifier newMods) {
    for (auto& shortcut : _shortcuts) {
        if (shortcut.actionId == actionId) {
            shortcut.key = newKey;
            shortcut.modifiers = newMods;
            LOG_INFO("Updated shortcut '{}' to {}", actionId, shortcut.toString());
            return;
        }
    }
    LOG_WARNING("Shortcut with ID '{}' not found", actionId);
}

void ShortcutManager::resetToDefaults() {
    _shortcuts = _defaultShortcuts;
    LOG_INFO("Reset all shortcuts to defaults");
}

Shortcut* ShortcutManager::findShortcut(const std::string& actionId) {
    for (auto& shortcut : _shortcuts) {
        if (shortcut.actionId == actionId) {
            return &shortcut;
        }
    }
    return nullptr;
}

const Shortcut* ShortcutManager::getDefaultShortcut(const std::string& actionId) const {
    for (const auto& shortcut : _defaultShortcuts) {
        if (shortcut.actionId == actionId) {
            return &shortcut;
        }
    }
    return nullptr;
}

void ShortcutManager::resetShortcutToDefault(const std::string& actionId) {
    const Shortcut* defaultShortcut = getDefaultShortcut(actionId);
    if (defaultShortcut) {
        updateShortcut(actionId, defaultShortcut->key, defaultShortcut->modifiers);
        LOG_INFO("Reset shortcut '{}' to default: {}", actionId, defaultShortcut->toString());
    } else {
        LOG_WARNING("No default shortcut found for '{}'", actionId);
    }
}

std::string ShortcutManager::isKeyCombinationUsed(int key, KeyModifier mods, const std::string& excludeActionId) const {
    for (const auto& shortcut : _shortcuts) {
        if (shortcut.actionId != excludeActionId && shortcut.matches(key, mods)) {
            return shortcut.actionName;
        }
    }
    return "";
}

void ShortcutManager::saveToFile(const std::string& filepath) {
    // TODO: Implement JSON/text file saving
    LOG_INFO("Save shortcuts to file: {}", filepath);
}

void ShortcutManager::loadFromFile(const std::string& filepath) {
    // TODO: Implement JSON/text file loading
    LOG_INFO("Load shortcuts from file: {}", filepath);
}

} // namespace Gump
