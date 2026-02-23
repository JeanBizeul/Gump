#include "Actions.hpp"
#include "ActionRegistry.hpp"
#include "Application.hpp"
#include "Logger.hpp"
#include "Actions/TopMenuActions.hpp"

namespace Gump {
namespace Actions {

// File Actions
void newFile(Application& app) {
    LOG_INFO("Action: New File");
    TopMenu::newFile(app);
}

void openFile(Application& app) {
    LOG_INFO("Action: Open File");
    TopMenu::openFile(app);
}

void saveFile(Application& app) {
    LOG_INFO("Action: Save File");
    TopMenu::saveFile(app);
}

void saveFileAs(Application& app) {
    LOG_INFO("Action: Save File As");
    // TODO: Implement save as dialog (different from regular save)
}

void exportFile(Application& app) {
    LOG_INFO("Action: Export File");
    TopMenu::exportFile(app);
}

// Edit Actions
void undo(Application& app) {
    LOG_INFO("Action: Undo");
    // TODO: Implement undo system
}

void redo(Application& app) {
    LOG_INFO("Action: Redo");
    // TODO: Implement redo system
}

void cut(Application& app) {
    if (!app.getSelectionState().hasSelection) {
        LOG_WARNING("No selection to cut");
        return;
    }
    LOG_INFO("Action: Cut");
    app.cutSelection();
}

void copy(Application& app) {
    if (!app.getSelectionState().hasSelection) {
        LOG_WARNING("No selection to copy");
        return;
    }
    LOG_INFO("Action: Copy");
    app.copySelection();
}

void paste(Application& app) {
    if (!app.getClipboard().hasData) {
        LOG_WARNING("Clipboard is empty");
        return;
    }
    LOG_INFO("Action: Paste");
    app.pasteClipboard();
}

// Selection Actions
void selectAll(Application& app) {
    LOG_INFO("Action: Select All");
    if (app.getLayerCount() == 0) {
        LOG_WARNING("No layers to select");
        return;
    }
    
    // Select the entire top layer
    auto& topLayer = app.getLayer(app.getLayerCount() - 1);
    auto& selState = app.getSelectionState();
    selState.hasSelection = true;
    selState.startPos = glm::vec2(0, 0);
    selState.endPos = glm::vec2(topLayer.getWidth(), topLayer.getHeight());
    selState.offset = glm::vec2(0, 0);
    selState.clearMask();
    app.updateSelectionMesh();
}

void deselectAll(Application& app) {
    LOG_INFO("Action: Deselect");
    auto& selState = app.getSelectionState();
    selState.hasSelection = false;
    selState.clearMask();
    app.updateSelectionMesh();
}

// Tool Actions
void selectPencilTool(Application& app) {
    LOG_INFO("Action: Select Pencil Tool");
    app.setSelectedTool("pencil");
}

void selectEraserTool(Application& app) {
    LOG_INFO("Action: Select Eraser Tool");
    app.setSelectedTool("eraser");
}

void selectSelectionTool(Application& app) {
    LOG_INFO("Action: Select Selection Tool");
    app.setSelectedTool("selection");
}

void selectMoveTool(Application& app) {
    LOG_INFO("Action: Select Move Tool");
    app.setSelectedTool("move");
}

void selectFuzzySelectTool(Application& app) {
    LOG_INFO("Action: Select Fuzzy Select Tool");
    app.setSelectedTool("fuzzy_select");
}

// View Actions
void zoomIn(Application& app) {
    LOG_INFO("Action: Zoom In");
    app.getCamera().zoom(1.1f);
}

void zoomOut(Application& app) {
    LOG_INFO("Action: Zoom Out");
    app.getCamera().zoom(0.9f);
}

void resetZoom(Application& app) {
    LOG_INFO("Action: Reset Zoom");
    app.getCamera().resetZoom();
}

// App Actions
void openPreferences(Application& app) {
    LOG_INFO("Action: Open Preferences");
    app.openPreferences();
}

// Register all actions
void registerAllActions() {
    auto& registry = ActionRegistry::instance();
    
    // File
    registry.registerAction("file.new", newFile);
    registry.registerAction("file.open", openFile);
    registry.registerAction("file.save", saveFile);
    registry.registerAction("file.save_as", saveFileAs);
    registry.registerAction("file.export", exportFile);
    
    // Edit
    registry.registerAction("edit.undo", undo);
    registry.registerAction("edit.redo", redo);
    registry.registerAction("edit.cut", cut);
    registry.registerAction("edit.copy", copy);
    registry.registerAction("edit.paste", paste);
    
    // Selection
    registry.registerAction("select.all", selectAll);
    registry.registerAction("select.none", deselectAll);
    
    // Tools
    registry.registerAction("tool.pencil", selectPencilTool);
    registry.registerAction("tool.eraser", selectEraserTool);
    registry.registerAction("tool.selection", selectSelectionTool);
    registry.registerAction("tool.move", selectMoveTool);
    registry.registerAction("tool.fuzzy_select", selectFuzzySelectTool);
    
    // View
    registry.registerAction("view.zoom_in", zoomIn);
    registry.registerAction("view.zoom_out", zoomOut);
    registry.registerAction("view.zoom_reset", resetZoom);
    
    // App
    registry.registerAction("app.preferences", openPreferences);
    
    LOG_INFO("Registered all application actions");
}

} // namespace Actions
} // namespace Gump
