#include "GeometryPass.h"
#include <string>
#include "core/CommandManager.h"
#include "ECS/systems/LightSystem.h"
#include "ECS/systems/RenderSystem.h"

static constexpr int SHADOW_MAP_SLOT = 6;
static constexpr int CUBE_SHADOW_MAP_SLOTS = 4;

GeometryPass::GeometryPass(ShaderManager *sm, ECSWorld *w)
    : RenderPass("GeometryPass", sm), world(w) {
}

void GeometryPass::Setup() {
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void GeometryPass::Execute(FrameContext &ctx) {
    if (!m_enabled || !world) return;
    Setup();

    BindTarget(ctx.sceneFBO, ctx.width, ctx.height);

    const std::string shaderName = "basic";

    m_shaderManager->Bind(shaderName);
    m_shaderManager->SetMat4(shaderName, "projection", ctx.projection);
    m_shaderManager->SetMat4(shaderName, "view", ctx.view);

    bool shadowsEnabled = false;

    for (int i = 0; i < ctx.shadows.lightMatrices->size(); i++)
        m_shaderManager->SetMat4(
            shaderName, "lightSpaceMatrices[" + std::to_string(i) + "]", ctx.shadows.lightMatrices->at(i));

    if (ctx.shadows.dirSpotArray != 0) {
        shadowsEnabled = true;
        m_shaderManager->SetBool(shaderName, "shadowsEnabled", true);

        m_shaderManager->SetInt(shaderName, "shadowMapArray", SHADOW_MAP_SLOT);
        glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_SLOT);
        glBindTexture(GL_TEXTURE_2D_ARRAY, ctx.shadows.dirSpotArray);

        m_shaderManager->SetInt(shaderName, "shadowCubeArray", CUBE_SHADOW_MAP_SLOTS);
        m_shaderManager->SetFloat(shaderName, "pointLightFarPlane", 100.0f);
        glActiveTexture(GL_TEXTURE0 + CUBE_SHADOW_MAP_SLOTS);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ctx.shadows.cubeArray);
    }

    if (!shadowsEnabled)
        m_shaderManager->SetBool(shaderName, "shadowsEnabled", false);

    LightSystem::Update(*world, *m_shaderManager, shaderName, ctx.shadows.dirSpotIndices, ctx.shadows.cubeIndices);
    RenderSystem::Update(*world, *m_shaderManager, shaderName);

    m_shaderManager->Unbind();

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