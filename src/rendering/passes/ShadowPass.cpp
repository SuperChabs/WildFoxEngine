#include "ShadowPass.h"
#include <cassert>
#include <entt/entt.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/logging/Logger.h"
#include "UI/UICameraController.h"

ShadowPass::ShadowPass(ShaderManager *sm, ECSWorld *world)
    : RenderPass("ShadowPass", sm), m_World(world) {
    InitializeShadowMap(MAX_DIR_SPOT_LIGHTS);
    InitializeCubeShadowMap(MAX_POINT_LIGHTS);
}

ShadowPass::~ShadowPass() {
    ShadowPass::Cleanup();
}

void ShadowPass::Setup() {}

void ShadowPass::Execute(FrameContext &ctx) {
    assert(m_LightSpaceMatrices.size() == MAX_DIR_SPOT_LIGHTS && "LightSpaceMatrices not initialized!");
    assert(m_ShadowFBOs.size() == MAX_DIR_SPOT_LIGHTS && "ShadowFBOs not initialized!");

    if (!m_enabled || !m_World)
        return;

    m_ShadowMapIndices.clear();
    m_PointShadowMapIndices.clear();

    std::vector<c_Light> shadowLights;
    std::vector<c_Light> pointShadowLights;
    std::vector<int> globalLightIndices;

    auto renderSceneDepth = [&](const std::string &shaderName) {
        m_World->Each<c_Transform, c_Mesh, c_Visibility>(
            [&](entt::entity entity, c_Transform &, c_Mesh &meshComp, c_Visibility &vis) {
                if (!vis.isActive || !vis.visible || !meshComp.mesh)
                    return;

                glm::mat4 model = m_World->GetGlobalTransform(entity);
                m_shaderManager->SetMat4(shaderName, "model", model);
                meshComp.mesh->DrawDepthOnly();
            }
        );
    };

    m_World->Each<c_Light, c_Transform>(
        [&](entt::entity e, c_Light &light, c_Transform) {
            if (!light.isActive)
                return;

            if (light.castShadows && light.type == LightType::POINT && shadowLights.size() < MAX_DIR_SPOT_LIGHTS) {
                m_PointShadowMapIndices[e] = static_cast<int>(pointShadowLights.size());
                pointShadowLights.push_back(light);
            } else if (light.castShadows && light.type != LightType::POINT && shadowLights.size() <
                       MAX_DIR_SPOT_LIGHTS) {
                m_ShadowMapIndices[e] = static_cast<int>(shadowLights.size());
                shadowLights.push_back(light);
            }
        });

    m_shaderManager->Bind("shadow_depth");
    glDisable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    const auto frustumCorners = GetFrustumCornersWS(ctx.view, ctx.projection, 0.0f, m_shadowDistance);

    for (int i = 0; i < shadowLights.size(); i++) {
        auto &light = shadowLights[i];

        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBOs[i]);
        glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightMatrix;

        if (light.type == LightType::DIRECTIONAL)
            lightMatrix = BuildLightSpaceMatrix(glm::normalize(light.direction), frustumCorners);
        else if (light.type == LightType::SPOT)
            lightMatrix = BuildSpotLightMatrix(light);
        else
            continue;

        m_LightSpaceMatrices[i] = lightMatrix;
        m_shaderManager->SetMat4("shadow_depth", "lightSpaceMatrix", lightMatrix);

        renderSceneDepth("shadow_depth");
    }

    m_shaderManager->Unbind();

    m_shaderManager->Bind("shadowCubeMapDepth");
    glBindFramebuffer(GL_FRAMEBUFFER, m_CubeShadowFBO);
    glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < pointShadowLights.size(); i++) {
        auto &light = pointShadowLights[i];

        m_shaderManager->SetInt("shadowCubeMapDepth", "layerOffset", i);

        auto matrices = BuildPointSpaceMatrices(light);
        for (int face = 0; face < 6; face++) {
            std::string name = "shadowMatrices[" + std::to_string(face) + "]";
            m_shaderManager->SetMat4("shadowCubeMapDepth", name, matrices[face]);
        }

        float far = std::max(light.radius, 100.0f);
        m_shaderManager->SetVec3("shadowCubeMapDepth", "lightPos", light.position);
        m_shaderManager->SetFloat("shadowCubeMapDepth", "far_plane", far);

        renderSceneDepth("shadowCubeMapDepth");
    }

    auto &out = ctx.shadows;
    out.lightMatrices  = &m_LightSpaceMatrices;
    out.dirSpotIndices = &m_ShadowMapIndices;
    out.cubeIndices    = &m_PointShadowMapIndices;
    out.cubeArray      =  m_CubeShadowMapArray;
    out.dirSpotArray   =  m_ShadowMapArray;

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, ctx.targetFBO);
    glViewport(0, 0, ctx.width, ctx.height);
}

void ShadowPass::Cleanup() {
    if (!m_ShadowFBOs.empty()) {
        glDeleteFramebuffers(static_cast<GLsizei>(m_ShadowFBOs.size()), m_ShadowFBOs.data());
        m_ShadowFBOs.clear();
    }

    if (m_ShadowMapArray != 0) {
        glDeleteTextures(1, &m_ShadowMapArray);
        m_ShadowMapArray = 0;
    }

    if (m_CubeShadowFBO != 0) {
        glDeleteFramebuffers(1, &m_CubeShadowFBO);
        m_CubeShadowFBO = 0;
    }

    if (m_CubeShadowMapArray != 0) {
        glDeleteTextures(1, &m_CubeShadowMapArray);
        m_CubeShadowMapArray = 0;
    }

    m_LightSpaceMatrices.clear();
    m_ShadowMapIndices.clear();
}

void ShadowPass::SetShadowMapSize(int size) {
    if (size == m_ShadowMapSize) return;
    m_ShadowMapSize = size;
    Cleanup();
    InitializeShadowMap(8);
}

void ShadowPass::InitializeShadowMap(int count) {
    m_ShadowFBOs.resize(count, 0);
    m_LightSpaceMatrices.resize(count, glm::mat4(1.0f));

    glGenFramebuffers(count, m_ShadowFBOs.data());

    glGenTextures(1, &m_ShadowMapArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowMapArray);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, m_ShadowMapSize, m_ShadowMapSize, count);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER  , GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER  , GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S      , GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T      , GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    for (int i = 0; i < count; i++) {
        // framebuffers initializing
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBOs[i]);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowMapArray, 0, i);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
            Logger::Log(LogLevel::ERROR, "ShadowPass: FBO incomplete — status " + std::to_string(status));
        else
            Logger::Log(LogLevel::INFO, "ShadowPass: shadow map FBO created (" + std::to_string(m_ShadowMapSize) + "px)");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowPass::InitializeCubeShadowMap(const int count) {
    glGenTextures(1, &m_CubeShadowMapArray);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_CubeShadowMapArray);
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32F, m_ShadowMapSize, m_ShadowMapSize, count * 6);

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glGenFramebuffers(1, &m_CubeShadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_CubeShadowFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_CubeShadowMapArray, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        Logger::Log(LogLevel::ERROR, "ShadowPass: Cube FBO incomplete — " + std::to_string(status));
    else
        Logger::Log(LogLevel::INFO, "ShadowPass: cube shadow map FBO created");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::array<glm::vec3, 8> ShadowPass::GetFrustumCornersWS(const glm::mat4 &view, const glm::mat4 &projection,
        float sliceNear, float sliceFar) {
    const float near = projection[3][2] / (projection[2][2] - 1.0f);
    const float far  = projection[3][2] / (projection[2][2] + 1.0f);

    const float tNear = glm::clamp((sliceNear - near) / (far - near), 0.0f, 1.0f);
    const float tFar  = glm::clamp((sliceFar  - near) / (far - near), 0.0f, 1.0f);

    const glm::mat4 invViewProj = glm::inverse(projection * view);

    std::array<glm::vec3, 8> corners{};
    int i = 0;
    for (const float x : {-1.0f, 1.0f})
        for (const float y : {-1.0f, 1.0f}) {
            glm::vec4 nearP = invViewProj * glm::vec4{x, y, -1.0f, 1.0f};
            glm::vec4 farP  = invViewProj * glm::vec4{x, y, 1.0f, 1.0f};
            const glm::vec3 nearWS = glm::vec3(nearP) / nearP.w;
            const glm::vec3 farWS  = glm::vec3(farP)  / farP.w;

            corners[i++] = nearWS + (farWS - nearWS) * tNear;
            corners[i++] = nearWS + (farWS - nearWS) * tFar;
        }
    return corners;
}

glm::mat4 ShadowPass::BuildLightSpaceMatrix(const glm::vec3 &lightDir, const std::array<glm::vec3, 8> &corners) const {
    glm::vec3 center{0.0f};
    for (const auto &c : corners) center += c;
    center /= corners.size();

    float radius = 0.0f;
    for (const auto &c : corners) radius = std::max(radius, glm::length(c - center));
    radius = std::ceil(radius * 16.0f) / 16.0f; // quantize so float noise doesn't change the texel size

    const glm::vec3 dir = glm::normalize(lightDir);
    const glm::vec3 up  = glm::abs(dir.y) > 0.99f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::mat4 rotView = glm::lookAt(glm::vec3{0.0f}, dir, up);

    glm::vec3 centerLS = rotView * glm::vec4{center, 1.0f};
    const float texel = 2.0f * radius / m_ShadowMapSize;
    centerLS.x = std::floor(centerLS.x / texel) * texel;
    centerLS.y = std::floor(centerLS.y / texel) * texel;

    const float zBack = radius + m_casterMargin;
    const glm::mat4 lightView = glm::translate(glm::mat4{1.0f}, {-centerLS.x, -centerLS.y, -centerLS.z -zBack}) * rotView;

    const glm::mat4 lightProjection = glm::ortho(-radius, radius, -radius, radius, 0.0f, zBack + radius);

    return lightProjection * lightView;
}

glm::mat4 ShadowPass::BuildSpotLightMatrix(const c_Light &light) {
    const float fov = glm::radians(light.outerCutoff) * 2.0f;

    const glm::mat4 proj = glm::perspective(
        fov,
        1.0f,
        1.0f,
        std::max(light.radius, 100.0f)
    );

    const glm::vec3 lightDir = glm::normalize(light.direction);
    auto up = glm::vec3(0, 1, 0);

    if (glm::abs(glm::dot(lightDir, up)) > 0.99f)
        up = glm::vec3(1, 0, 0);

    const glm::mat4 view = glm::lookAt(
        light.position,
        light.position + lightDir,
        up
    );

    return proj * view;
}

std::vector<glm::mat4> ShadowPass::BuildPointSpaceMatrices(const c_Light &light) {
    const float far      = std::max(light.radius, 100.0f);
    const glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, far);
    const glm::vec3 pos  = light.position;

    return {
        proj * glm::lookAt(pos, pos + glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0)),
        proj * glm::lookAt(pos, pos + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0)),
        proj * glm::lookAt(pos, pos + glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1)),
        proj * glm::lookAt(pos, pos + glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1)),
        proj * glm::lookAt(pos, pos + glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0)),
        proj * glm::lookAt(pos, pos + glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0))
    };
}