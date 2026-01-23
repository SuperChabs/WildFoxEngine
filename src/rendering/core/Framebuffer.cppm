module;

#include <glad/glad.h>
#include <string>

export module WFE.Rendering.Core.Framebuffer;

import WFE.Core.Logger; 

export class Framebuffer
{
private:
    unsigned int FBO = 0;
    unsigned int textureID = 0;
    unsigned int RBO = 0;

    int width = 0;
    int height = 0;

    void Invalidate()
    {
        if (FBO)
        {
            glDeleteFramebuffers(1, &FBO);
            glDeleteTextures(1, &textureID);
            glDeleteRenderbuffers(1, &RBO);
            Logger::Log(LogLevel::INFO, "Deleted old framebuffer");
        }

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // Color texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            textureID,
            0
        );

        // Depth + stencil
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width,
            height
        );
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            RBO
        );

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            Logger::Log(
                LogLevel::ERROR,
                "Framebuffer incomplete! Status: " + std::to_string(status)
            );
        }
        else
        {
            Logger::Log(
                LogLevel::INFO,
                "Framebuffer created: " +
                std::to_string(width) + "x" +
                std::to_string(height) +
                " TextureID=" + std::to_string(textureID)
            );
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

public:
    Framebuffer(int w, int h)
        : width(w), height(h)
    {
        Invalidate();
    }

    ~Framebuffer()
    {
        if (FBO)
        {
            glDeleteFramebuffers(1, &FBO);
            glDeleteTextures(1, &textureID);
            glDeleteRenderbuffers(1, &RBO);
        }
    }

    void Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glViewport(0, 0, width, height);
    }

    void Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Resize(int newWidth, int newHeight)
    {
        if (newWidth <= 0 || newHeight <= 0)
            return;

        if (width == newWidth && height == newHeight)
            return;

        width = newWidth;
        height = newHeight;
        Invalidate();
    }

    unsigned int GetTextureID() const { return textureID; }
    int GetWidth()  const { return width; }
    int GetHeight() const { return height; }
};
