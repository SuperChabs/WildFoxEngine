#pragma once

#include <vector>
#include <variant>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "ECS/World.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"
#include "core/logging/Logger.h"

class GeometryPass : public RenderPass {
    ECSWorld *world;

    std::vector<glm::mat4> m_LightSpaceMatrices;
    std::vector<int> m_ShadowMapIndices;
    GLuint m_shadowMapArray;
    GLuint m_CubeShadowMapArray;
    std::vector<int> m_CubeShadowMapIndices;

    static constexpr int SHADOW_MAP_TEXTURE_SLOT = 6;
    static constexpr int CUBE_SHADOW_MAP_TEXTURE_SLOT = 4;

public:
    GeometryPass(GLContext *ctx, ShaderManager *sm, ECSWorld *w);

    void SetShadowData(const std::vector<glm::mat4> &lightSpaceMatrices, GLuint shadowMapArray,
                       const std::vector<int> &shadowMapIndices = {}, GLuint shadowCubeMapArray = 0,
                       const std::vector<int> &CubeShadowMapIndices = {});

    void Setup() override;

    void Execute(const glm::mat4 &view, const glm::mat4 &projection) override;

    void Cleanup() override;

private:
    void CleanupShadowBinding();
};