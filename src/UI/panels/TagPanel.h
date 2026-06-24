#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <memory>
#include <vector>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class TagPanel {
public:
    void Render(ECSWorld *ecs, entt::entity entity);
};