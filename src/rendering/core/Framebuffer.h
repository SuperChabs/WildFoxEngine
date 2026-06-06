#pragma once

#include <glad/glad.h>

class Framebuffer {
    unsigned int FBO = 0;
    unsigned int textureID = 0;
    unsigned int RBO = 0;

    int width = 0;
    int height = 0;

public:
    Framebuffer(int w, int h);

    ~Framebuffer();

    void Bind();

    void Unbind();

    void Resize(int newWidth, int newHeight);

    unsigned int GetTextureID() const { return textureID; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    void Invalidate();
};