#include "ToolsFunctions.hpp"
#include "../Application.hpp"
#include "../Input.hpp"
#include <GLFW/glfw3.h>
#include "Logger.hpp"

void Gump::Tools::UpdateFuzzySelectionTool(Application &app)
{
    auto& selection = app.getSelectionState();
    auto &cam = app.getCamera();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float width  = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);
    glm::vec2 windowSize(width, height);

    // Check if mouse is over ImGui window
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    // Start new selection on left mouse button press
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);

        selection.hasSelection = true;
        selection.startPos = worldPos;
        selection.endPos = worldPos;
        selection.offset = glm::vec2(0.0f, 0.0f);
    }

    // Update selection end position while dragging
    if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);
        selection.endPos = worldPos;
    }

    // Finalize selection on release
    if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);
        selection.endPos = worldPos;

        // If selection is too small, cancel it
        glm::vec2 size = selection.getSize();
        if (size.x < 1.0f || size.y < 1.0f) {
            selection.hasSelection = false;
        }
    }

    // Update selection mesh
    app.updateSelectionMesh();
}
