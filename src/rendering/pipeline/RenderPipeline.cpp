#include "RenderPipeline.h"
#include <glm/glm.hpp>
#include "core/logging/Logger.h"

RenderPipeline::RenderPipeline(const std::string &n, GLContext *ctx, ShaderManager *sm)
    : name(n), context(ctx), shaderManager(sm) {
    Logger::Log(LogLevel::DEBUG, "RenderPipeline '" + name + "' constructed");
}

void RenderPipeline::Execute(CameraComponent &camera, TransformComponent &transform,
    CameraOrientationComponent &orientation, int width, int height) {

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
    glm::mat4 view = orientation.GetViewMatrix(transform.position);

    for (auto &pass: passes)
        if (pass && pass->IsEnabled())
            pass->Execute(view, projection);
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