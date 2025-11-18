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
void Camera2D::move(const glm::vec2& delta) { _position += delta; }
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
    // Center the camera on the window
    return glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

glm::mat4 Camera2D::getPVMatrix() const {
    // Query current GL viewport (requires a current GL context)
    GLint vp[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, vp);
    float width  = static_cast<float>(vp[2]);
    float height = static_cast<float>(vp[3]);

    return getProjectionMatrix(width, height) * getViewMatrix(width, height);
}
