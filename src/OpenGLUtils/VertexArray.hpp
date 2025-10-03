#pragma once

#include <cstddef>

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

namespace OpenGLUtils
{
/**
 * This class reprsents a VertexArrayObject, it aims to store the VBO & EBO
 * of an object.
 */
class VertexArray {
 public:
    VertexArray(VertexBuffer &vbo, IndexBuffer &ebo);
    ~VertexArray();

    VertexArray(const VertexArray &other) = delete;
    VertexArray &operator=(const VertexArray &other) = delete;
    VertexArray(VertexArray &&other) noexcept;
    VertexArray &operator=(VertexArray &&other) noexcept;

    void bind() const;
    static void unbind();

    unsigned int getId() const;

 private:
    unsigned int _id = 0;
};
}  // OpenGLUtils
