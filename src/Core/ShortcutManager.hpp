#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace Gump {

// Keyboard modifier flags
enum class KeyModifier {
    None = 0,
    Ctrl = 1 << 0,
    Shift = 1 << 1,
    Alt = 1 << 2,
    Super = 1 << 3  // Windows key / Command key
};

inline KeyModifier operator|(KeyModifier a, KeyModifier b) {
    return static_cast<KeyModifier>(static_cast<int>(a) | static_cast<int>(b));
}

inline KeyModifier operator&(KeyModifier a, KeyModifier b) {
    return static_cast<KeyModifier>(static_cast<int>(a) & static_cast<int>(b));
}

inline bool hasModifier(KeyModifier flags, KeyModifier mod) {
    return (flags & mod) == mod;
}

// Represents a keyboard shortcut
struct Shortcut {
    std::string actionName;      // Human-readable action name
    std::string actionId;        // Unique identifier for the action
    int key;                     // GLFW key code
    KeyModifier modifiers;       // Modifier keys (Ctrl, Shift, Alt, etc.)
    
    Shortcut(const std::string& name, const std::string& id, int k, KeyModifier mods = KeyModifier::None)
        : actionName(name), actionId(id), key(k), modifiers(mods) {}
    
    // Get human-readable string (e.g., "Ctrl+Shift+S")
    std::string toString() const;
    
    // Check if this shortcut matches the current input state
    bool matches(int pressedKey, KeyModifier currentMods) const;
};

class ShortcutManager {
public:
    ShortcutManager();
    
    // Get all shortcuts
    const std::vector<Shortcut>& getShortcuts() const { return _shortcuts; }
    
    // Update a shortcut
    void updateShortcut(const std::string& actionId, int newKey, KeyModifier newMods);
    
    // Reset to defaults
    void resetToDefaults();
    
    // Find shortcut by action ID
    Shortcut* findShortcut(const std::string& actionId);
    
    // Get default shortcut for an action ID
    const Shortcut* getDefaultShortcut(const std::string& actionId) const;
    
    // Reset a specific shortcut to its default
    void resetShortcutToDefault(const std::string& actionId);
    
    // Check if a key combination is already used (returns action name if used, empty if free)
    std::string isKeyCombinationUsed(int key, KeyModifier mods, const std::string& excludeActionId = "") const;
    
    // Save/Load shortcuts (TODO: implement file I/O)
    void saveToFile(const std::string& filepath);
    void loadFromFile(const std::string& filepath);

private:
    std::vector<Shortcut> _shortcuts;
    std::vector<Shortcut> _defaultShortcuts;
    
    void initializeDefaults();
};

} // namespace Gump
