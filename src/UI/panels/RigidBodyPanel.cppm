module;

#include <variant>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

export module WFE.UI.Panels.RigidBodyPanel;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class RigidBodyPanel
{
public:
    void Render(ECSWorld* ecs, entt::entity entity)
    {
        if (!ecs->HasComponent<RigidBodyComponent>(entity)) return;
        if (!ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) return;

        auto& rb = ecs->GetComponent<RigidBodyComponent>(entity);
        auto& t = ecs->GetComponent<TransformComponent>(entity);

        RenderMass(rb);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        RenderVelocity(rb);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        RenderAngularVelocity(rb);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        RenderInertia(rb);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        RenderAccumulators(rb);
    }

private:
    void RenderMass(RigidBodyComponent& rb)
    {
        ImGui::Text("Mass");

        float mass = (rb.inv_mass > 0.0f) ? (1.0f / rb.inv_mass) : 0.0f;
        bool isStatic = (rb.inv_mass == 0.0f);

        if (ImGui::Checkbox("Static (infinite mass)", &isStatic))
            rb.inv_mass = isStatic ? 0.0f : 1.0f;

        if (!isStatic)
        {
            ImGui::Text("Mass (kg)");
            if (ImGui::DragFloat("##Mass", &mass, 0.1f, 0.01f, 1000.0f))
                rb.inv_mass = 1.0f / mass;
        }
    }

    void RenderVelocity(RigidBodyComponent& rb)
    {
        ImGui::Text("Linear Velocity");
        ImGui::DragFloat3("##Velocity", &rb.velocity[0], 0.01f);

        ImGui::Spacing();

        float speed = glm::length(rb.velocity);
        ImGui::Text("Speed: %.3f m/s", speed);

        if (ImGui::Button("Reset Velocity"))
            rb.velocity = glm::vec3(0.0f);
    }

    void RenderAngularVelocity(RigidBodyComponent& rb)
    {
        ImGui::Text("Angular Velocity");
        ImGui::DragFloat3("##AngularVelocity", &rb.angular_velocity[0], 0.01f);

        ImGui::Spacing();

        if (ImGui::Button("Reset Angular Velocity"))
            rb.angular_velocity = glm::vec3(0.0f);
    }

    void RenderInertia(RigidBodyComponent& rb)
    {
        ImGui::Text("Inertia Tensor (diagonal)");
        ImGui::DragFloat3("##Inertia", &rb.inertia[0], 0.01f, 0.001f, 1000.0f);
    }

    void RenderAccumulators(RigidBodyComponent& rb)
    {
        ImGui::Text("Accumulated Force");
        ImGui::Text("  X: %.4f  Y: %.4f  Z: %.4f",
            rb.force_accum.x, rb.force_accum.y, rb.force_accum.z);

        ImGui::Spacing();

        ImGui::Text("Accumulated Torque");
        ImGui::Text("  X: %.4f  Y: %.4f  Z: %.4f",
            rb.torque_accum.x, rb.torque_accum.y, rb.torque_accum.z);

        ImGui::Spacing();

        if (ImGui::Button("Clear Accumulators"))
        {
            rb.force_accum  = glm::vec3(0.0f);
            rb.torque_accum = glm::vec3(0.0f);
        }
    }
};
