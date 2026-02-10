#include "ToolsFunctions.hpp"
#include "Application.hpp"
#include "Input.hpp"
#include <GLFW/glfw3.h>

void Gump::Tools::UpdateMoveTool(Application &app)
{
    auto &selection = app.getSelectionState();
    auto &cam = app.getCamera();

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

    static bool isDragging = false;
    static glm::vec2 dragStartWorld;
    static glm::vec2 dragStartOffset;
    static bool movingLayer = false; // Track if we're moving a layer or selection

    // If there's a selection, move it
    if (selection.hasSelection) {
        glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);
        glm::vec2 selMin = selection.getMin() + selection.offset;
        glm::vec2 selMax = selection.getMax() + selection.offset;

        bool isInsideSelection = (worldPos.x >= selMin.x && worldPos.x <= selMax.x &&
                                   worldPos.y >= selMin.y && worldPos.y <= selMax.y);

        // Start dragging if clicked inside selection
        if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && isInsideSelection) {
            isDragging = true;
            movingLayer = false;
            dragStartWorld = worldPos;
            dragStartOffset = selection.offset;
        }

        // Update offset while dragging
        if (isDragging && !movingLayer && Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec2 currentWorld = cam.screenToWorld(Input::getMousePosition(), windowSize);
            glm::vec2 delta = currentWorld - dragStartWorld;
            selection.offset = dragStartOffset + delta;
        }

        // Stop dragging on release
        if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
            if (isDragging && !movingLayer) {
                glm::vec2 currentWorld = cam.screenToWorld(Input::getMousePosition(), windowSize);
                glm::vec2 delta = currentWorld - dragStartWorld;
                selection.offset = dragStartOffset + delta;
                isDragging = false;
            }
        }

        // Update selection mesh
        app.updateSelectionMesh();
    }
    // If there's no selection, move the top layer
    else if (!app.getLayers().empty()) {
        auto& topLayer = app.getLayers().back();

        // Start dragging layer
        if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            isDragging = true;
            movingLayer = true;
            glm::vec2 worldPos = cam.screenToWorld(Input::getMousePosition(), windowSize);
            dragStartWorld = worldPos;
            dragStartOffset = topLayer->position;
        }

        // Update layer position while dragging
        if (isDragging && movingLayer && Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec2 currentWorld = cam.screenToWorld(Input::getMousePosition(), windowSize);
            glm::vec2 delta = currentWorld - dragStartWorld;
            topLayer->position = dragStartOffset + delta;
            topLayer->updateMesh();
        }

        // Stop dragging on release
        if (Input::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
            if (isDragging && movingLayer) {
                glm::vec2 currentWorld = cam.screenToWorld(Input::getMousePosition(), windowSize);
                glm::vec2 delta = currentWorld - dragStartWorld;
                topLayer->position = dragStartOffset + delta;
                topLayer->updateMesh();
                isDragging = false;
                movingLayer = false;
            }
        }
    }
}
