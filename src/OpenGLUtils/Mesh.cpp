#include "Mesh.hpp"

#include "glad/glad.h"

OpenGLUtils::Mesh::Mesh(const std::vector<Vertex_t> &vertices,
    const std::vector<unsigned int> &indices)
    : _vbo(vertices.data(), vertices.size()),
      _ebo(indices.data(), indices.size()),
      _vao(_vbo, _ebo),
      _indexCount(indices.size())
{}

void OpenGLUtils::Mesh::bind() const
{
    _vao.bind();
}

void OpenGLUtils::Mesh::draw() const
{
    bind();
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
}
