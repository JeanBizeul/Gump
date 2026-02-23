#pragma once

#include <cstddef>

namespace OpenGLUtils
{
/**
 * This class reprsents a ElementBufferObject, it aims to store all indices
 * of an object in an array.
 */
class IndexBuffer {
 public:
    IndexBuffer(const unsigned int *data, size_t count);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer &other) = delete;
    IndexBuffer &operator=(const IndexBuffer &other) = delete;
    IndexBuffer(IndexBuffer &&other) noexcept;
    IndexBuffer &operator=(IndexBuffer &&other) noexcept;

    void bind() const;
    static void unbind();

    size_t getCount() const;
    unsigned int getId() const;

 private:
    size_t _count;
    unsigned int _id = 0;
};
}  // OpenGLUtils
