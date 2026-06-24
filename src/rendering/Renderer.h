#pragma once

#include <memory>
#include <chrono>
#include <entt/entt.hpp>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/IRenderer.h"
#include "rendering/core/GLContext.h"
#include "rendering/core/Framebuffer.h"
#include "rendering/pipeline/RenderPipeline.h"
#include "rendering/RenderingTypes.h"
#include "resource/shader/ShaderManager.h"
#include "resource/texture/TextureManager.h"
#include "ECS/World.h"
#include "ECS/systems/Systems.h"

static constexpr int SHADOW_MAP_SLOT = 8;

class Renderer : public IRenderer {
    std::unique_ptr<GLContext> context;
    std::unique_ptr<RenderPipeline> pipeline;

    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<LightSystem> lightSystem;
    std::unique_ptr<IconRenderSystem> m_icon;

    ShaderManager *shaderManager;
    ECSWorld *world;

    RendererConfig config;
    RenderStats stats;

    bool initialized = false;

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint frameStart;
    TimePoint lastFPSUpdate;
    int frameCount = 0;
    int fpsFrameCount = 0;

public:
    Renderer(ShaderManager *sm, ECSWorld *w, TextureManager *tm);

    ~Renderer() override;

    bool Initialize(GLuint skyboxVAO, GLuint cubemapTexture) override;

    void BeginFrame() override;

    void Render(ECSWorld &ecs, entt::entity cameraEntity,
                int width, int height) override;

    void EndFrame() override;

    void Shutdown() override;

    void SetClearColor(const glm::vec4 &color) override;

    void SetWireframe(bool enable) override;

    void SetEnableShadows(bool enable) override;

    GLContext *GetContext() override;

    RenderPipeline *GetPipeline() override;

    const RenderStats &GetStats() const override;

    RendererConfig &GetConfig() override;

    ECSWorld *GetWorld() override;

    ShaderManager *GetShaderManager() override;

    IconRenderSystem *GetIcon();

    bool IsInitialized() const override;

private:
    void ApplySettings();

    void LogStats() const;

    void RegisterRenderCommands();
};