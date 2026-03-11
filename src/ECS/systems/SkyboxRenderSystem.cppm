module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

export module WFE.ECS.Systems.SkyboxRenderSystem;

import WFE.Core.Camera;
import WFE.Core.Logger;
import WFE.Resource.Shader.ShaderManager;
import WFE.Rendering.Core.GLContext;
import WFE.Rendering.RenderQueue;
import WFE.Rendering.Commands.DrawSkyboxCommand;

export class SkyboxRenderSystem
{
private:
    GLuint skyboxVAO = 0;
    GLuint cubemapTexture = 0;
    GLContext* context = nullptr;
    
    void SetupSkybox()
    {
        float skyboxVertices[] = 
        {
            -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f
        };
        
        GLuint vbo;
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &vbo);
        
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        
        glBindVertexArray(0);
    }

public:
    SkyboxRenderSystem(GLuint cubemap)
        : cubemapTexture(cubemap)
    {
        SetupSkybox();
        Logger::Log(LogLevel::INFO, "SkyboxRenderSystem initialized");
    }
    
    ~SkyboxRenderSystem()
    {
        if (skyboxVAO) glDeleteVertexArrays(1, &skyboxVAO);
    }
    
    void SetContext(GLContext* ctx) { context = ctx; }
    
    void SubmitToQueue(RenderQueue& queue)
    {
        if (!context)
        {
            Logger::Log(LogLevel::ERROR, "SkyboxRenderSystem: Context not set");
            return;
        }
        
        auto cmd = std::make_unique<DrawSkyboxCommand>(
            skyboxVAO,
            cubemapTexture,
            context
        );
        
        queue.Submit(std::move(cmd));
    }
    
    GLuint GetVAO() const { return skyboxVAO; }
    GLuint GetCubemap() const { return cubemapTexture; }
};