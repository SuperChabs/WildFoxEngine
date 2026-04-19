module;

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

export module WFE.Rendering.Passes.GeometryPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.ECS.ECSWorld;
import WFE.Core.CommandManager;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.Core.Logger;

export class GeometryPass : public RenderPass
{
private:
    ECSWorld* world;

    std::vector<glm::mat4> m_LightSpaceMatrices;
    std::vector<GLuint>    m_ShadowMapTextures;

    static constexpr int SHADOW_MAP_TEXTURE_SLOT = 8;

public:
    GeometryPass(GLContext* ctx, ShaderManager* sm, ECSWorld* w)
        : RenderPass("GeometryPass", ctx, sm)
        , world(w)
    {
        RegisterCommands();
    }

    void SetShadowData(const std::vector<glm::mat4> lightSpaceMatrices, std::vector<GLuint> shadowMapTextures)
    {
        m_LightSpaceMatrices  = lightSpaceMatrices;
        m_ShadowMapTextures   = shadowMapTextures;
    }

    void Setup() override
    {
        glDepthMask(GL_TRUE);
        context->SetDepthTest(true);
        context->SetDepthFunc(GL_LESS);
        context->SetBlend(false);
        context->SetCullFace(true);
        glCullFace(GL_BACK);
    }

    void Execute(const glm::mat4& view, const glm::mat4& projection) override
    {
        if (!enabled || !world) return;
        Setup();

        if (m_LightSpaceMatrices.empty() || m_ShadowMapTextures.empty())
        {
            CommandManager::ExecuteCommand("Renderer_RenderGeometry",
            {
                view,
                projection,
                std::string("basic"),
                glm::mat4(1.0f),
                0
            });
        }
        else
        {
            CommandManager::ExecuteCommand("Renderer_RenderGeometry",
            {
                view,
                projection,
                std::string("basic"),
                m_LightSpaceMatrices[0],
                static_cast<int>(m_ShadowMapTextures[0])
            });
        }

        CleanupShadowBinding();
    }

    void Cleanup() override {}

private:
    void CleanupShadowBinding()
    {
        glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_SLOT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RegisterCommands()
    {
        if (!CommandManager::HasCommand("Shadow_UpdateLightSpaceMatrix"))
        {
            CommandManager::RegisterCommand("Shadow_UpdateLightSpaceMatrices",
            [this](const CommandArgs& args)
            {
                if (args.size() > 1) return;

                auto lightSpaceMatrix = std::get<glm::mat4>(args[0]);

                m_LightSpaceMatrices.push_back(lightSpaceMatrix);
            });
        }
    }
};