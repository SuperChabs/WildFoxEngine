#pragma once

#include <cstddef>

#include <glad/glad.h>

class VertexBuffer {
private:
    unsigned int VBO = 0;

public:
    VertexBuffer();


    ~VertexBuffer();


    void Bind() const;


    void Unbind() const;


    void SetData(const void *data, size_t size, GLenum usage = GL_STATIC_DRAW);


    unsigned int GetID() const;
};