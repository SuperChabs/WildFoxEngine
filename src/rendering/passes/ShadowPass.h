#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "resource/shader/ShaderManager.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

constexpr int MAX_POINT_LIGHTS = 4;
constexpr int MAX_DIR_SPOT_LIGHTS = 6;
constexpr int MAX_SHADOW_LIGHTS = MAX_DIR_SPOT_LIGHTS + MAX_POINT_LIGHTS;

class ShadowPass : public RenderPass {
    std::vector<GLuint> m_ShadowFBOs;
    GLuint m_CubeShadowFBO = 0;
    GLuint m_ShadowMapArray = 0;
    GLuint m_CubeShadowMapArray = 0;
    int m_ShadowMapSize = 2048;

    ECSWorld *m_World = nullptr;

    std::vector<glm::mat4> m_LightSpaceMatrices;
    std::unordered_map<entt::entity, int> m_ShadowMapIndices;
    std::unordered_map<entt::entity, int> m_PointShadowMapIndices;

    float m_OrthoSize      = 30.0f;
    float m_NearPlane      = 1.0f;
    float m_FarPlane       = 100.0f;
    float m_shadowDistance = 80.0f;
    float m_casterMargin   = 50.0f;

public:
    ShadowPass(ShaderManager *sm, ECSWorld *world);
    ~ShadowPass() override;

    void Setup() override;
    void Execute(FrameContext &ctx) override;
    void Cleanup() override;

    int GetShadowMapSize() const { return m_ShadowMapSize; }

    void SetShadowMapSize(int size);
    void SetOrthoSize(const float size) { m_OrthoSize = size; }
    void SetFarPlane(const float farP)  { m_FarPlane = farP; }

private:
    void InitializeShadowMap(int count);
    void InitializeCubeShadowMap(int count);

    static std::array<glm::vec3, 8> GetFrustumCornersWS(const glm::mat4 &view, const glm::mat4 &projection,
        float sliceNear, float sliceFar);

    glm::mat4 BuildLightSpaceMatrix(const glm::vec3 &lightDir, const std::array<glm::vec3, 8> &corners) const;
    static glm::mat4 BuildSpotLightMatrix(const c_Light &light);
    static std::vector<glm::mat4> BuildPointSpaceMatrices(const c_Light &light);
};