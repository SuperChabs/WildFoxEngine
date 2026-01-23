module;

#include <cstddef>
#include <glad/glad.h>

export module WFE.Rendering.Core.VertexBuffer;

export class VertexBuffer
{
private:
    unsigned int VBO = 0;

public:
    VertexBuffer()
    {
        glGenBuffers(1, &VBO);
    }

    ~VertexBuffer()
    {
        glDeleteBuffers(1, &VBO);
    }

    void Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    void Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void SetData(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW)
    {
        Bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }

    unsigned int GetID() const { return VBO; }
};