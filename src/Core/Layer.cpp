#include "Layer.hpp"

#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Mesh.hpp"

using namespace Gump;

const std::vector<unsigned int> Indices = {
    0, 1, 2,
    2, 3, 0
};

Layer::Layer(size_t width, size_t height)
    : _width(width), _height(height)
{
    const std::vector<OpenGLUtils::Vertex_t> vertices = {
        // Position                  // UVs
        { { 0.0f,    0.0f,    0.0f }, { 0.0f, 0.0f } },
        { { _width,   0.0f,    0.0f }, { 1.0f, 0.0f } },
        { { _width,   _height,  0.0f }, { 1.0f, 1.0f } },
        { { 0.0f,    _height,  0.0f }, { 0.0f, 1.0f } }
    };

    _mesh = std::make_unique<OpenGLUtils::Mesh>(vertices, Indices);
    _shader = std::make_unique<OpenGLUtils::Shader>(
        "shaders/canva.vert",
        "shaders/canva.frag"
    );
    _camera = std::make_unique<Camera2D>();
    _camera->setPosition(glm::vec2(_width / 2.0f, _height / 2.0f));
}

void Layer::draw() const {
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 projectionView = _camera->getPVMatrix();

    _shader->use();
    _shader->set("uModel", model);
    _shader->set("uProjectionView", projectionView);

    _mesh->bind();
    _mesh->draw();
}

void Layer::manageInputs() {
    // Input management logic for the camera would go here
}
