#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>

class Framebuffer 
{
private:
    unsigned int FBO;
    unsigned int textureID;
    unsigned int RBO;
    
    int width;
    int height;

public:
    Framebuffer(int width, int height);
    ~Framebuffer();
    
    void Bind();
    void Unbind();
    void Resize(int newWidth, int newHeight);
    
    unsigned int GetTextureID() const { return textureID; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    
    void Invalidate();
};

#endif