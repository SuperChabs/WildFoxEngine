#include "VertexArray.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &VAO);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &VAO);
}

void VertexArray::Bind() const {
    glBindVertexArray(VAO);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::AddAttribute(
    unsigned int index,
    int size,
    GLenum type,
    bool normalized,
    int stride,
    size_t offset
) {
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        index,
        size,
        type,
        normalized ? GL_TRUE : GL_FALSE,
        stride,
        reinterpret_cast<void *>(offset)
    );
}

unsigned int VertexArray::GetID() const {
    return VAO;
}