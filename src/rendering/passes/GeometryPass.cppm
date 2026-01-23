export module WFE.Rendering.Passes.GeometryPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.ECS.ECSWorld;

export class GeometryPass : public RenderPass
{
private:
    ECSWorld* world;

public:
    GeometryPass(GLContext* ctx, ShaderManager* sm, ECSWorld* w)
        : RenderPass("GeometryPass", ctx, sm), world(w)
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
        
        commandBuffer.Sort();
        commandBuffer.Execute();
        commandBuffer.Clear();
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        // Cleanup if needed
    }
};