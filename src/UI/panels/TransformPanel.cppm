module;

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        if (ImGui::DragFloat3("##Rot", &transform.eulerHint[0], 1.0f))
            transform.rotation = glm::quat(glm::radians(transform.eulerHint));
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", &transform.scale[0], 0.1f);
    }
};