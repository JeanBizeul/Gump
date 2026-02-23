#pragma once

#include <cstddef>

#include "Vertex.hpp"

namespace OpenGLUtils
{
/**
 * This class reprsents a VertexBufferObject, it aims to store all vertices
 * of an object in an array.
 */
class VertexBuffer {
 public:
    VertexBuffer(const Vertex_t *data, size_t count);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer &other) = delete;
    VertexBuffer &operator=(const VertexBuffer &other) = delete;
    VertexBuffer(VertexBuffer &&other) noexcept;
    VertexBuffer &operator=(VertexBuffer &&other) noexcept;

    void bind() const;
    static void unbind();

    unsigned int getId() const;

 private:
    unsigned int _id = 0;
};
}  // OpenGLUtils
