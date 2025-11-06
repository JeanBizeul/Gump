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
    void setZoom(float zoom);
    void zoom(float factor);

    // Getters
    glm::vec2 getPosition() const;
    float getZoom() const;

    // Matrices
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getPVMatrix() const;

private:
    glm::vec2 _position;
    float _zoom;
};
}
