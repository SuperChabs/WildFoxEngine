module;

#include <string>
#include <vector>
#include <cassert>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

export module WFE.Rendering.Passes.ShadowPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;
import WFE.Core.CommandManager;
import WFE.Scene.Light;

#define MAX_SHADOW_LIGHTS 8

export class ShadowPass : public RenderPass
{
private:
    std::vector<GLuint> m_ShadowFBOs;
    GLuint m_ShadowMapArray = 0;
    int m_ShadowMapSize  = 2048;

    ECSWorld* m_World = nullptr;

    std::vector<glm::mat4> m_LightSpaceMatrices;
    std::vector<int> m_ShadowMapIndices;  // maps global light index -> shadow map index

    float m_OrthoSize   = 30.0f;
    float m_NearPlane   = 0.1f;
    float m_FarPlane    = 120.0f;

public:
    ShadowPass(GLContext* ctx, ShaderManager* sm, ECSWorld* world)
        : RenderPass("ShadowPass", ctx, sm)
        , m_World(world)
    {
        InitializeShadowMap(MAX_SHADOW_LIGHTS);
    }

    ~ShadowPass() override
    {
        Cleanup();
    }

    void Execute(const glm::mat4& /*view*/, const glm::mat4& /*projection*/) override
    {
        assert(m_LightSpaceMatrices.size() == MAX_SHADOW_LIGHTS && "LightSpaceMatrices not initialized!");
        assert(m_ShadowFBOs.size()         == MAX_SHADOW_LIGHTS && "ShadowFBOs not initialized!");

        if (!enabled || !m_World)
            return;

        m_ShadowMapIndices.assign(8, -1);

        std::vector<LightComponent> shadowLights;
        std::vector<int> globalLightIndices;

        int globalIndex = 0;
        m_World->Each<LightComponent>(
            [&](entt::entity, LightComponent& light)
        {
            if (!light.isActive)
            {
                globalIndex++;
                return;
            }

            if (globalIndex < 8)
            {
                if (light.castShadows && light.type != LightType::POINT && shadowLights.size() < 8)
                {
                    int shadowIndex = static_cast<int>(shadowLights.size());
                    m_ShadowMapIndices[globalIndex] = shadowIndex;
                    shadowLights.push_back(light);
                    globalLightIndices.push_back(globalIndex);
                }
            }
            globalIndex++;
        });

        shaderManager->Bind("shadow_depth");
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        for (int i = 0; i < shadowLights.size(); i++)
        {
            auto& light = shadowLights[i];

            glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBOs[i]);
            glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
            glClear(GL_DEPTH_BUFFER_BIT);

            glm::mat4 lightMatrix;

            if (light.type == LightType::DIRECTIONAL)
                lightMatrix = BuildLightSpaceMatrix(glm::normalize(light.direction));
            else if (light.type == LightType::SPOT)
                lightMatrix = BuildSpotLightMatrix(light);
            else
                continue;

            m_LightSpaceMatrices[i] = lightMatrix;
            shaderManager->SetMat4("shadow_depth", "lightSpaceMatrix", lightMatrix);

            // draw shadows
            m_World->Each<TransformComponent, MeshComponent, VisibilityComponent>(
                [&](entt::entity entity,
                    TransformComponent& transform,
                    MeshComponent& meshComp,
                    VisibilityComponent& vis)
            {
                if (!vis.isActive || !vis.visible || !meshComp.mesh)
                    return;

                glm::mat4 model = m_World->GetGlobalTransform(entity);
                shaderManager->SetMat4("shadow_depth", "model", model);
                meshComp.mesh->DrawDepthOnly();
            });
        }

        shaderManager->Unbind();
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Cleanup() override
    {
        if (!m_ShadowFBOs.empty())
            glDeleteFramebuffers(static_cast<GLsizei>(m_ShadowFBOs.size()), m_ShadowFBOs.data());
        
        if (m_ShadowMapArray != 0)
        {
            glDeleteTextures(1, &m_ShadowMapArray);
            m_ShadowMapArray = 0;
        }

        m_ShadowFBOs.clear();
        m_LightSpaceMatrices.clear();
        m_ShadowMapIndices.clear();
    }

    GLuint GetShadowMapArray() const { return m_ShadowMapArray; }
    const std::vector<glm::mat4>& GetLightMatrices() const { return m_LightSpaceMatrices; }
    const std::vector<int>& GetShadowMapIndices() const { return m_ShadowMapIndices; }
    int GetShadowMapSize()            const { return m_ShadowMapSize; }

    void SetShadowMapSize(int size)
    {
        if (size == m_ShadowMapSize) return;
        m_ShadowMapSize = size;
        Cleanup();
        InitializeShadowMap(8);
    }

    void SetOrthoSize(float size)   { m_OrthoSize = size; }
    void SetFarPlane(float farP)    { m_FarPlane  = farP; }

private:
    void InitializeShadowMap(int count)
    {
        m_ShadowFBOs.resize(count, 0);
        m_LightSpaceMatrices.resize(count, glm::mat4(1.0f));

        glGenFramebuffers(count, m_ShadowFBOs.data());

        glGenTextures(1, &m_ShadowMapArray);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowMapArray);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F,
                    m_ShadowMapSize, m_ShadowMapSize, count);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

        for (int i = 0; i < count; i++)
        {
            // framebuffers initializating
            glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBOs[i]);
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                m_ShadowMapArray, 0, i);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
                Logger::Log(LogLevel::ERROR,
                    "ShadowPass: FBO incomplete — status " + std::to_string(status));
            else
                Logger::Log(LogLevel::INFO,
                    "ShadowPass: shadow map FBO created (" +
                    std::to_string(m_ShadowMapSize) + "px)");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::mat4 BuildLightSpaceMatrix(const glm::vec3& lightDir) const
    {
        glm::vec3 sceneCenter{0.0f, 0.0f, 0.0f};
        glm::vec3 lightPos = sceneCenter - lightDir * (m_FarPlane * 0.5f);

        glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));

        if (glm::abs(glm::dot(lightDir, glm::vec3(0.0f, 1.0f, 0.0f))) > 0.99f)
            lightView = glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 lightProjection = glm::ortho(
            -m_OrthoSize,  m_OrthoSize,
            -m_OrthoSize,  m_OrthoSize,
             m_NearPlane,  m_FarPlane);

        return lightProjection * lightView;
    }

    glm::mat4 BuildSpotLightMatrix(const LightComponent& light)
    {
        float fov = light.outerCutoff * 2.0f;
        
        glm::mat4 proj = glm::perspective(
            glm::radians(fov),
            1.0f,
            0.1f,
            std::max(light.radius, 100.0f)
        );

        glm::vec3 lightDir = glm::normalize(light.direction);
        glm::vec3 up = glm::vec3(0, 1, 0);
        
        if (glm::abs(glm::dot(lightDir, up)) > 0.99f)
            up = glm::vec3(1, 0, 0);

        glm::mat4 view = glm::lookAt(
            light.position,
            light.position + lightDir,
            up
        );

        return proj * view;
    }
};