#include "ForwardPipeline.h"

#include "rendering/passes/SkyboxPass.h"
#include "rendering/passes/UIPass.h"
#include "core/logging/Logger.h"
#include "rendering/passes/PostPass.h"

ForwardPipeline::ForwardPipeline(GLContext *ctx, ShaderManager *sm, ECSWorld *w, GLuint skyVAO, GLuint cubeMap)
    : RenderPipeline("ForwardPipeline", ctx, sm), m_world(w), m_skyboxVAO(skyVAO), m_cubeMapTexture(cubeMap) {
}

void ForwardPipeline::Initialize() {
    Logger::Log(LogLevel::INFO, "Initializing Forward Rendering Pipeline");

    // --- ShadowPass ---
    auto shadowPass = std::make_unique<ShadowPass>(shaderManager, m_world);
    AddPass(std::move(shadowPass));
    Logger::Log(LogLevel::DEBUG, "ShadowPass created");

    // --- GeometryPass ---
    auto geometryPass = std::make_unique<GeometryPass>(shaderManager, m_world);
    AddPass(std::move(geometryPass));
    Logger::Log(LogLevel::DEBUG, "GeometryPass created");

    // --- SkyboxPass ---
    auto skyboxPass = std::make_unique<SkyboxPass>(shaderManager, m_skyboxVAO, m_cubeMapTexture);
    AddPass(std::move(skyboxPass));
    Logger::Log(LogLevel::DEBUG, "SkyboxPass created");

    // --- UIPass ---
    auto uiPass = std::make_unique<UIPass>(shaderManager);
    AddPass(std::move(uiPass));
    Logger::Log(LogLevel::DEBUG, "UIPass created");

    // --- PostPass ---
    auto postPass = std::make_unique<PostPass>(shaderManager);
    AddPass(std::move(postPass));
    Logger::Log(LogLevel::DEBUG, "PostPass created");

    Logger::Log(LogLevel::INFO, "Forward pipeline initialized with " + std::to_string(GetPassCount()) + " passes");
}