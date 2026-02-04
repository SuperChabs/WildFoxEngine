module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module WFE.Rendering.Passes.SkyboxPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Core.Camera;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

export class SkyboxPass : public RenderPass
{
private:
    GLuint skyboxVAO = 0;
    GLuint cubemapTexture = 0;

public:
    SkyboxPass(GLContext* ctx, ShaderManager* sm, GLuint vao, GLuint cubemap)
        : RenderPass("SkyboxPass", ctx, sm)
        , skyboxVAO(vao)
        , cubemapTexture(cubemap)
    {}
    
    void Setup() override
    {
        glDepthMask(GL_FALSE);
        context->SetDepthFunc(GL_LEQUAL);
        
        shaderManager->Bind("skybox");
    }
    
    void Execute(const glm::mat4& view, const glm::mat4& projection) override
    {
        if (!enabled) return;
        
        Setup();
        
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
        shaderManager->SetMat4("skybox", "view", skyboxView);
        shaderManager->SetMat4("skybox", "projection", projection);
        
        context->BindVAO(skyboxVAO);
        context->BindTexture2D(cubemapTexture, 0);
        context->DrawArrays(GL_TRIANGLES, 0, 36);
        context->UnbindVAO();
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        glDepthMask(GL_TRUE); 
        context->SetDepthFunc(GL_LESS);
        shaderManager->Unbind();
    }
};