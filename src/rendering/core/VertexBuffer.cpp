#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &VBO);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &VBO);
}

void VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void VertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void *data, size_t size, GLenum usage) {
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}

unsigned int VertexBuffer::GetID() const {
    return VBO;
}