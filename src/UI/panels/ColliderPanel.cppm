module;

#include <variant>
#include <imgui.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

export module WFE.UI.Panels.ColliderPanel;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class ColliderPanel
{
public:
    void Render(ECSWorld* ecs, entt::entity entity)
    {
        if (!ecs->HasComponent<ColliderComponent>(entity)) return;
        if (!ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen)) return;

        auto& t = ecs->GetComponent<TransformComponent>(entity);
        auto& c = ecs->GetComponent<ColliderComponent>(entity);

        ImGui::Text("Hitbox");

        if (std::holds_alternative<AABB>(c.shape))
        {
            AABB& aabb = std::get<AABB>(c.shape);
            ImGui::DragFloat3("Min##aabb", &aabb.min[0], 0.01f);
            ImGui::DragFloat3("Max##aabb", &aabb.max[0], 0.01f);

            if (ImGui::Button("Set hitbox scale to entity scale"))
            {
                AABB aabb;
                aabb.min = -t.scale;
                aabb.max =  t.scale;
                c.shape = aabb;
            }
        }
    }
};
