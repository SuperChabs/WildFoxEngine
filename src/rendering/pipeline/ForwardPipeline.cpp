#include "ForwardPipeline.h"
#include <glm/glm.hpp>
#include "rendering/passes/SkyboxPass.h"
#include "rendering/passes/UIPass.h"
#include "core/logging/Logger.h"

ForwardPipeline::ForwardPipeline(GLContext *ctx, ShaderManager *sm, ECSWorld *w,
                                 GLuint skyVAO, GLuint cubemap)
    : RenderPipeline("ForwardPipeline", ctx, sm)
      , world(w)
      , skyboxVAO(skyVAO)
      , cubemapTexture(cubemap) {
}

void ForwardPipeline::Initialize() {
    Logger::Log(LogLevel::INFO, "Initializing Forward Rendering Pipeline");

    // --- SkyboxPass ---
    auto skyboxPass = std::make_unique<SkyboxPass>(
        context, shaderManager, skyboxVAO, cubemapTexture);
    AddPass(std::move(skyboxPass));
    Logger::Log(LogLevel::DEBUG, "SkyboxPass created");

    // --- ShadowPass ---
    auto shadowPassOwned = std::make_unique<ShadowPass>(context, shaderManager, world);
    m_ShadowPassPtr = shadowPassOwned.get();
    shadowPassOwned->SetEnabled(true);
    AddPass(std::move(shadowPassOwned));
    Logger::Log(LogLevel::DEBUG, "ShadowPass created");

    // --- GeometryPass ---
    auto geometryPassOwned = std::make_unique<GeometryPass>(context, shaderManager, world);
    m_GeometryPassPtr = geometryPassOwned.get();
    AddPass(std::move(geometryPassOwned));
    Logger::Log(LogLevel::DEBUG, "GeometryPass created");

    // --- UIPass ---
    auto uiPass = std::make_unique<UIPass>(context, shaderManager);
    AddPass(std::move(uiPass));
    Logger::Log(LogLevel::DEBUG, "UIPass created");

    Logger::Log(LogLevel::INFO,
                "Forward pipeline initialized with " +
                std::to_string(GetPassCount()) + " passes");
}

void ForwardPipeline::Execute(ECSWorld &ecs, entt::entity cameraEntity,
                              int width, int height) {
    if (!ecs.HasComponent<CameraComponent>(cameraEntity) ||
        !ecs.HasComponent<TransformComponent>(cameraEntity) ||
        !ecs.HasComponent<CameraOrientationComponent>(cameraEntity))
        return;

    auto &camera = ecs.GetComponent<CameraComponent>(cameraEntity);
    auto &transform = ecs.GetComponent<TransformComponent>(cameraEntity);
    auto &orientation = ecs.GetComponent<CameraOrientationComponent>(cameraEntity);

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
    glm::mat4 view = orientation.GetViewMatrix(transform.position);

    if (m_ShadowPassPtr &&m_ShadowPassPtr
    
    ->
    IsEnabled()
    )
    {
        m_ShadowPassPtr->Execute(view, projection);

        glViewport(0, 0, width, height);

        m_GeometryPassPtr->SetShadowData(
            m_ShadowPassPtr->GetLightMatrices(),
            m_ShadowPassPtr->GetShadowMapArray(),
            m_ShadowPassPtr->GetShadowMapIndices()
        );
    }
    else
    if (m_GeometryPassPtr) {
        m_GeometryPassPtr->SetShadowData({glm::mat4(1.0f)}, 0);
    }

    for (auto &pass: passes) {
        if (!pass || !pass->IsEnabled()) continue;
        if (pass.get() == m_ShadowPassPtr) continue;
        pass->Execute(view, projection);
    }
}

ShadowPass *ForwardPipeline::GetShadowPass() const {
    return m_ShadowPassPtr;
}