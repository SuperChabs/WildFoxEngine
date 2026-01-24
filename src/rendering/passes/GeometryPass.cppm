module;

#include <string>
#include <glm/glm.hpp>

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
        context->SetDepthTest(true);
        context->SetBlend(false);
        context->SetCullFace(true);
    }
    
    void Execute(Camera& camera, const glm::mat4& projection) override
    {
        if (!enabled || !world) return;
        
        Setup();
        
        CommandManager::ExecuteCommand("Renderer_RenderGeometry", 
        {
            &camera,
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