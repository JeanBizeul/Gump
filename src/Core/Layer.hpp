#pragma once

#include <memory>
#include <cstddef>
#include <string>

#include "Mesh.hpp"

namespace Gump {

class Layer {
public:
    Layer(size_t width, size_t height, const std::string &name);
    ~Layer() = default;

    void draw() const;  // only draws the mesh
    size_t getWidth() const { return _width; }
    size_t getHeight() const { return _height; }

    float transparency = 1.0f;
    bool isVisible = true;
    std::string name;

    OpenGLUtils::Mesh* getMesh() const { return _mesh.get(); }

private:
    size_t _width;
    size_t _height;
    std::unique_ptr<OpenGLUtils::Mesh> _mesh;
};

} // namespace Gump
