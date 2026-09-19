#include "RigidBodyPanel.h"
#include <glm/glm.hpp>

void RigidBodyPanel::Render(ECSWorld *ecs, entt::entity entity) {
    if (!ecs->HasComponent<RigidBodyComponent>(entity)) return;
    if (!ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto &rb = ecs->GetComponent<RigidBodyComponent>(entity);
    auto &t = ecs->GetComponent<TransformComponent>(entity);

    RenderMass(rb);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Elasticity");
    ImGui::DragFloat("##Elasticity", &rb.m_elasticity, 0.01f, 0.0f, 1.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Friction");
    ImGui::DragFloat("##Friction", &rb.m_friction, 0.01f, 0.0f, 1.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    RenderVelocity(rb);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    RenderAngularVelocity(rb);

    // ImGui::Spacing();
    // ImGui::Separator();
    // ImGui::Spacing();
    //
    // RenderInertia(rb);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    RenderAccumulators(rb);
}

void RigidBodyPanel::RenderMass(RigidBodyComponent &rb) {
    ImGui::Text("Mass");

    float mass = (rb.m_invMass > 0.0f) ? (1.0f / rb.m_invMass) : 0.0f;
    bool isStatic = (rb.m_invMass == 0.0f);

    if (ImGui::Checkbox("Static (infinite mass)", &isStatic)) {
        rb.m_invMass = isStatic ? 0.0f : 1.0f;
        rb.m_linearVelocity = glm::vec3(0.0f);
        rb.m_angularVelocity = glm::vec3(0.0f);
        rb.m_forceAccum = glm::vec3(0.0f);
        rb.m_torqueAccum = glm::vec3(0.0f);
    }

    if (!isStatic) {
        ImGui::Text("Mass (kg)");
        if (ImGui::DragFloat("##Mass", &mass, 0.1f, 0.01f, 1000.0f))
            rb.m_invMass = 1.0f / mass;
    }
}

void RigidBodyPanel::RenderVelocity(RigidBodyComponent &rb) {
    ImGui::Text("Linear Velocity");
    ImGui::DragFloat3("##Velocity", &rb.m_linearVelocity[0], 0.01f);

    ImGui::Spacing();

    float speed = glm::length(rb.m_linearVelocity);
    ImGui::Text("Speed: %.3f m/s", speed);

    if (ImGui::Button("Reset Velocity"))
        rb.m_linearVelocity = glm::vec3(0.0f);
}

void RigidBodyPanel::RenderAngularVelocity(RigidBodyComponent &rb) {
    ImGui::Text("Angular Velocity");
    ImGui::DragFloat3("##AngularVelocity", &rb.m_angularVelocity[0], 0.01f);

    ImGui::Spacing();

    if (ImGui::Button("Reset Angular Velocity"))
        rb.m_angularVelocity = glm::vec3(0.0f);
}

void RigidBodyPanel::RenderInertia(RigidBodyComponent &rb) {
//     ImGui::Text("Inertia Tensor (diagonal)");
//     ImGui::DragFloat3("##Inertia", &rb.inertia[0], 0.01f, 0.001f, 1000.0f);
}

void RigidBodyPanel::RenderAccumulators(RigidBodyComponent &rb) {
    ImGui::Text("Accumulated Force");
    ImGui::Text("  X: %.4f  Y: %.4f  Z: %.4f",
                rb.m_forceAccum.x, rb.m_forceAccum.y, rb.m_forceAccum.z);

    ImGui::Spacing();

    ImGui::Text("Accumulated Torque");
    ImGui::Text("  X: %.4f  Y: %.4f  Z: %.4f",
                rb.m_torqueAccum.x, rb.m_torqueAccum.y, rb.m_torqueAccum.z);

    ImGui::Spacing();

    if (ImGui::Button("Clear Accumulators")) {
        rb.m_forceAccum = glm::vec3(0.0f);
        rb.m_torqueAccum = glm::vec3(0.0f);
    }
}