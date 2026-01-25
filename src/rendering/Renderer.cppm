module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <chrono>
#include <variant>

export module WFE.Rendering.Renderer;

import WFE.Rendering.Core.GLContext;
import WFE.Rendering.Core.Framebuffer;
import WFE.Rendering.Pipeline.RenderPipeline;
import WFE.Rendering.Pipeline.PipelineBuilder;
import WFE.Resource.Shader.ShaderManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Systems;
import WFE.Core.Camera;
import WFE.Core.Logger;
import WFE.Core.CommandManager;

export struct RendererConfig
{
    bool enableDepthTest = true;
    bool enableCullFace = true;
    bool enableMultisampling = true;
    bool enableWireframe = false;
    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    
    int shadowMapSize = 2048;
    bool enableShadows = false;
};

export struct RenderStats
{
    int drawCalls = 0;
    int stateChanges = 0;
    int vertexCount = 0;
    int triangleCount = 0;
    float frameTime = 0.0f;
    float fps = 0.0f;
    
    void Reset()
    {
        drawCalls = 0;
        stateChanges = 0;
        vertexCount = 0;
        triangleCount = 0;
    }
};

export class Renderer
{
private:
    std::unique_ptr<GLContext> context;
    std::unique_ptr<RenderPipeline> pipeline;
    
    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<LightSystem> lightSystem;
    std::unique_ptr<IconRenderSystem> iconRenderSystem;
    
    ShaderManager* shaderManager;
    ECSWorld* world;
    
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
    Renderer(ShaderManager* sm, ECSWorld* w)
        : shaderManager(sm)
        , world(w)
    {
        if (!shaderManager)
            Logger::Log(LogLevel::ERROR, "Renderer: shaderManager is null in constructor!");
        if (!world)
            Logger::Log(LogLevel::ERROR, "Renderer: world is null in constructor!");
        
        context = std::make_unique<GLContext>();
        
        renderSystem = std::make_unique<RenderSystem>();
        lightSystem = std::make_unique<LightSystem>();
        
        RegisterRenderCommands();
        
        Logger::Log(LogLevel::INFO, "Renderer created with render commands registered");
        Logger::Log(LogLevel::DEBUG, "  - shaderManager: " + std::string(shaderManager ? "OK" : "NULL"));
        Logger::Log(LogLevel::DEBUG, "  - world: " + std::string(world ? "OK" : "NULL"));
    }
    
    ~Renderer()
    {
        Shutdown();
    }
    
    bool Initialize(GLuint skyboxVAO, GLuint cubemapTexture)
    {
        if (initialized)
        {
            Logger::Log(LogLevel::WARNING, "Renderer already initialized");
            return true;
        }
        
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
        
            if (!context) Logger::Log(LogLevel::ERROR, "  - context is null");
            if (!shaderManager) Logger::Log(LogLevel::ERROR, "  - shaderManager is null");
            if (!world) Logger::Log(LogLevel::ERROR, "  - world is null");
            if (skyboxVAO == 0) Logger::Log(LogLevel::ERROR, "  - skyboxVAO is 0");
            if (cubemapTexture == 0) Logger::Log(LogLevel::ERROR, "  - cubemapTexture is 0");
            
            return false;
        }
        
        lastFPSUpdate = Clock::now();
        
        initialized = true;
        Logger::Log(LogLevel::INFO, "Renderer initialized successfully");
        return true;
    }
    
    void BeginFrame()
    {
        if (!initialized) return;
        
        frameStart = Clock::now();
        context->ResetStats();
        stats.Reset();
        
        context->ClearColor(
            config.clearColor.r, 
            config.clearColor.g, 
            config.clearColor.b, 
            config.clearColor.a
        );
        context->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void Render(Camera& camera, int width, int height)
    {
        if (!initialized || !pipeline) return;
        
        pipeline->Execute(camera, width, height);
    }
    
    void EndFrame()
    {
        if (!initialized) return;
        
        auto now = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            now - frameStart
        );
        stats.frameTime = duration.count() / 1000.0f;
        
        frameCount++;
        fpsFrameCount++;
        
        auto fpsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastFPSUpdate
        );
        
        if (fpsDuration.count() >= 1000)
        {
            stats.fps = fpsFrameCount / (fpsDuration.count() / 1000.0f);
            fpsFrameCount = 0;
            lastFPSUpdate = now;
        }
        
        const auto& contextStats = context->GetStats();
        stats.drawCalls = contextStats.drawCalls;
        stats.stateChanges = contextStats.stateChanges;
        stats.vertexCount = contextStats.vertexCount;
        stats.triangleCount = contextStats.triangleCount;
        
        if (frameCount % 60 == 0)
        {
            LogStats();
        }
    }
    
    void Shutdown()
    {
        if (!initialized) return;
        
        pipeline.reset();
        renderSystem.reset();
        lightSystem.reset();
        iconRenderSystem.reset();
        context.reset();
        
        initialized = false;
        Logger::Log(LogLevel::INFO, "Renderer shutdown");
    }
    
    void SetClearColor(const glm::vec4& color)
    {
        config.clearColor = color;
    }
    
    void SetWireframe(bool enable)
    {
        config.enableWireframe = enable;
        if (enable)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    void SetEnableShadows(bool enable)
    {
        config.enableShadows = enable;
        if (pipeline)
        {
            auto* shadowPass = pipeline->GetPass("ShadowPass");
            if (shadowPass)
                shadowPass->SetEnabled(enable);
        }
    }

    // Getters
    GLContext* GetContext() { return context.get(); }
    RenderPipeline* GetPipeline() { return pipeline.get(); }
    const RenderStats& GetStats() const { return stats; }
    RendererConfig& GetConfig() { return config; }
    
    void SetIconRenderSystem(std::unique_ptr<IconRenderSystem> sys)
    {
        iconRenderSystem = std::move(sys);
    }
    
    ECSWorld* GetWorld() { return world; }
    ShaderManager* GetShaderManager() { return shaderManager; }
    
    bool IsInitialized() const { return initialized; }

private:
    void ApplySettings()
    {
        if (config.enableDepthTest)
            context->SetDepthTest(true);
        
        if (config.enableCullFace)
            context->SetCullFace(true);
        
        if (config.enableMultisampling)
            glEnable(GL_MULTISAMPLE);
        
        if (config.enableWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
        CommandManager::RegisterCommand("Renderer_RenderGeometry",
        [this](const CommandArgs& args) 
        {
            if (args.size() < 3)
            {
                Logger::Log(LogLevel::ERROR, "Renderer_RenderGeometry requires 3 args");
                return;
            }
            
            auto* camera = std::get<Camera*>(args[0]);
            const auto& projection = std::get<glm::mat4>(args[1]);
            const auto& shaderName = std::get<std::string>(args[2]);
            
            shaderManager->Bind(shaderName);
            
            glm::mat4 view = camera->GetViewMatrix();
            shaderManager->SetMat4(shaderName, "projection", projection);
            shaderManager->SetMat4(shaderName, "view", view);
            shaderManager->SetVec3(shaderName, "viewPos", camera->GetPosition());
            
            lightSystem->Update(*world, *shaderManager, shaderName);
            renderSystem->Update(*world, *shaderManager, shaderName, *camera);
            
            shaderManager->Unbind();
        });
        
        CommandManager::RegisterCommand("Renderer_RenderUI",
        [this](const CommandArgs& args) 
        {
            if (args.size() < 3)
            {
                Logger::Log(LogLevel::ERROR, "Renderer_RenderUI requires 3 args");
                return;
            }
            
            auto* camera = std::get<Camera*>(args[0]);
            const auto& projection = std::get<glm::mat4>(args[1]);
            const auto& shaderName = std::get<std::string>(args[2]);
            
            if (!iconRenderSystem)
                return;
            
            shaderManager->Bind(shaderName);
            
            glm::mat4 view = camera->GetViewMatrix();
            shaderManager->SetMat4(shaderName, "projection", projection);
            shaderManager->SetMat4(shaderName, "view", view);
            
            iconRenderSystem->Update(*world, *shaderManager, shaderName, *camera);
            
            shaderManager->Unbind();
        });
        
        Logger::Log(LogLevel::INFO, "Render commands registered in CommandManager");
    }
};