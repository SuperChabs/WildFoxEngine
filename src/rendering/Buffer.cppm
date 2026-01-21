module;

#include <cstddef>
#include <glad/glad.h>

export module WildFoxEngine.Rendering.Buffer;

// ================= VertexBuffer =================
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

// ================= IndexBuffer =================
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

// ================= VertexArray =================
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
