module;

#include <glad/glad.h>
#include <memory>
#include <utility>
#include <string>
#include <vector>

export module WFE.Rendering.Pipeline.ForwardPipeline;

import WFE.Rendering.Pipeline.RenderPipeline;
import WFE.Rendering.Passes.GeometryPass;
import WFE.Rendering.Passes.SkyboxPass;
import WFE.Rendering.Passes.UIPass;
import WFE.Rendering.Passes.ShadowPass;
// import WFE.Rendering.Passes.GridPass;
import WFE.ECS.ECSWorld;
import WFE.Core.Logger;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

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
        
        /// shadow pass
        Logger::Log(LogLevel::DEBUG, "Creating ShadowPass...");
        auto shadowPass = std::make_unique<ShadowPass>(context, shaderManager);
        shadowPass->SetEnabled(false);
        AddPass(std::move(shadowPass));
        Logger::Log(LogLevel::DEBUG, "ShadowPass created");
        
        /// skybox pass
        Logger::Log(LogLevel::DEBUG, "Creating SkyboxPass...");
        auto skyboxPass = std::make_unique<SkyboxPass>(
            context, shaderManager, skyboxVAO, cubemapTexture
        );
        AddPass(std::move(skyboxPass));
        Logger::Log(LogLevel::DEBUG, "SkyboxPass created");

        /// geometry pass
        Logger::Log(LogLevel::DEBUG, "Creating GeometryPass...");
        auto geometryPass = std::make_unique<GeometryPass>(
            context, shaderManager, world
        );
        AddPass(std::move(geometryPass));
        Logger::Log(LogLevel::DEBUG, "GeometryPass created");
        
        /// grid pass
        // auto gridPass = std::make_unique<GridPass>(context, shaderManager);
        // AddPass(std::move(gridPass));
        // Logger::Log(LogLevel::DEBUG, "GridPass created");

        /// ui pass2
        Logger::Log(LogLevel::DEBUG, "Creating UIPass...");
        auto uiPass = std::make_unique<UIPass>(
            context, shaderManager
        );
        AddPass(std::move(uiPass));
        Logger::Log(LogLevel::DEBUG, "UIPass created");
        
        Logger::Log(LogLevel::INFO, 
            "Forward pipeline initialized with " + 
            std::to_string(GetPassCount()) + " passes");
    }
};