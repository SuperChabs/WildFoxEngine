#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "core/CommandManager.h"
#include "scene/Light.h"

#define MAX_SHADOW_LIGHTS 8

class ShadowPass : public RenderPass {
private:
    std::vector<GLuint> m_ShadowFBOs;
    GLuint m_ShadowMapArray = 0;
    int m_ShadowMapSize = 2048;

    ECSWorld *m_World = nullptr;

    std::vector<glm::mat4> m_LightSpaceMatrices;
    std::vector<int> m_ShadowMapIndices; // maps global light index -> shadow map index

    float m_OrthoSize = 30.0f;
    float m_NearPlane = 0.1f;
    float m_FarPlane = 120.0f;

public:
    ShadowPass(GLContext *ctx, ShaderManager *sm, ECSWorld *world);

    ~ShadowPass() override;

    void Setup() override;

    void Execute(const glm::mat4 &, const glm::mat4 &) override;

    void Cleanup() override;

    GLuint GetShadowMapArray() const;

    const std::vector<glm::mat4> &GetLightMatrices() const;

    const std::vector<int> &GetShadowMapIndices() const;

    int GetShadowMapSize() const;

    void SetShadowMapSize(int size);

    void SetOrthoSize(float size);

    void SetFarPlane(float farP);

private:
    void InitializeShadowMap(int count);

    glm::mat4 BuildLightSpaceMatrix(const glm::vec3 &lightDir) const;

    glm::mat4 BuildSpotLightMatrix(const LightComponent &light);
};