#include "GeometryPass.h"
#include <string>
#include "core/CommandManager.h"

GeometryPass::GeometryPass(GLContext *ctx, ShaderManager *sm, ECSWorld *w)
    : RenderPass("GeometryPass", ctx, sm)
      , world(w) {
}

void GeometryPass::SetShadowData(const std::vector<glm::mat4> &lightSpaceMatrices, GLuint shadowMapArray,
                                 const std::vector<int> &shadowMapIndices, GLuint shadowCubeMapArray,
                                 const std::vector<int> &CubeShadowMapIndices) {
    m_LightSpaceMatrices = lightSpaceMatrices;
    m_shadowMapArray = shadowMapArray;
    m_ShadowMapIndices = shadowMapIndices;
    m_CubeShadowMapArray = shadowCubeMapArray;
    m_CubeShadowMapIndices = CubeShadowMapIndices;
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
                                       view,                   // 0
                                       projection,             // 1
                                       std::string("basic"), // 2
                                       m_LightSpaceMatrices,   // 3
                                       m_shadowMapArray,       // 4
                                       m_ShadowMapIndices,     // 5
                                       m_CubeShadowMapArray,   // 6
                                       m_CubeShadowMapIndices  // 7
                                   });


    CleanupShadowBinding();
}

void GeometryPass::Cleanup() {
}

void GeometryPass::CleanupShadowBinding() {
    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_SLOT);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glActiveTexture(GL_TEXTURE0 + CUBE_SHADOW_MAP_TEXTURE_SLOT);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);
}