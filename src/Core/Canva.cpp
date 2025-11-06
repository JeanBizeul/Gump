#include "Canva.hpp"

#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Mesh.hpp"

using namespace Gump;

const std::vector<OpenGLUtils::Vertex_t> Vertices = {
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
    { {  1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
    { {  1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
    { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
};

const std::vector<unsigned int> Indices = {
    0, 1, 2,
    2, 3, 0
};

Canva::Canva() {
    _mesh = std::make_unique<OpenGLUtils::Mesh>(Vertices, Indices);
    _shader = std::make_unique<OpenGLUtils::Shader>(
        "shaders/canva.vert",
        "shaders/canva.frag"
    );
    _camera = std::make_unique<Camera2D>();
}

void Canva::draw() const {
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 view = _camera->getViewMatrix();

    glm::mat4 projection = _camera->getProjectionMatrix();

    _shader->use();
    _shader->set("uModel", model);
    _shader->set("uView", view);
    _shader->set("uProjection", projection);

    _mesh->bind();
    _mesh->draw();
}

void Canva::manageInputs() {
    // Input management logic for the camera would go here
}
