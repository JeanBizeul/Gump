#include "VertexArray.hpp"

#include "glad/glad.h"

#include "Vertex.hpp"

/**
 * Remember that in Vertex.hpp :
 *  typedef struct Vertex_s {
 *      glm::vec3 position;
 *      glm::vec3 normal;
 *      glm::vec2 uv;
 *  } Vertex_t;
 */


OpenGLUtils::VertexArray::VertexArray(VertexBuffer &vbo, IndexBuffer &ebo)
{
    glGenVertexArrays(1, &_id);
    this->bind();

    vbo.bind();
    ebo.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void *)(offsetof(Vertex_t, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_t), (void *)(offsetof(Vertex_t, uv)));

    this->unbind();
}

OpenGLUtils::VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &_id);
}

OpenGLUtils::VertexArray::VertexArray(VertexArray &&other) noexcept
{
    _id = other._id;
    other._id = 0;
}

OpenGLUtils::VertexArray &OpenGLUtils::VertexArray::operator=(VertexArray &&other) noexcept
{
    if (this != &other) {
        glDeleteVertexArrays(1, &_id);
        _id = other._id;
        other._id = 0;
    }
    return *this;
}

void OpenGLUtils::VertexArray::bind() const
{
    glBindVertexArray(_id);
}

void OpenGLUtils::VertexArray::unbind()
{
    glBindVertexArray(0);
}

unsigned int OpenGLUtils::VertexArray::getId() const
{
    return _id;
}
