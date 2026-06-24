#include "IndexBuffer.h"

IndexBuffer::IndexBuffer() {
    glGenBuffers(1, &EBO);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &EBO);
}

void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void IndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::SetData(const unsigned int *data, unsigned int cnt,
                          GLenum usage) {
    count = cnt;
    Bind();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        count * sizeof(unsigned int),
        data,
        usage
    );
}

unsigned int IndexBuffer::GetCount() const {
    return count;
}

unsigned int IndexBuffer::GetID() const {
    return EBO;
}