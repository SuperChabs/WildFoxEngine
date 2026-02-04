module;

#include <imgui.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

export module WFE.UI.Panels.TransformPanel;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class TransformPanel
{
public:
    void Render(ECSWorld* ecs, entt::entity entity)
    {
        if (!ecs->HasComponent<TransformComponent>(entity)) return;
        if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) return;
        
        auto& transform = ecs->GetComponent<TransformComponent>(entity);
        
        ImGui::Text("Position");
        ImGui::DragFloat3("##Pos", &transform.position[0], 0.1f);
        
        ImGui::Spacing();
        ImGui::Separator();
        
        ImGui::Text("Rotation");
        ImGui::DragFloat3("##Rot", &transform.rotation[0], 1.0f);
        
        ImGui::Spacing();
        ImGui::Separator();
        
        ImGui::Text("Scale");
        float uniformScale = transform.scale.x;
        if (ImGui::DragFloat("##Scale", &uniformScale, 0.01f, 0.01f, 10.0f))
            transform.scale = glm::vec3(uniformScale);
    }
};