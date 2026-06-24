#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class TransformPanel {
public:
    void Render(ECSWorld *ecs, entt::entity entity);
};