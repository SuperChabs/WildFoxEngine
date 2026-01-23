export module WFE.Rendering.Pipeline.PipelineBuilder;

import WFE.Rendering.Pipeline.RenderPipeline;
import WFE.Rendering.Pipeline.ForwardPipeline;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.ECS.ECSWorld;
import WFE.Core.Logger;

export enum class PipelineType
{
    FORWARD,
    DEFERRED,
    CUSTOM
};

export class PipelineBuilder
{
private:
    GLContext* context = nullptr;
    ShaderManager* shaderManager = nullptr;
    ECSWorld* world = nullptr;
    
    GLuint skyboxVAO = 0;
    GLuint cubemapTexture = 0;
    
    PipelineType type = PipelineType::FORWARD;

public:
    PipelineBuilder& SetContext(GLContext* ctx)
    {
        context = ctx;
        return *this;
    }
    
    PipelineBuilder& SetShaderManager(ShaderManager* sm)
    {
        shaderManager = sm;
        return *this;
    }
    
    PipelineBuilder& SetWorld(ECSWorld* w)
    {
        world = w;
        return *this;
    }
    
    PipelineBuilder& SetSkybox(GLuint vao, GLuint cubemap)
    {
        skyboxVAO = vao;
        cubemapTexture = cubemap;
        return *this;
    }
    
    PipelineBuilder& SetType(PipelineType t)
    {
        type = t;
        return *this;
    }
    
    std::unique_ptr<RenderPipeline> Build()
    {
        if (!context || !shaderManager || !world)
        {
            Logger::Log(LogLevel::ERROR, 
                "PipelineBuilder: Missing required components");
            return nullptr;
        }
        
        std::unique_ptr<RenderPipeline> pipeline;
        
        switch (type)
        {
            case PipelineType::FORWARD:
                pipeline = std::make_unique<ForwardPipeline>(
                    context, shaderManager, world, 
                    skyboxVAO, cubemapTexture
                );
                break;
                
            case PipelineType::DEFERRED:
                Logger::Log(LogLevel::WARNING, 
                    "Deferred pipeline not implemented yet");
                return nullptr;
                
            case PipelineType::CUSTOM:
                Logger::Log(LogLevel::WARNING, 
                    "Custom pipeline requires manual setup");
                return nullptr;
        }
        
        if (pipeline)
        {
            pipeline->Initialize();
            Logger::Log(LogLevel::INFO, 
                "Pipeline built successfully: " + pipeline->GetName());
        }
        
        return pipeline;
    }
};