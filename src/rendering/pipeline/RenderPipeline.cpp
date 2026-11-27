#include "RenderPipeline.h"

#include <glm/glm.hpp>

#include "core/logging/Logger.h"
#include "rendering/passes/PassContext.h"

RenderPipeline::RenderPipeline(const std::string &n, GLContext *ctx, ShaderManager *sm)
    : name(n), context(ctx), shaderManager(sm) {
    Logger::Log(LogLevel::DEBUG, "RenderPipeline '" + name + "' constructed");
}

void RenderPipeline::Execute(c_Camera &camera, c_Transform &transform, c_CameraOrientation &orientation, int width, int height) {
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    const glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
    const glm::mat4 view = orientation.GetViewMatrix(transform.position);

    GLint target = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &target);

    auto ctx = FrameContext{
        .view       = view,
        .projection = projection,
        .cameraPos  = transform.position,
        .targetFBO  = (GLuint)target,
        .width      = width,
        .height     = height
    };

    if (!m_sceneFBO) m_sceneFBO = std::make_unique<Framebuffer>(width, height);
    else if (m_sceneFBO->GetWidth() != width || m_sceneFBO->GetHeight() != height)
        m_sceneFBO->Resize(width, height);

    ctx.sceneFBO   = m_sceneFBO->GetID();
    ctx.sceneColor = m_sceneFBO->GetTextureID();

    glBindFramebuffer(GL_FRAMEBUFFER, ctx.sceneFBO);
    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto &pass: passes)
        if (pass && pass->IsEnabled())
            pass->Execute(ctx);
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