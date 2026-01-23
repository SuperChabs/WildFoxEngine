export module WFE.Rendering.Pipeline.ForwardPipeline;

import WFE.Rendering.Pipeline.RenderPipeline;
import WFE.Rendering.Passes.GeometryPass;
import WFE.Rendering.Passes.SkyboxPass;
import WFE.Rendering.Passes.UIPass;
import WFE.ECS.ECSWorld;

export class ForwardPipeline : public RenderPipeline
{
private:
    ECSWorld* world;
    GLuint skyboxVAO;
    GLuint cubemapTexture;

public:
    ForwardPipeline(GLContext* ctx, ShaderManager* sm, ECSWorld* w,
                    GLuint skyVAO, GLuint cubemap)
        : RenderPipeline("ForwardPipeline", ctx, sm)
        , world(w)
        , skyboxVAO(skyVAO)
        , cubemapTexture(cubemap)
    {}
    
    void Initialize() override
    {
        Logger::Log(LogLevel::INFO, "Initializing Forward Rendering Pipeline");
        
        auto geometryPass = std::make_unique<GeometryPass>(
            context, shaderManager, world
        );
        AddPass(std::move(geometryPass));
        
        auto skyboxPass = std::make_unique<SkyboxPass>(
            context, shaderManager, skyboxVAO, cubemapTexture
        );
        AddPass(std::move(skyboxPass));
        
        auto uiPass = std::make_unique<UIPass>(
            context, shaderManager
        );
        AddPass(std::move(uiPass));
        
        Logger::Log(LogLevel::INFO, 
            "Forward pipeline initialized with " + 
            std::to_string(GetPassCount()) + " passes");
    }
};