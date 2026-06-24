#include "GeometryPass.h"
#include <string>
#include "core/CommandManager.h"

GeometryPass::GeometryPass(GLContext *ctx, ShaderManager *sm, ECSWorld *w)
    : RenderPass("GeometryPass", ctx, sm)
      , world(w) {
}

void GeometryPass::SetShadowData(const std::vector<glm::mat4> &lightSpaceMatrices, GLuint shadowMapArray,
                                 const std::vector<int> &shadowMapIndices) {
    m_LightSpaceMatrices = lightSpaceMatrices;
    m_shadowMapArray = shadowMapArray;
    m_ShadowMapIndices = shadowMapIndices;
}

void GeometryPass::Setup() {
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST); //context->SetDepthTest(true);
    glDepthFunc(GL_LESS); //context->SetDepthFunc(GL_LESS);
    glEnable(GL_BLEND); //context->SetBlend(false);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE); //context->SetCullFace(true);
    glCullFace(GL_BACK);
}

void GeometryPass::Execute(const glm::mat4 &view, const glm::mat4 &projection) {
    if (!enabled || !world) return;
    Setup();

    CommandManager::ExecuteCommand("Renderer_RenderGeometry",
                                   {
                                       view,
                                       projection,
                                       std::string("basic"),
                                       m_LightSpaceMatrices,
                                       static_cast<int>(m_shadowMapArray),
                                       m_ShadowMapIndices
                                   });


    CleanupShadowBinding();
}

void GeometryPass::Cleanup() {
}

void GeometryPass::CleanupShadowBinding() {
    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_SLOT);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}