#include "ToolsFunctions.hpp"
#include "../Application.hpp"
#include "../Input.hpp"
#include <GLFW/glfw3.h>
#include "Logger.hpp"

void Gump::Tools::UpdateEraserTool(Application &app)
{
    auto& cam = app.getCamera();

    // Get viewport size from ImGui's main viewport
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float width = viewport->Size.x;
    float height = viewport->Size.y;
    glm::vec2 windowSize(width, height);

    // Check if mouse is over ImGui window
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    // Start stroke on mouse press
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);

        // Ensure eraser is enabled
        auto& brushSettings = app.getBrushSettings();
        brushSettings.isEraser = true;

        app.startStroke(worldPos);
        LOG_DEBUG("Eraser stroke started at ({}, {})", worldPos.x, worldPos.y);
    }

    // Continue stroke while dragging
    if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT) && app.hasActiveStroke()) {
        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);
        app.continueStroke(worldPos);
    }

    // Cancel stroke on right mouse button press
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && app.hasActiveStroke()) {
        app.cancelStroke();
        LOG_DEBUG("Eraser stroke cancelled by right click");
    }

    // Finish stroke on mouse release
    if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT) && app.hasActiveStroke()) {
        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);
        app.continueStroke(worldPos);
        app.finishStroke();
        LOG_DEBUG("Eraser stroke finished");
    }

    // Cancel stroke on Escape
    if (Input::isKeyPressed(GLFW_KEY_ESCAPE) && app.hasActiveStroke()) {
        app.cancelStroke();
        LOG_DEBUG("Eraser stroke cancelled");
    }
}
