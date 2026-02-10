#pragma once

#include <unordered_map>
#include <string>
#include <functional>

namespace Gump
{

class Application;

namespace Tools
{

extern std::unordered_map<std::string, std::function<void(Application &)>> ActionFunction;

void UpdateMoveTool(Application &app);
void UpdateSelectionTool(Application &app);
void UpdateFuzzySelectionTool(Application &app);
void UpdatePencilTool(Application &app);
void UpdateEraserTool(Application &app);

} // namespace Tools

} // namespace Gump
