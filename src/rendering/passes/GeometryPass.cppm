module;

#include "ext/stdlib.hpp"
#include "ext/glm.hpp"
#include "ext/gl.hpp"

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

    glm::mat4 m_LightSpaceMatrix{1.0f};
    GLuint    m_ShadowMapTexture = 0;

    static constexpr int SHADOW_MAP_TEXTURE_SLOT = 8;

public:
    GeometryPass(GLContext* ctx, ShaderManager* sm, ECSWorld* w)
        : RenderPass("GeometryPass", ctx, sm)
        , world(w)
    {
        RegisterCommands();
    }

    void SetShadowData(const glm::mat4& lightSpaceMatrix, GLuint shadowMapTexture)
    {
        m_LightSpaceMatrix  = lightSpaceMatrix;
        m_ShadowMapTexture  = shadowMapTexture;
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

        CommandManager::ExecuteCommand("Renderer_RenderGeometry",
        {
            view,
            projection,
            std::string("basic"),
            m_LightSpaceMatrix,
            static_cast<int>(m_ShadowMapTexture)
        });

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
            CommandManager::RegisterCommand("Shadow_UpdateLightSpaceMatrix",
            [this](const CommandArgs& args)
            {
                if (args.size() < 2) return;
                m_LightSpaceMatrix = std::get<glm::mat4>(args[0]);
            });
        }
    }
};