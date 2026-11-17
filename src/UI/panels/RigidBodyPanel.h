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
    static void RenderMass(c_RigidBody &rb);

    static void RenderVelocity(c_RigidBody &rb);

    static void RenderAngularVelocity(c_RigidBody &rb);

    static void RenderInertia(c_RigidBody &rb);

    static void RenderAccumulators(c_RigidBody &rb);
};