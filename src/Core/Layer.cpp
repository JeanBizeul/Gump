#include "Layer.hpp"
#include <vector>

using namespace Gump;

const std::vector<unsigned int> Indices = {
    0, 1, 2,
    2, 3, 0
};

Layer::Layer(size_t width, size_t height, const std::string &name)
    : _width(width), _height(height), name(name)
{
    const std::vector<OpenGLUtils::Vertex_t> vertices = {
        // Position                  // UVs
        { { 0.0f,    0.0f,    0.0f }, { 0.0f, 0.0f } },
        { { _width,   0.0f,    0.0f }, { 1.0f, 0.0f } },
        { { _width,   _height,  0.0f }, { 1.0f, 1.0f } },
        { { 0.0f,    _height,  0.0f }, { 0.0f, 1.0f } }
    };

    _mesh = std::make_unique<OpenGLUtils::Mesh>(vertices, Indices);
}

// Draw only binds and draws the mesh
void Layer::draw() const {
    _mesh->bind();
    _mesh->draw();
}
