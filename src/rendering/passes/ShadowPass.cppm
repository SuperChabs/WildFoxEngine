module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module WFE.Rendering.Passes.ShadowPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.Core.CommandManager;

export class ShadowPass : public RenderPass
{
private:
    GLuint shadowFBO = 0;
    GLuint shadowMap = 0;
    int shadowMapSize = 2048;

public:
    ShadowPass(GLContext* ctx, ShaderManager* sm)
        : RenderPass("ShadowPass", ctx, sm)
    {
        InitializeShadowMap();
    }
    
    ~ShadowPass()
    {
        if (shadowFBO) glDeleteFramebuffers(1, &shadowFBO);
        if (shadowMap) glDeleteTextures(1, &shadowMap);
    }
    
    void Setup() override
    {
        context->BindFBO(shadowFBO);
        context->SetViewport(0, 0, shadowMapSize, shadowMapSize);
        context->Clear(GL_DEPTH_BUFFER_BIT);
    }
    
    void Execute(const glm::mat4& view, const glm::mat4& projection) override
    {
        if (!enabled) return;
        
        Setup();
        

        
        Cleanup();
    }
    
    void Cleanup() override
    {
        context->BindFBO(0);
    }
    
    GLuint GetShadowMap() const { return shadowMap; }

private:
    void InitializeShadowMap()
    {
        glGenFramebuffers(1, &shadowFBO);
        
        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                     shadowMapSize, shadowMapSize, 0, 
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                              GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};