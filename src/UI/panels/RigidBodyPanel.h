#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <variant>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class RigidBodyPanel {
public:
    static void Render(ECSWorld *ecs, entt::entity entity);

private:
    static void RenderMass(RigidBodyComponent &rb);

    static void RenderVelocity(RigidBodyComponent &rb);

    static void RenderAngularVelocity(RigidBodyComponent &rb);

    static void RenderInertia(RigidBodyComponent &rb);

    static void RenderAccumulators(RigidBodyComponent &rb);
};