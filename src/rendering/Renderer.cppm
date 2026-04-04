module;

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

export module WFE.Rendering.Renderer;

import WFE.Rendering.IRenderer;
import WFE.Rendering.Core.GLContext;
import WFE.Rendering.Core.Framebuffer;
import WFE.Rendering.Pipeline.RenderPipeline;
import WFE.Rendering.Pipeline.PipelineBuilder;
import WFE.Rendering.Pipeline.ForwardPipeline;
import WFE.Rendering.RenderingTypes;
import WFE.Resource.Shader.ShaderManager;
import WFE.Resource.Texture.TextureManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Systems;
import WFE.Core.Logger;
import WFE.Core.CommandManager;

static constexpr int SHADOW_MAP_SLOT = 8;

export class Renderer : public IRenderer
{
    std::unique_ptr<GLContext>       context;
    std::unique_ptr<RenderPipeline>  pipeline;

    std::unique_ptr<RenderSystem>    renderSystem;
    std::unique_ptr<LightSystem>     lightSystem;
    std::unique_ptr<IconRenderSystem> m_icon;

    ShaderManager* shaderManager;
    ECSWorld*      world;

    RendererConfig config;
    RenderStats    stats;

    bool initialized = false;

    using Clock      = std::chrono::high_resolution_clock;
    using TimePoint  = std::chrono::time_point<Clock>;
    TimePoint frameStart;
    TimePoint lastFPSUpdate;
    int frameCount    = 0;
    int fpsFrameCount = 0;

public:
    Renderer(ShaderManager* sm, ECSWorld* w, TextureManager* tm)
        : shaderManager(sm)
        , world(w)
    {
        if (!shaderManager)
            Logger::Log(LogLevel::ERROR, "Renderer: shaderManager is null!");
        if (!world)
            Logger::Log(LogLevel::ERROR, "Renderer: world is null!");

        context      = std::make_unique<GLContext>();
        renderSystem = std::make_unique<RenderSystem>();
        lightSystem  = std::make_unique<LightSystem>();
        m_icon       = std::make_unique<IconRenderSystem>(tm);

        RegisterRenderCommands();
    }

    ~Renderer() override { Shutdown(); }

    bool Initialize(GLuint skyboxVAO, GLuint cubemapTexture) override
    {
        if (initialized) return true;

        ApplySettings();

        PipelineBuilder builder;
        pipeline = builder
            .SetContext(context.get())
            .SetShaderManager(shaderManager)
            .SetWorld(world)
            .SetSkybox(skyboxVAO, cubemapTexture)
            .SetType(PipelineType::FORWARD)
            .Build();

        if (!pipeline)
        {
            Logger::Log(LogLevel::ERROR, "Failed to build render pipeline");
            return false;
        }

        lastFPSUpdate = Clock::now();
        initialized   = true;
        Logger::Log(LogLevel::INFO, "Renderer initialized successfully");
        return true;
    }

    void BeginFrame() override
    {
        if (!initialized) return;

        frameStart = Clock::now();
        context->ResetStats();
        stats.Reset();

        context->ClearColor(
            config.clearColor.x, config.clearColor.y,
            config.clearColor.z, config.clearColor.w);
        context->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Render(ECSWorld& ecs, entt::entity cameraEntity,
                int width, int height) override
    {
        if (!initialized || !pipeline) return;
        pipeline->Execute(ecs, cameraEntity, width, height);
    }

    void EndFrame() override
    {
        if (!initialized) return;

        auto now      = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - frameStart);
        stats.frameTime = duration.count() / 1000.0f;

        frameCount++;
        fpsFrameCount++;

        auto fpsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFPSUpdate);
        if (fpsDuration.count() >= 1000)
        {
            stats.fps     = fpsFrameCount / (fpsDuration.count() / 1000.0f);
            fpsFrameCount = 0;
            lastFPSUpdate = now;
        }

        const auto& cs  = context->GetStats();
        stats.drawCalls  = cs.drawCalls;
        stats.stateChanges = cs.stateChanges;
        stats.vertexCount  = cs.vertexCount;
        stats.triangleCount = cs.triangleCount;

        if (frameCount % 60 == 0) LogStats();
    }

    void Shutdown() override
    {
        if (!initialized) return;
        pipeline.reset();
        renderSystem.reset();
        lightSystem.reset();
        m_icon.reset();
        context.reset();
        initialized = false;
        Logger::Log(LogLevel::INFO, "Renderer shutdown");
    }

    void SetClearColor(const glm::vec4& color) override { config.clearColor = color; }

    void SetWireframe(bool enable) override
    {
        config.enableWireframe = enable;
        glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
    }

    void SetEnableShadows(bool enable) override
    {
        config.enableShadows = enable;

        if (!pipeline) return;

        if (auto* fp = dynamic_cast<ForwardPipeline*>(pipeline.get()))
        {
            if (auto* sp = fp->GetShadowPass())
                sp->SetEnabled(enable);
        }

        Logger::Log(LogLevel::INFO,
            std::string("Shadows ") + (enable ? "enabled" : "disabled"));
    }

    GLContext*           GetContext()       override { return context.get(); }
    RenderPipeline*      GetPipeline()      override { return pipeline.get(); }
    const RenderStats&   GetStats()   const override { return stats; }
    RendererConfig&      GetConfig()        override { return config; }
    ECSWorld*            GetWorld()         override { return world; }
    ShaderManager*       GetShaderManager() override { return shaderManager; }
    IconRenderSystem*    GetIcon()                   { return m_icon.get(); }
    bool                 IsInitialized() const override { return initialized; }

private:
    void ApplySettings()
    {
        if (config.enableDepthTest)   context->SetDepthTest(true);
        if (config.enableCullFace)    context->SetCullFace(true);
        if (config.enableMultisampling) glEnable(GL_MULTISAMPLE);
        if (config.enableWireframe)   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void LogStats() const
    {
        Logger::Log(LogLevel::DEBUG,
            "FPS: " + std::to_string(static_cast<int>(stats.fps)) +
            " | Frame: " + std::to_string(stats.frameTime) + "ms" +
            " | Draws: " + std::to_string(stats.drawCalls) +
            " | Tris: " + std::to_string(stats.triangleCount));
    }

    void RegisterRenderCommands()
    {
        if (CommandManager::HasCommand("Renderer_RenderGeometry"))
        {
            Logger::Log(LogLevel::WARNING,
                "Renderer: Renderer_RenderGeometry already registered — skipping (hot-reload path?)");
        }
        else
            CommandManager::RegisterCommand("Renderer_RenderGeometry",
            [this](const CommandArgs& args)
            {
                if (args.size() < 3)
                {
                    Logger::Log(LogLevel::ERROR, "Renderer_RenderGeometry: needs ≥3 args");
                    return;
                }

                const auto& view       = std::get<glm::mat4>(args[0]);
                const auto& projection = std::get<glm::mat4>(args[1]);
                const auto& shaderName = std::get<std::string>(args[2]);

                shaderManager->Bind(shaderName);
                shaderManager->SetMat4(shaderName, "projection", projection);
                shaderManager->SetMat4(shaderName, "view", view);

                bool shadowsEnabled = false;
                if (args.size() >= 5)
                {
                    const auto& lightSpaceMat = std::get<glm::mat4>(args[3]);
                    int shadowTexID           = std::get<int>(args[4]);

                    shaderManager->SetMat4(shaderName, "lightSpaceMatrix", lightSpaceMat);

                    if (shadowTexID != 0)
                    {
                        shadowsEnabled = true;
                        shaderManager->SetBool(shaderName, "shadowsEnabled", true);
                        shaderManager->SetInt(shaderName, "shadowMap", SHADOW_MAP_SLOT);

                        glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_SLOT);
                        glBindTexture(GL_TEXTURE_2D, shadowTexID);
                    }
                }

                if (!shadowsEnabled)
                    shaderManager->SetBool(shaderName, "shadowsEnabled", false);

                lightSystem->Update(*world, *shaderManager, shaderName);
                renderSystem->Update(*world, *shaderManager, shaderName);

                shaderManager->Unbind();
            });

        if (!CommandManager::HasCommand("Renderer_RenderUI"))
            CommandManager::RegisterCommand("Renderer_RenderUI",
            [this](const CommandArgs& args)
            {
                if (args.size() < 3) return;

                const auto& view       = std::get<glm::mat4>(args[0]);
                const auto& projection = std::get<glm::mat4>(args[1]);
                const auto& shaderName = std::get<std::string>(args[2]);

                shaderManager->Bind(shaderName);
                shaderManager->SetMat4(shaderName, "projection", projection);
                shaderManager->SetMat4(shaderName, "view", view);
                shaderManager->Unbind();
            });

        Logger::Log(LogLevel::INFO, "Render commands registered");
    }
};