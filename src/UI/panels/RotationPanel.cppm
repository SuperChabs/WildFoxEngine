module;

#include <imgui.h>
#include <entt.hpp>

export module WildFoxEngine.UI.Panels.RotationPanel;

import WildFoxEngine.ECS.ECSWorld;
import WildFoxEngine.ECS.Components;

export class RotationPanel
{
public:
    void Render(ECSWorld* ecs, entt::entity entity)
    {
        if (ecs->HasComponent<RotationComponent>(entity))
        {
            if (!ImGui::CollapsingHeader("Rotation Animation")) return;
            
            auto& rotation = ecs->GetComponent<RotationComponent>(entity);
            
            ImGui::Checkbox("Auto Rotate", &rotation.autoRotate);
            ImGui::DragFloat("Speed", &rotation.speed, 1.0f, 0.0f, 360.0f);
            ImGui::DragFloat3("Axis", &rotation.axis[0], 0.1f, -1.0f, 1.0f);
        }
        else
        {
            if (ImGui::Button("Add Rotation Component", ImVec2(-1, 0)))
                ecs->AddComponent<RotationComponent>(entity);
        }
    }
};