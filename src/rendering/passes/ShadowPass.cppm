module;

#include <string>
#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

export module WFE.Rendering.Passes.ShadowPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;
import WFE.Core.CommandManager;
import WFE.Scene.Light;

export class ShadowPass : public RenderPass
{
private:
    GLuint m_ShadowFBO      = 0;
    GLuint m_ShadowMap      = 0;
    int    m_ShadowMapSize  = 2048;

    ECSWorld* m_World = nullptr;

    glm::mat4 m_LightSpaceMatrix{1.0f};

    float m_OrthoSize   = 30.0f;
    float m_NearPlane   = 0.1f;
    float m_FarPlane    = 120.0f;

public:
    ShadowPass(GLContext* ctx, ShaderManager* sm, ECSWorld* world)
        : RenderPass("ShadowPass", ctx, sm)
        , m_World(world)
    {
        InitializeShadowMap();
        RegisterCommands();
    }

    ~ShadowPass() override
    {
        Cleanup();
    }


    void Setup() override
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBO);
        glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    void Execute(const glm::mat4& /*view*/, const glm::mat4& /*projection*/) override
    {
        if (!enabled || !m_World)
            return;

        glm::vec3 lightDir{0.0f, -1.0f, 0.0f};
        bool foundLight = false;

        m_World->Each<LightComponent>(
            [&](entt::entity, LightComponent& light)
        {
            if (!foundLight && light.isActive &&
                light.type == LightType::DIRECTIONAL && light.castShadows)
            {
                lightDir   = glm::normalize(light.direction);
                foundLight = true;
            }
        });

        if (!foundLight)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return;
        }

        m_LightSpaceMatrix = BuildLightSpaceMatrix(lightDir);

        Setup();

        shaderManager->Bind("shadow_depth");
        shaderManager->SetMat4("shadow_depth", "lightSpaceMatrix", m_LightSpaceMatrix);

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

        shaderManager->Unbind();
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        CommandManager::ExecuteCommand("Shadow_UpdateLightSpaceMatrix",
        {
            m_LightSpaceMatrix,
            std::string("shadow_depth")
        });
    }

    void Cleanup() override
    {
        if (m_ShadowFBO) { glDeleteFramebuffers(1, &m_ShadowFBO); m_ShadowFBO = 0; }
        if (m_ShadowMap) { glDeleteTextures(1,    &m_ShadowMap);  m_ShadowMap = 0; }
    }

    GLuint GetShadowMap()             const { return m_ShadowMap; }
    const glm::mat4& GetLightMatrix() const { return m_LightSpaceMatrix; }
    int GetShadowMapSize()            const { return m_ShadowMapSize; }

    void SetShadowMapSize(int size)
    {
        if (size == m_ShadowMapSize) return;
        m_ShadowMapSize = size;
        Cleanup();
        InitializeShadowMap();
    }

    void SetOrthoSize(float size)   { m_OrthoSize = size; }
    void SetFarPlane(float farP)    { m_FarPlane  = farP; }

private:
    void InitializeShadowMap()
    {
        glGenTextures(1, &m_ShadowMap);
        glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                     m_ShadowMapSize, m_ShadowMapSize,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glGenFramebuffers(1, &m_ShadowFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, m_ShadowMap, 0);
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

    void RegisterCommands()
    {
        if (!CommandManager::HasCommand("Shadow_GetShadowMap"))
        {
            CommandManager::RegisterCommand("Shadow_GetShadowMap",
            [this](const CommandArgs&)
            {
            });
        }
    }
};