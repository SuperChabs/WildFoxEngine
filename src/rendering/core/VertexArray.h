#pragma once

#include <cstddef>

#include <glad/glad.h>

class VertexArray {
private:
    unsigned int VAO = 0;

public:
    VertexArray();

    ~VertexArray();


    void Bind() const;

    void Unbind() const;


    void AddAttribute(
        unsigned int index,
        int size,
        GLenum type,
        bool normalized,
        int stride,
        size_t offset
    );


    unsigned int GetID() const;
};