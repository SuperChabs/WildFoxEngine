#include "UIPass.h"
#include <string>
#include "core/CommandManager.h"

UIPass::UIPass( ShaderManager *sm)
    : RenderPass("UIPass", sm) {
}

void UIPass::Setup() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void UIPass::Execute(FrameContext &ctx) {
    if (!m_enabled) return;

    Setup();

    BindTarget(ctx.sceneFBO, ctx.width, ctx.height);

    std::string shaderName = "icon";

    m_shaderManager->Bind(shaderName);
    m_shaderManager->SetMat4(shaderName, "projection", ctx.projection);
    m_shaderManager->SetMat4(shaderName, "view", ctx.view);
    m_shaderManager->Unbind();

    Cleanup();
}

void UIPass::Cleanup() {
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}