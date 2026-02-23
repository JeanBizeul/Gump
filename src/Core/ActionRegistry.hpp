#pragma once

#include <string>
#include <functional>
#include <map>

namespace Gump {

class Application;

// Central registry of all application actions
// Actions are independent of UI - they can be triggered by shortcuts, menu items, buttons, etc.
class ActionRegistry {
public:
    using ActionFunction = std::function<void(Application&)>;
    
    static ActionRegistry& instance() {
        static ActionRegistry inst;
        return inst;
    }
    
    // Register an action with its ID
    void registerAction(const std::string& actionId, ActionFunction func);
    
    // Execute an action by ID
    bool executeAction(const std::string& actionId, Application& app);
    
    // Check if an action exists
    bool hasAction(const std::string& actionId) const;
    
private:
    ActionRegistry() = default;
    std::map<std::string, ActionFunction> _actions;
};

// Helper to register actions at startup
#define REGISTER_ACTION(id, func) \
    static bool _action_registered_##id = (ActionRegistry::instance().registerAction(#id, func), true)

} // namespace Gump
