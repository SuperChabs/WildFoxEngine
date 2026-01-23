module;

#include <cstddef>
#include <glad/glad.h>

export module WFE.Rendering.Core.VertexArray;

export class VertexArray
{
private:
    unsigned int VAO = 0;

public:
    VertexArray()
    {
        glGenVertexArrays(1, &VAO);
    }

    ~VertexArray()
    {
        glDeleteVertexArrays(1, &VAO);
    }

    void Bind() const
    {
        glBindVertexArray(VAO);
    }

    void Unbind() const
    {
        glBindVertexArray(0);
    }

    void AddAttribute(
        unsigned int index,
        int size,
        GLenum type,
        bool normalized,
        int stride,
        size_t offset
    )
    {
        Bind();
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
            index,
            size,
            type,
            normalized ? GL_TRUE : GL_FALSE,
            stride,
            reinterpret_cast<void*>(offset)
        );
    }

    unsigned int GetID() const { return VAO; }
};