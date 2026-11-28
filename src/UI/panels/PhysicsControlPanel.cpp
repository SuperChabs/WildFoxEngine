#include "PhysicsControlPanel.h"

#include "physics/dynamics/BodyTransform.h"

void PhysicsControlPanel::Render(ECSWorld *world, PhysicsModule *physicsModule, PhysicsDebugRenderSystem *debugSystem) {
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
    if (debugSystem)
        ImGui::Checkbox("Draw Colliders", &debugSystem->m_enabled);

    // --- Joint ---
    if (ImGui::Button("Create Joint")) {
        m_showOpenJointDialog = true;
        ImGui::OpenPopup("NewJoint");
    }

    RenderOnCreateJoint(world);
}

void PhysicsControlPanel::RenderOnCreateJoint(ECSWorld *world) {
    if (!ImGui::BeginPopupModal("NewJoint", &m_showOpenJointDialog, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    std::vector<entt::entity> roots;
    world->Each<c_RigidBody, c_Collider>(
        [&](const entt::entity e, c_RigidBody &, c_Collider &) {
            if (world->HasComponent<c_Hierarchy>(e))
                if (world->GetComponent<c_Hierarchy>(e).HasParent())
                    return;

            roots.push_back(e);
        }
    );

    ImGui::Text("Type");
    if (ImGui::Button(kJointTypeNames[(int)m_selectedType]))
        ImGui::OpenPopup("Select1");
    if (ImGui::BeginPopup("Select1")) {
        ImGui::SeparatorText("Type");

        RenderJointTypePopUp();

        ImGui::EndPopup();
    }

    ImGui::Text("First Entity");
    if (ImGui::Button("Select..##first"))
        ImGui::OpenPopup("Select2");
    if (ImGui::BeginPopup("Select2")) {
        ImGui::SeparatorText("Entities");
        for (auto e: roots)
            RenderEntityNode(world, e, m_selectedFirst);
        ImGui::EndPopup();
    }

    ImGui::Text("Second Entity");
    if (ImGui::Button("Select..##second"))
        ImGui::OpenPopup("Select3");
    if (ImGui::BeginPopup("Select3")) {
        ImGui::SeparatorText("Entities");
        for (auto e: roots)
            RenderEntityNode(world, e, m_selectedSecond);

        ImGui::EndPopup();
    }

    const bool valid = m_selectedFirst  != entt::null &&
                       m_selectedSecond != entt::null &&
                       m_selectedFirst  != m_selectedSecond;

    ImGui::BeginDisabled(!valid);
    if (ImGui::Button("Apply")) {

        BodyInfo a{
          &world->GetComponent<c_RigidBody>(m_selectedFirst),
          &world->GetComponent<c_Transform>(m_selectedFirst),
          &world->GetComponent<c_Collider>(m_selectedFirst)
        };
        BodyInfo b{
          &world->GetComponent<c_RigidBody>(m_selectedSecond),
          &world->GetComponent<c_Transform>(m_selectedSecond),
          &world->GetComponent<c_Collider>(m_selectedSecond)
        };

        const glm::vec3 worldAnchor = a.transform->position;

        c_Joint j;
        j.type = m_selectedType;
        j.bodyA = m_selectedFirst;
        j.bodyB = m_selectedSecond;
        j.localAnchorA = BodyTransform::WorldSpaceToBodySpace(a, worldAnchor);
        j.localAnchorB = BodyTransform::WorldSpaceToBodySpace(b, worldAnchor);

        const auto newJoint = world->CreateEntity("joint");
        world->AddComponent<c_Joint>(newJoint, j);

        m_selectedFirst = entt::null;
        m_selectedSecond = entt::null;

        m_showOpenJointDialog = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        m_showOpenJointDialog = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void PhysicsControlPanel::RenderEntityNode(ECSWorld *world, entt::entity &e, entt::entity &selected) {
    const auto &tag = world->GetComponent<c_Tag>(e);
    const bool hasChildren = world->HasComponent<c_Hierarchy>(e) && world->GetComponent<c_Hierarchy>(e).HasChildren();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (selected == e) flags |= ImGuiTreeNodeFlags_Selected;

    const bool open = ImGui::TreeNodeEx((void *) (intptr_t) e, flags, "%s", tag.name.c_str());

    if (ImGui::IsItemClicked()) {
        selected = e;
        ImGui::CloseCurrentPopup();
    }

    if (open && hasChildren) {
        world->ForEachChild(e, [&](entt::entity child) {
            RenderEntityNode(world, child, selected);
        });

        ImGui::TreePop();
    }
}

void PhysicsControlPanel::RenderJointTypePopUp() {
    for (int i = 0; i <= (int)JointType::HINGE_LIMITED; i++) {
        const bool selected = ((int)m_selectedType == i);
        if (ImGui::Selectable(kJointTypeNames[i], selected))
            m_selectedType = (JointType)i;

        if (selected)
            ImGui::SetItemDefaultFocus();
    }
}
