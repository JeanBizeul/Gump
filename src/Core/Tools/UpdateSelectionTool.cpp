#include "ToolsFunctions.hpp"
#include "../Application.hpp"
#include "../Input.hpp"
#include <GLFW/glfw3.h>

void Gump::Tools::UpdateSelectionTool(Application &app)
{
    auto& selection = app.getSelectionState();

    // Check if mouse is over ImGui window
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    // Start new selection on left mouse button press
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = app.screenToWorld(Input::getMousePosition());

        selection.hasSelection = true;
        selection.startPos = worldPos;
        selection.endPos = worldPos;
        selection.offset = glm::vec2(0.0f, 0.0f);
    }

    // Update selection end position while dragging
    if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = app.screenToWorld(Input::getMousePosition());
        selection.endPos = worldPos;
    }

    // Finalize selection on release
    if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 worldPos = app.screenToWorld(Input::getMousePosition());
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
