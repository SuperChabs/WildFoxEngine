module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

export module WFE.Rendering.Renderer;

import WFE.Rendering.Core.GLContext;
import WFE.Rendering.Core.Framebuffer;
import WFE.Rendering.Pipeline.RenderPipeline;
import WFE.Rendering.Pipeline.PipelineBuilder;
import WFE.Rendering.RenderStats;
import WFE.Resource.Shader.ShaderManager;
import WFE.ECS.ECSWorld;
import WFE.Core.Camera;
import WFE.Core.Logger;

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

export class Renderer
{
private:
    std::unique_ptr<GLContext> context;
    std::unique_ptr<RenderPipeline> pipeline;
    std::unique_ptr<RenderStats> stats;
    
    ShaderManager* shaderManager;
    ECSWorld* world;
    
    RendererConfig config;
    
    bool initialized = false;

public:
    Renderer(ShaderManager* sm, ECSWorld* w)
        : shaderManager(sm)
        , world(w)
    {
        context = std::make_unique<GLContext>();
        stats = std::make_unique<RenderStats>();
        
        Logger::Log(LogLevel::INFO, "Renderer created");
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
        
        // Apply initial settings
        ApplySettings();
        
        // Build pipeline
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
        
        initialized = true;
        Logger::Log(LogLevel::INFO, "Renderer initialized successfully");
        return true;
    }
    
    void BeginFrame()
    {
        if (!initialized) return;
        
        stats->BeginFrame();
        context->ResetStats();
        
        // Clear
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
        
        stats->EndFrame();
        
        // Update stats from context
        const auto& contextStats = context->GetStats();
        stats->drawCalls = contextStats.drawCalls;
        stats->stateChanges = contextStats.stateChanges;
        stats->vertexCount = contextStats.vertexCount;
        stats->triangleCount = contextStats.triangleCount;
        
        // Log every second
        if (stats->GetFrameCount() % 60 == 0)
        {
            context->LogStats();
            stats->LogFPS();
        }
    }
    
    void Shutdown()
    {
        if (!initialized) return;
        
        pipeline.reset();
        context.reset();
        stats.reset();
        
        initialized = false;
        Logger::Log(LogLevel::INFO, "Renderer shutdown");
    }
    
    // Settings
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
    }
    
    // Getters
    GLContext* GetContext() { return context.get(); }
    RenderPipeline* GetPipeline() { return pipeline.get(); }
    const RenderStats& GetStats() const { return *stats; }
    RendererConfig& GetConfig() { return config; }
    
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
};