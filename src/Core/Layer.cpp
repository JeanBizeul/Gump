#include "Layer.hpp"
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <glm/vec2.hpp>

#include "Logger.hpp"

using namespace Gump;

const std::vector<unsigned int> Indices = {
    0, 1, 2,
    2, 3, 0
};

Layer::Layer(const std::string &name, size_t width, size_t height,
    glm::vec2 uvMin, glm::vec2 uvMax, size_t textureID)
    : name(name), _width(width), _height(height), texturePageIndex(textureID),
      _uvMin(uvMin), _uvMax(uvMax)
{
    const std::vector<OpenGLUtils::Vertex_t> vertices = {
           // Position                            // UVs
        { { 0.0f,         0.0f,          0.0f }, { uvMin.x, uvMax.y } }, // top-left
        { { (float)width, 0.0f,          0.0f }, { uvMax.x, uvMax.y } }, // top-right
        { { (float)width, (float)height, 0.0f }, { uvMax.x, uvMin.y } }, // bottom-right
        { { 0.0f,         (float)height, 0.0f }, { uvMin.x, uvMin.y } }  // bottom-left
    };

    _mesh = std::make_unique<OpenGLUtils::Mesh>(vertices, Indices);
}

void Layer::draw() const {
    if (!isVisible) return;

    _mesh->bind();
    _mesh->draw();
}

void Layer::updateMesh() {
    const std::vector<OpenGLUtils::Vertex_t> vertices = {
           // Position                                    // UVs
        { { position.x,         position.y,          0.0f }, { _uvMin.x, _uvMax.y } }, // top-left
        { { position.x + (float)_width, position.y,          0.0f }, { _uvMax.x, _uvMax.y } }, // top-right
        { { position.x + (float)_width, position.y + (float)_height, 0.0f }, { _uvMax.x, _uvMin.y } }, // bottom-right
        { { position.x,         position.y + (float)_height, 0.0f }, { _uvMin.x, _uvMin.y } }  // bottom-left
    };

    _mesh = std::make_unique<OpenGLUtils::Mesh>(vertices, Indices);
}
