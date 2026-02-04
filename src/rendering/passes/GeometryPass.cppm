module;

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

export module WFE.Rendering.Passes.GeometryPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.ECS.ECSWorld;
import WFE.Core.Camera;
import WFE.Core.CommandManager;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

export class GeometryPass : public RenderPass
{
private:
    ECSWorld* world;

public:
    GeometryPass(GLContext* ctx, ShaderManager* sm, ECSWorld* w)
        : RenderPass("GeometryPass", ctx, sm)
        , world(w)
    {}
    
    void Setup() override
    {
        glDepthMask(GL_TRUE);
        
        context->SetDepthTest(true);
        context->SetDepthFunc(GL_LESS);
        
        context->SetBlend(false);
        context->SetCullFace(true);
        glCullFace(GL_BACK); 
    }
    
    void Execute(const glm::mat4& view, const glm::mat4& projection) override
    {
        if (!enabled || !world) return;
        
        Setup();
        
        CommandManager::ExecuteCommand("Renderer_RenderGeometry", 
        {
            view,
            projection,
            std::string("basic")
        });
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        // Cleanup if needed
    }
};