#pragma once

#include <variant>
#include <vector>

#include "ECS/components/Components.h"

using ComponentDefinition = std::variant<
    c_Tag,
    c_Transform,
    c_Visibility
>;

struct EntityDefinition {
    std::string name;
    std::vector<ComponentDefinition> components;
};