#pragma once

#include <memory>
#include <cstddef>
#include <string>

#include "Mesh.hpp"

namespace Gump {

class Layer {
public:
    Layer(const std::string &name, size_t width, size_t height,
        glm::vec2 uvMin, glm::vec2 uvMax, size_t texturePageIndex);
    ~Layer() = default;

    void draw() const;  // only draws the mesh
    size_t getWidth() const { return _width; }
    size_t getHeight() const { return _height; }
    glm::vec2 getUVMin() const { return _uvMin; }
    glm::vec2 getUVMax() const { return _uvMax; }

    float transparency = 1.0f;
    bool isVisible = true;
    std::string name;

    OpenGLUtils::Mesh* getMesh() const { return _mesh.get(); }

    const size_t texturePageIndex;

private:
    std::unique_ptr<OpenGLUtils::Mesh> _mesh;
    size_t _width;
    size_t _height;
    glm::vec2 _uvMin;
    glm::vec2 _uvMax;
};

} // namespace Gump
