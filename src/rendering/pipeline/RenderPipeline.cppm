module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <string>

export module WFE.Rendering.Pipeline.RenderPipeline;

import WFE.Rendering.Passes.RenderPass;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.Core.Camera;
import WFE.Core.Logger;

export class RenderPipeline
{
protected:
    std::string name;
    std::vector<std::unique_ptr<RenderPass>> passes;
    GLContext* context;
    ShaderManager* shaderManager;

public:
    RenderPipeline(const std::string& n, GLContext* ctx, ShaderManager* sm)
        : name(n), context(ctx), shaderManager(sm)
    {
        Logger::Log(LogLevel::DEBUG, "RenderPipeline '" + name + "' constructed");
    }
    
    virtual ~RenderPipeline() = default;
    
    virtual void Initialize() = 0;
    
    virtual void Execute(Camera& camera, int width, int height)
    {
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.GetZoom()),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f, 100.0f
        );
        
        for (auto& pass : passes)
        {
            if (pass && pass->IsEnabled())
            {
                pass->Execute(camera, projection);
            }
        }
    }
    
    void AddPass(std::unique_ptr<RenderPass> pass)
    {
        if (!pass)
        {
            Logger::Log(LogLevel::ERROR, "Attempted to add null pass!");
            return;
        }
        
        std::string passName = pass->GetName();
        passes.push_back(std::move(pass));
        Logger::Log(LogLevel::DEBUG, "Added pass: " + passName);
    }
    
    RenderPass* GetPass(const std::string& passName)
    {
        for (auto& pass : passes)
        {
            if (pass && pass->GetName() == passName)
                return pass.get();
        }
        return nullptr;
    }
    
    const std::string& GetName() const { return name; }
    size_t GetPassCount() const { return passes.size(); }
};