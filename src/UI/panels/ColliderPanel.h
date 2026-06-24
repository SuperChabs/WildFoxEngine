#pragma once

#include <variant>
#include <string>

#include <imgui.h>
#include <glm/glm.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class ColliderPanel {
public:
    void Render(ECSWorld *ecs, entt::entity entity);
};