module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>

#include <vector>
#include <memory>
#include <string>

export module WFE.Rendering.Pipeline.RenderPipeline;

import WFE.Rendering.Passes.RenderPass;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.Core.Logger;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

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
    
    virtual void Execute(ECSWorld& ecs, entt::entity cameraEntity, int width, int height)
    {
        if (ecs.HasComponent<CameraComponent>(cameraEntity) && 
            ecs.HasComponent<TransformComponent>(cameraEntity) &&
            ecs.HasComponent<CameraOrientationComponent>(cameraEntity))
        {
            auto& camera = ecs.GetComponent<CameraComponent>(cameraEntity);
            auto& transform = ecs.GetComponent<TransformComponent>(cameraEntity);
            auto& orientation = ecs.GetComponent<CameraOrientationComponent>(cameraEntity);

            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
            glm::mat4 view = orientation.GetViewMatrix(transform.position);

            for (auto& pass : passes)
                if (pass && pass->IsEnabled()) 
                    pass->Execute(view, projection);
        }
        else 
        {
            return;
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
            if (pass && pass->GetName() == passName)
                return pass.get();
        return nullptr;
    }
    
    const std::string& GetName() const { return name; }
    size_t GetPassCount() const { return passes.size(); }
};