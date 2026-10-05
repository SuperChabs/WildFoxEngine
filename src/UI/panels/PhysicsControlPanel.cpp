#include "PhysicsControlPanel.h"

void PhysicsControlPanel::Render(PhysicsModule *physicsModule, PhysicsDebugRenderSystem *debugSystem) {
    if (!physicsModule) return;

    Physics *physics = physicsModule->GetPhysics();

    // --- Playback controls ---
    bool paused = physicsModule->IsPaused();
    if (ImGui::Checkbox("Paused", &paused))
        physicsModule->SetPaused(paused);

    ImGui::SameLine();
    ImGui::BeginDisabled(!paused);
    if (ImGui::Button("Step"))
        physicsModule->RequestStep();
    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Gravity ---
    ImGui::Text("Gravity");
    glm::vec3 gravity = physics->GetGravity();
    if (ImGui::DragFloat3("##Gravity", &gravity[0], 0.1f))
        physics->SetGravity(gravity);

    if (ImGui::Button("Reset to Earth (-9.8)"))
        physics->SetGravity({0, -9.8f, 0});
    ImGui::SameLine();
    if (ImGui::Button("Zero-G"))
        physics->SetGravity({0, 0, 0});

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Substeps ---
    ImGui::Text("Substeps");
    int substeps = physicsModule->GetSubsteps();
    if (ImGui::SliderInt("##Substeps", &substeps, 1, 16))
        physicsModule->SetSubsteps(substeps);
    ImGui::TextDisabled("Higher values reduce tunneling at fast speeds");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Debug draw ---
    if (debugSystem) {
        ImGui::Checkbox("Draw Colliders", &debugSystem->m_enabled);
    }
}