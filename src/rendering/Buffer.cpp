#include "rendering/Buffer.h"

// ============ VertexBuffer ============
VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &VBO);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &VBO);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void* data, size_t size, GLenum usage)
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

// ============ IndexBuffer ============
IndexBuffer::IndexBuffer()
    : count(0)
{
    glGenBuffers(1, &EBO);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &EBO);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::SetData(const unsigned int* data, unsigned int cnt, GLenum usage)
{
    count = cnt;
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, usage);
}

// ============ VertexArray ============
VertexArray::VertexArray()
{
    glGenVertexArrays(1, &VAO);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &VAO);
}

void VertexArray::Bind() const
{
    glBindVertexArray(VAO);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddAttribute(unsigned int index, int size, GLenum type, 
                               bool normalized, int stride, size_t offset)
{
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, 
                         stride, (void*)offset);
}