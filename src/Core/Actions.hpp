#pragma once

namespace Gump {

class Application;

namespace Actions {

// File Actions
void newFile(Application& app);
void openFile(Application& app);
void loadFile(Application& app);
void saveFile(Application& app);
void saveFileAs(Application& app);
void exportFile(Application& app);

// Edit Actions
void undo(Application& app);
void redo(Application& app);
void cut(Application& app);
void copy(Application& app);
void paste(Application& app);

// Selection Actions
void selectAll(Application& app);
void deselectAll(Application& app);

// Tool Actions
void selectPencilTool(Application& app);
void selectEraserTool(Application& app);
void selectSelectionTool(Application& app);
void selectMoveTool(Application& app);
void selectFuzzySelectTool(Application& app);

// View Actions
void zoomIn(Application& app);
void zoomOut(Application& app);
void resetZoom(Application& app);

// App Actions
void openPreferences(Application& app);

// Register all actions
void registerAllActions();

} // namespace Actions
} // namespace Gump
