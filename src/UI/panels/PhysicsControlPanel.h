#pragma once

#include <imgui.h>

#include "physics/PhysicsModule.h"
#include "ECS/systems/PhysicsDebugRenderSystem.h"

class PhysicsControlPanel {
    bool m_showOpenJointDialog = false;
    entt::entity m_selectedFirst = entt::null;
    entt::entity m_selectedSecond = entt::null;
    JointType m_selectedType = JointType::NONE;

public:
    void Render(ECSWorld *world, PhysicsModule *physicsModule, PhysicsDebugRenderSystem *debugSystem);

private:
    void RenderOnCreateJoint(ECSWorld *world);

    static void RenderEntityNode(ECSWorld *world, entt::entity &e, entt::entity &selected);
    void RenderJointTypePopUp();
};