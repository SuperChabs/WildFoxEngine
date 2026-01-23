export module WFE.Rendering.Passes.UIPass;

import WFE.Rendering.Passes.RenderPass;

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

        commandBuffer.Execute();
        commandBuffer.Clear();
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        context->SetDepthTest(true);
        context->SetBlend(false);
    }
};