#include "SkyboxPass.h"

SkyboxPass::SkyboxPass(GLContext *ctx, ShaderManager *sm, GLuint vao, GLuint cubemap)
    : RenderPass("SkyboxPass", ctx, sm)
      , skyboxVAO(vao)
      , cubemapTexture(cubemap) {
}

void SkyboxPass::Setup() {
    glDepthMask(GL_FALSE);
    context->SetDepthFunc(GL_LEQUAL);

    shaderManager->Bind("skybox");
}

void SkyboxPass::Execute(const glm::mat4 &view, const glm::mat4 &projection) {
    if (!enabled) return;

    Setup();

    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    shaderManager->SetMat4("skybox", "view", skyboxView);
    shaderManager->SetMat4("skybox", "projection", projection);

    context->BindVAO(skyboxVAO);
    context->BindTexture2D(cubemapTexture, 0);
    context->DrawArrays(GL_TRIANGLES, 0, 36);
    context->UnbindVAO();

    Cleanup();
}

void SkyboxPass::Cleanup() {
    glDepthMask(GL_TRUE);
    context->SetDepthFunc(GL_LESS);
    shaderManager->Unbind();
}
