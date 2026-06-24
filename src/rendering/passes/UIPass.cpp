#include "UIPass.h"
#include <string>
#include "core/CommandManager.h"

UIPass::UIPass(GLContext *ctx, ShaderManager *sm)
    : RenderPass("UIPass", ctx, sm) {
}

void UIPass::Setup() {
    context->SetDepthTest(true);
    context->SetDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
    context->SetBlend(true);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void UIPass::Execute(const glm::mat4 &view, const glm::mat4 &projection) {
    if (!enabled) return;

    Setup();

    CommandManager::ExecuteCommand("Renderer_RenderUI",
                                   {
                                       view,
                                       projection,
                                       std::string("icon")
                                   });

    Cleanup();
}

void UIPass::Cleanup() {
    glDepthMask(GL_TRUE);
    context->SetDepthTest(true);
    context->SetBlend(false);
}