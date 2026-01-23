export module WFE.Rendering.Passes.SkyboxPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Rendering.Commands.DrawSkyboxCommand;

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
        context->SetDepthFunc(GL_LEQUAL);
        shaderManager->Bind("skybox");
    }
    
    void Execute(Camera& camera, const glm::mat4& projection) override
    {
        if (!enabled) return;
        
        Setup();
        
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        shaderManager->SetMat4("skybox", "view", view);
        shaderManager->SetMat4("skybox", "projection", projection);
        
        auto cmd = std::make_unique<DrawSkyboxCommand>(
            skyboxVAO, cubemapTexture, context
        );
        commandBuffer.Submit(std::move(cmd));
        
        commandBuffer.Execute();
        commandBuffer.Clear();
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        context->SetDepthFunc(GL_LESS);
    }
};
