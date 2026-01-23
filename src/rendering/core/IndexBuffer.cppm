module;

#include <glad/glad.h>

export module WFE.Rendering.Core.IndexBuffer;

export class IndexBuffer
{
private:
    unsigned int EBO = 0;
    unsigned int count = 0;

public:
    IndexBuffer()
    {
        glGenBuffers(1, &EBO);
    }

    ~IndexBuffer()
    {
        glDeleteBuffers(1, &EBO);
    }

    void Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }

    void Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void SetData(const unsigned int* data, unsigned int cnt,
                 GLenum usage = GL_STATIC_DRAW)
    {
        count = cnt;
        Bind();
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            count * sizeof(unsigned int),
            data,
            usage
        );
    }

    unsigned int GetCount() const { return count; }
    unsigned int GetID() const { return EBO; }
};