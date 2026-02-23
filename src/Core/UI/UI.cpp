#include "UI/UI.hpp"

// ...existing code...

void Gump::renderUI(Gump::Application &app) {
    // ...existing code...

    UI::renderTopMenu(app);
    UI::renderTools(app);
    UI::renderToolSettings(app);
    UI::renderBrushSettings(app);
    UI::renderLayers(app);
    UI::renderResizeCanvasDialog(app);
    UI::renderManualResizeDialog(app);
    UI::renderDebugInfo(app);
    UI::renderPreferences(app);
}