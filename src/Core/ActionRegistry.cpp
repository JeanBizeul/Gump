#include "ActionRegistry.hpp"
#include "Logger.hpp"

namespace Gump {

void ActionRegistry::registerAction(const std::string& actionId, ActionFunction func) {
    _actions[actionId] = func;
    LOG_DEBUG("Registered action: {}", actionId);
}

bool ActionRegistry::executeAction(const std::string& actionId, Application& app) {
    auto it = _actions.find(actionId);
    if (it != _actions.end()) {
        it->second(app);
        return true;
    }
    LOG_WARNING("Action not found: {}", actionId);
    return false;
}

bool ActionRegistry::hasAction(const std::string& actionId) const {
    return _actions.find(actionId) != _actions.end();
}

} // namespace Gump
