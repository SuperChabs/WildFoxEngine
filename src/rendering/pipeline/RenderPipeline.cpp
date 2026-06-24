#include "RenderPipeline.h"
#include <glm/glm.hpp>
#include "core/logging/Logger.h"

RenderPipeline::RenderPipeline(const std::string &n, GLContext *ctx, ShaderManager *sm)
    : name(n), context(ctx), shaderManager(sm) {
    Logger::Log(LogLevel::DEBUG, "RenderPipeline '" + name + "' constructed");
}

void RenderPipeline::Execute(ECSWorld &ecs, entt::entity cameraEntity, int width, int height) {
    if (ecs.HasComponent<CameraComponent>(cameraEntity) &&
        ecs.HasComponent<TransformComponent>(cameraEntity) &&
        ecs.HasComponent<CameraOrientationComponent>(cameraEntity)) {
        auto &camera = ecs.GetComponent<CameraComponent>(cameraEntity);
        auto &transform = ecs.GetComponent<TransformComponent>(cameraEntity);
        auto &orientation = ecs.GetComponent<CameraOrientationComponent>(cameraEntity);

        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
        glm::mat4 view = orientation.GetViewMatrix(transform.position);

        for (auto &pass: passes)
            if (pass && pass->IsEnabled())
                pass->Execute(view, projection);
    } else {
        return;
    }
}

void RenderPipeline::AddPass(std::unique_ptr<RenderPass> pass) {
    if (!pass) {
        Logger::Log(LogLevel::ERROR, "Attempted to add null pass!");
        return;
    }

    std::string passName = pass->GetName();
    passes.push_back(std::move(pass));
    Logger::Log(LogLevel::DEBUG, "Added pass: " + passName);
}

RenderPass *RenderPipeline::GetPass(const std::string &passName) {
    for (auto &pass: passes)
        if (pass && pass->GetName() == passName)
            return pass.get();
    return nullptr;
}

const std::string &RenderPipeline::GetName() const {
    return name;
}

size_t RenderPipeline::GetPassCount() const {
    return passes.size();
}