module;

#include <string>
#include <glm/glm.hpp>

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
        context->SetDepthTest(false);
        context->SetBlend(true);
    }
    
    void Execute(Camera& camera, const glm::mat4& projection) override
    {
        if (!enabled) return;
        
        Setup();
        
        CommandManager::ExecuteCommand("Renderer_RenderUI", 
        {
            &camera,
            projection,
            std::string("icon")
        });
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        context->SetDepthTest(true);
        context->SetBlend(false);
    }
};