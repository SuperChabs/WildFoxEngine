#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <glad/glad.h>

class VertexBuffer 
{
private:
    unsigned int VBO;
    
public:
    VertexBuffer();
    ~VertexBuffer();
    
    void Bind() const;
    void Unbind() const;
    void SetData(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);
    
    unsigned int GetID() const { return VBO; }
};

class IndexBuffer 
{
private:
    unsigned int EBO;
    unsigned int count;
    
public:
    IndexBuffer();
    ~IndexBuffer();
    
    void Bind() const;
    void Unbind() const;
    void SetData(const unsigned int* data, unsigned int count, GLenum usage = GL_STATIC_DRAW);
    
    unsigned int GetCount() const { return count; }
    unsigned int GetID() const { return EBO; }
};

class VertexArray 
{
private:
    unsigned int VAO;
    
public:
    VertexArray();
    ~VertexArray();
    
    void Bind() const;
    void Unbind() const;
    
    void AddAttribute(unsigned int index, int size, GLenum type, 
                     bool normalized, int stride, size_t offset);
    
    unsigned int GetID() const { return VAO; }
};

#endif