#pragma once
#include <glm/glm.hpp>

namespace Gump {

class Camera2D {
 public:
    Camera2D();

    // Movement
    void move(const glm::vec2& delta);
    void setPosition(const glm::vec2& pos);

    // Zoom
    void zoom(float factor);
    void setZoom(float zoom);
    void resetZoom() { _zoom = 1.0f; }

    // Getters
    glm::vec2 getPosition() const;
    float getZoom() const;

    // Matrices
    glm::mat4 getPVMatrix(float width, float height) const;

    glm::vec2 screenToWorld(glm::vec2 screenPos, glm::vec2 viewportSize) const;
    glm::vec2 worldToScreen(glm::vec2 worldPos, glm::vec2 viewportSize) const;


 private:
    glm::vec2 _position;
    float _zoom;

    glm::mat4 getViewMatrix(float width, float height) const;
    glm::mat4 getProjectionMatrix(float width, float height) const;
};

} // namespace Gump
