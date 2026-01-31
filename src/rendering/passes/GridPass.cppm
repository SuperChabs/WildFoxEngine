module;

#include <glad/glad.h>
#include <glm/glm.hpp>

export module WFE.Rendering.Passes.GridPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Core.Camera;
import WFE.Core.CommandManager;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

export class GridPass : public RenderPass
{
public:
    GridPass(GLContext* ctx, ShaderManager* sm)
        : RenderPass("GridPass", ctx, sm)
    {}
    
    void Setup() override
    {
        context->SetDepthTest(true);
        context->SetDepthFunc(GL_LESS);
        
        context->SetBlend(true);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void Execute(Camera& camera, const glm::mat4& projection) override
    {
        if (!enabled) return;
        
        Setup();
        
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        int width = viewport[2];
        int height = viewport[3];
        
        CommandManager::ExecuteCommand("Renderer_RenderGrid", 
        {
            &camera,
            width,
            height
        });
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        context->SetBlend(false);
    }
};