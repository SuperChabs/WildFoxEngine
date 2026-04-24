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
    std::vector<int> m_ShadowMapIndices;
    GLuint m_shadowMapArray;

    static constexpr int SHADOW_MAP_TEXTURE_SLOT = 8;

public:
    GeometryPass(GLContext* ctx, ShaderManager* sm, ECSWorld* w)
        : RenderPass("GeometryPass", ctx, sm)
        , world(w)
    {
    }

    void SetShadowData(const std::vector<glm::mat4>& lightSpaceMatrices, GLuint shadowMapArray,
                       const std::vector<int>& shadowMapIndices = {})
    {
        m_LightSpaceMatrices  = lightSpaceMatrices;
        m_shadowMapArray      = shadowMapArray;
        m_ShadowMapIndices    = shadowMapIndices;
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
            m_LightSpaceMatrices,
            static_cast<int>(m_shadowMapArray),
            m_ShadowMapIndices
        });
        

        CleanupShadowBinding();
    }

    void Cleanup() override {}

private:
    void CleanupShadowBinding()
    {
        glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_SLOT);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
};