#pragma once

#include <memory>
#include <chrono>
#include <entt/entt.hpp>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/core/GLContext.h"
#include "rendering/core/Framebuffer.h"
#include "rendering/pipeline/RenderPipeline.h"
#include "rendering/RenderingTypes.h"
#include "resource/shader/ShaderManager.h"
#include "resource/texture/TextureManager.h"
#include "ECS/World.h"
#include "ECS/systems/Systems.h"

static constexpr int SHADOW_MAP_SLOT = 6;
static constexpr int CUBE_SHADOW_MAP_SLOTS = 4;

class Renderer {
    std::unique_ptr<GLContext> context;
    std::unique_ptr<RenderPipeline> pipeline;

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

    ~Renderer();

    bool Initialize(GLuint skyboxVAO, GLuint cubemapTexture);
    void Shutdown();

    void BeginFrame();
    void Render(c_Camera &camera, c_Transform &transform, c_CameraOrientation &orientation, int width, int height);
    void EndFrame();

    void SetClearColor(const glm::vec4 &color) { config.clearColor = color; }
    void SetWireframe(bool enable);
    void SetEnableShadows(bool enable);

    [[nodiscard]] const RenderStats &GetStats()  const { return stats; }
    [[nodiscard]] RendererConfig     GetConfig() const { return config; }
    [[nodiscard]] IconRenderSystem  *GetIcon()   const { return m_icon.get(); }

    [[nodiscard]] bool IsInitialized() const { return initialized; }

private:
    void ApplySettings();

    void LogStats() const;
};