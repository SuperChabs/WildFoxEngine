module;

#include <entt/entt.hpp>

export module WFE.Rendering.Passes.IconPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Systems;
import WFE.ECS.Components;

export class IconPass : public RenderPass
{
    IconRenderSystem* m_icon;

public:
    IconPass(GLContext* ctx, ShaderManager* sm, IconRenderSystem* icon)
        : RenderPass("IconPass", ctx, sm), m_icon(icon) {}

    void Execute(ECSWorld& ecs, entt::entity camera, int w, int h) override
    {
        if (!m_icon || !enabled) return;

        auto& t   = ecs.GetComponent<TransformComponent>(camera);
        auto& ori = ecs.GetComponent<CameraOrientationComponent>(camera);
        auto& cam = ecs.GetComponent<CameraComponent>(camera);

        glm::mat4 view = ori.GetViewMatrix(t.position);
        glm::mat4 proj = cam.GetProjectionMatrix((float)w / (float)h);

        m_icon->Update(ecs, *shaderManager, "icon", view, proj);
    }
};