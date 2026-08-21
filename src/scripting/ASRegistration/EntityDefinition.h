#pragma once

#include <variant>
#include <vector>

#include "ECS/components/Components.h"

using ComponentDefinition = std::variant<
    TagComponent,
    TransformComponent,
    VisibilityComponent
>;

struct EntityDefinition {
    std::string name;
    std::vector<ComponentDefinition> components;
};