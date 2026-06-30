#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "rendering/core/GLContext.h"
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
    std::vector<int> m_ShadowMapIndices;
    std::vector<int> m_PointShadowMapIndices;

    float m_OrthoSize = 30.0f;
    float m_NearPlane = 1.0f;
    float m_FarPlane = 100.0f;

public:
    ShadowPass(GLContext *ctx, ShaderManager *sm, ECSWorld *world);
    ~ShadowPass() override;

    void Setup() override;
    void Execute(const glm::mat4 &, const glm::mat4 &) override;
    void Cleanup() override;

    GLuint GetShadowMapArray() const;
    GLuint GetCubeShadowMapArray() const;
    const std::vector<glm::mat4> &GetLightMatrices() const;
    const std::vector<int> &GetShadowMapIndices() const;
    const std::vector<int> &GetPointShadowMapIndices() const;
    int GetShadowMapSize() const;

    void SetShadowMapSize(int size);
    void SetOrthoSize(float size);
    void SetFarPlane(float farP);

private:
    void InitializeShadowMap(int count);
    void InitializeCubeShadowMap(int count);

    glm::mat4 BuildLightSpaceMatrix(const glm::vec3 &lightDir) const;
    glm::mat4 BuildSpotLightMatrix(const LightComponent &light);
    static std::vector<glm::mat4> BuildPointSpaceMatrices(const LightComponent &light);
};