#include "SkyboxPass.h"

SkyboxPass::SkyboxPass(ShaderManager *sm, GLuint vao, GLuint cubeMap)
    : RenderPass("SkyboxPass", sm), m_skyboxVAO(vao), m_cubeMapTexture(cubeMap) {
}

void SkyboxPass::Setup() {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    m_shaderManager->Bind("skybox");
}

void SkyboxPass::Execute(FrameContext &ctx) {
    if (!m_enabled) return;

    Setup();

    BindTarget(ctx.sceneFBO, ctx.width, ctx.height);

    const auto skyboxView = glm::mat4(glm::mat3(ctx.view));
    m_shaderManager->SetMat4("skybox", "view", skyboxView);
    m_shaderManager->SetMat4("skybox", "projection", ctx.projection);

    glBindVertexArray(m_skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeMapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    Cleanup();
}

void SkyboxPass::Cleanup() {
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    m_shaderManager->Unbind();
}