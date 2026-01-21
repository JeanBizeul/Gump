#include "ToolsFunctions.hpp"
#include "../Application.hpp"
#include "../Input.hpp"
#include <GLFW/glfw3.h>

void Gump::Tools::UpdateMoveTool(Application &app)
{
    auto &selection = app.getSelectionState();
    auto &cam = app.getCamera();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float width  = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);
    glm::vec2 windowSize(width, height);

    // Only work if there's an active selection
    if (!selection.hasSelection) {
        return;
    }

    // Check if mouse is over ImGui window
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    static bool isDragging = false;
    static glm::vec2 dragStartWorld;
    static glm::vec2 dragStartOffset;

    // Check if mouse is inside selection bounds
    glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);
    glm::vec2 selMin = selection.getMin() + selection.offset;
    glm::vec2 selMax = selection.getMax() + selection.offset;

    bool isInsideSelection = (worldPos.x >= selMin.x && worldPos.x <= selMax.x &&
                               worldPos.y >= selMin.y && worldPos.y <= selMax.y);

    // Start dragging if clicked inside selection
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && isInsideSelection) {
        isDragging = true;
        dragStartWorld = worldPos;
        dragStartOffset = selection.offset;
    }

    // Update offset while dragging
    if (isDragging && Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 currentWorld = cam.screenToWorld(Input::getMousePosition(), windowSize);
        glm::vec2 delta = currentWorld - dragStartWorld;
        selection.offset = dragStartOffset + delta;
    }

    // Stop dragging on release
    if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
        if (isDragging) {
            glm::vec2 currentWorld = cam.screenToWorld(Input::getMousePosition(), windowSize);
            glm::vec2 delta = currentWorld - dragStartWorld;
            selection.offset = dragStartOffset + delta;
            isDragging = false;
        }
    }

    // Update selection mesh
    app.updateSelectionMesh();
}
