#pragma once

#include <vector>

#include "VertexArray.hpp"
#include "Vertex.hpp"

namespace OpenGLUtils
{
class Mesh
{
 public:
    Mesh(const std::vector<Vertex_t> &vertices,
        const std::vector<unsigned int> &indices);
    ~Mesh() = default;

    void bind() const;
    void draw() const;
 private:
    VertexBuffer _vbo;
    IndexBuffer _ebo;
    VertexArray _vao;
    size_t _indexCount;
};
}  // namespace OpenGLUtils
