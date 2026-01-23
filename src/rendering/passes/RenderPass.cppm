module;

#include <glm/glm.hpp>
#include <string>

export module WFE.Rendering.Passes.RenderPass;

import WFE.Rendering.Commands.CommandBuffer;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.Core.Camera;

export class RenderPass
{
protected:
    std::string name;
    CommandBuffer commandBuffer;
    GLContext* context;
    ShaderManager* shaderManager;
    
    bool enabled = true;

public:
    RenderPass(const std::string& n, GLContext* ctx, ShaderManager* sm)
        : name(n), context(ctx), shaderManager(sm)
    {}
    
    virtual ~RenderPass() = default;
    
    virtual void Setup() {}
    virtual void Execute(Camera& camera, const glm::mat4& projection) = 0;
    virtual void Cleanup() {}
    
    void SetEnabled(bool e) { enabled = e; }
    bool IsEnabled() const { return enabled; }
    
    const std::string& GetName() const { return name; }
    
    CommandBuffer& GetCommandBuffer() { return commandBuffer; }
};