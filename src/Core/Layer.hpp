#pragma once

#include <memory>
#include <cstddef>

#include "Mesh.hpp"

namespace Gump {

class Layer {
public:
    Layer(size_t width, size_t height);
    ~Layer() = default;

    void draw() const;  // only draws the mesh
    size_t getWidth() const { return _width; }
    size_t getHeight() const { return _height; }

    OpenGLUtils::Mesh* getMesh() const { return _mesh.get(); }

private:
    size_t _width;
    size_t _height;
    std::unique_ptr<OpenGLUtils::Mesh> _mesh;
};

} // namespace Gump
