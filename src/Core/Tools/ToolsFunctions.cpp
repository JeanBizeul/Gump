#include "ToolsFunctions.hpp"

#include <unordered_map>
#include <string>
#include <functional>

std::unordered_map<std::string, std::function<void(Gump::Application &)>> Gump::Tools::ActionFunction = {
    {"move", Gump::Tools::UpdateMoveTool},
    {"selection", Gump::Tools::UpdateSelectionTool}
};
