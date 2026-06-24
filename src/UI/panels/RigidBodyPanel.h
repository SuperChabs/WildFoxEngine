#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <variant>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class RigidBodyPanel {
public:
    void Render(ECSWorld *ecs, entt::entity entity);

private:
    void RenderMass(RigidBodyComponent &rb);

    void RenderVelocity(RigidBodyComponent &rb);

    void RenderAngularVelocity(RigidBodyComponent &rb);

    void RenderInertia(RigidBodyComponent &rb);

    void RenderAccumulators(RigidBodyComponent &rb);
};