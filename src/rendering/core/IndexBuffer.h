#pragma once

#include <glad/glad.h>

class IndexBuffer {
private:
    unsigned int EBO = 0;
    unsigned int count = 0;

public:
    IndexBuffer();


    ~IndexBuffer();


    void Bind() const;


    void Unbind() const;


    void SetData(const unsigned int *data, unsigned int cnt,
                 GLenum usage = GL_STATIC_DRAW);


    unsigned int GetCount() const;

    unsigned int GetID() const;
};