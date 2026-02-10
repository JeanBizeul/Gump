#include "Camera2D.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "Logger.hpp"

#include "glad/glad.h"

using namespace Gump;

Camera2D::Camera2D()
    : _position(0.0f, 0.0f), _zoom(1.0f)
{
}

// Movement
void Camera2D::move(const glm::vec2& delta) {
    _position += glm::vec2(delta.x, delta.y) / _zoom;
}
void Camera2D::setPosition(const glm::vec2& pos) { _position = pos; }

// Zoom
void Camera2D::setZoom(float zoom) { _zoom = zoom; }
void Camera2D::zoom(float factor) { _zoom *= factor; }

// Getters
glm::vec2 Camera2D::getPosition() const { return _position; }
float Camera2D::getZoom() const { return _zoom; }

// Matrices
glm::mat4 Camera2D::getViewMatrix(float width, float height) const {

    glm::mat4 view(1.0f);

    // Move camera so that its position becomes the center of the screen
    view = glm::translate(view, glm::vec3(width * 0.5f, height * 0.5f, 0.0f));

    // Apply zoom around screen center
    view = glm::scale(view, glm::vec3(_zoom));

    // Move world so chosen pixel (camera position) is in the center
    view = glm::translate(view, glm::vec3(-_position.x, -_position.y, 0.0f));

    return view;
}


glm::mat4 Camera2D::getProjectionMatrix(float width, float height) const {
    // Flip Y-axis: use (0, height) to (width, 0) instead of (0, 0) to (width, height)
    // This makes Y increase upward, matching OpenGL texture coordinates
    return glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

glm::mat4 Camera2D::getPVMatrix(float width, float height) const {
    return getProjectionMatrix(width, height) * getViewMatrix(width, height);
}

glm::vec2 Gump::Camera2D::screenToWorld(glm::vec2 screenPos, glm::vec2 viewportSize) const
{
    float width  = viewportSize.x;
    float height = viewportSize.y;
    screenPos.y = height - screenPos.y; // Invert Y for OpenGL coordinates

    glm::mat4 pv = getPVMatrix(width, height);

    glm::mat4 invPV = glm::inverse(pv);

    // Screen → NDC
    glm::vec4 ndc;
    ndc.x = (screenPos.x / width) * 2.0f - 1.0f;
    ndc.y = (screenPos.y / height) * 2.0f - 1.0f;
    ndc.z = 0.0f;
    ndc.w = 1.0f;

    glm::vec4 world = invPV * ndc;

    return glm::vec2(world);
}

glm::vec2 Gump::Camera2D::worldToScreen(glm::vec2 worldPos, glm::vec2 viewportSize) const
{
    float width  = viewportSize.x;
    float height = viewportSize.y;

    glm::mat4 pv = getPVMatrix(width, height);

    glm::vec4 clip = pv * glm::vec4(worldPos, 0.0f, 1.0f);

    // Perspective divide (technically unnecessary for ortho, but correct)
    glm::vec3 ndc = glm::vec3(clip) / clip.w;

    // NDC [-1,1] → screen [0,width/height]
    glm::vec2 screen;
    screen.x = (ndc.x * 0.5f + 0.5f) * width;
    screen.y = (ndc.y * 0.5f + 0.5f) * height;

    return screen;
}
