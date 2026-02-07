module;

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

export module WFE.Rendering.IRenderer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Systems.IconRenderSystem;
import WFE.Rendering.Core.GLContext;
import WFE.Rendering.RenderingTypes;
import WFE.Rendering.Pipeline.RenderPipeline;
import WFE.Resource.Shader.ShaderManager;

export class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual bool Initialize(GLuint skyboxVAO, GLuint cubemapTexture) = 0;
    virtual void BeginFrame() = 0;
    virtual void Render(ECSWorld& ecs, entt::entity cameraEntity, int width, int height) = 0;
    virtual void EndFrame() = 0;
    virtual void Shutdown() = 0;

    virtual void SetClearColor(const glm::vec4& color) = 0;
    virtual void SetWireframe(bool enable) = 0;
    virtual void SetEnableShadows(bool enable) = 0;
    virtual void SetIconRenderSystem(std::unique_ptr<IconRenderSystem> sys) = 0;

    virtual GLContext* GetContext() = 0;
    virtual RenderPipeline* GetPipeline() = 0;
    virtual const RenderStats& GetStats() const = 0;
    virtual RendererConfig& GetConfig() = 0;
    virtual ECSWorld* GetWorld() = 0;
    virtual ShaderManager* GetShaderManager() = 0;
    virtual bool IsInitialized() const = 0;
};