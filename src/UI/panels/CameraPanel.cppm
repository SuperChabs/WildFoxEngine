module;

#include <imgui.h>
#include <entt/entt.hpp>

export module WFE.UI.Panels.CameraPanel;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class CameraPanel
{
public:
    void Render(ECSWorld* ecs, entt::entity entity)
    {
        if (!ecs->HasComponent<CameraComponent>(entity)) return;
        if (!ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) return;
        
        auto& camera = ecs->GetComponent<CameraComponent>(entity);
        
        ImGui::Checkbox("Is Main Camera", &camera.isMainCamera);
        ImGui::Checkbox("Active", &camera.isActive);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Projection");
        ImGui::SliderFloat("FOV##camera", &camera.fov, 1.0f, 120.0f);
        ImGui::DragFloat("Near Plane##camera", &camera.nearPlane, 0.01f, 0.001f, 10.0f);
        ImGui::DragFloat("Far Plane##camera", &camera.farPlane, 1.0f, 10.0f, 10000.0f);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Movement");
        ImGui::DragFloat("Speed##camera", &camera.movementSpeed, 0.1f, 0.1f, 50.0f);
        ImGui::SliderFloat("Sensitivity##camera", &camera.mouseSensitivity, 0.01f, 1.0f);
        
        if (ecs->HasComponent<CameraOrientationComponent>(entity))
        {
            auto& orientation = ecs->GetComponent<CameraOrientationComponent>(entity);
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Text("Orientation");
            ImGui::SliderFloat("Yaw##camera", &orientation.yaw, -360.0f, 360.0f);
            ImGui::SliderFloat("Pitch##camera", &orientation.pitch, -89.0f, 89.0f);
        }
    }
};