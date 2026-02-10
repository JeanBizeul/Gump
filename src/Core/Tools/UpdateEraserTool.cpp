#include "Stroke.hpp"
#include "../Application.hpp"
#include "../Input.hpp"
#include <GLFW/glfw3.h>
#include "ToolsFunctions.hpp"

void Gump::Tools::UpdateEraserTool(Application &app)
{
    auto &cam = app.getCamera();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float width  = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);

    glm::vec2 mousePos = Input::getMousePosition();
    glm::vec2 worldPos = cam.screenToWorld(mousePos, glm::vec2(width, height));

    // Get pressure from tablet if available (otherwise 1.0)
    float pressure = 1.0f; // TODO: Add tablet support

    // Ensure eraser mode is enabled
    auto& brushSettings = app.getBrushSettings();
    brushSettings.eraser = true;

    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        // Start new stroke
        app.startStroke(worldPos, pressure);
    }
    else if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
        // Continue stroke
        app.continueStroke(worldPos, pressure);
    }
    else if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        // End stroke and commit to layer
        app.endStroke();
    }
}
