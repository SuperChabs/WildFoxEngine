module;

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

export module WFE.Rendering.Passes.UIPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Core.Camera;
import WFE.Core.CommandManager;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

export class UIPass : public RenderPass
{
public:
    UIPass(GLContext* ctx, ShaderManager* sm)
        : RenderPass("UIPass", ctx, sm)
    {}
    
    void Setup() override
    {
        context->SetDepthTest(true);
        context->SetDepthFunc(GL_LESS); 
        glDepthMask(GL_FALSE);
        context->SetBlend(true);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void Execute(const glm::mat4& view, const glm::mat4& projection) override
    {
        if (!enabled) return;
        
        Setup();
        
        CommandManager::ExecuteCommand("Renderer_RenderUI", 
        {
            view,
            projection,
            std::string("icon")
        });
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        glDepthMask(GL_TRUE);
        context->SetDepthTest(true);
        context->SetBlend(false);
    }
};