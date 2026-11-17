#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ECS/World.h"
#include "scene/serializer/DeserializeContext.h"

using json = nlohmann::json;

class IComponentSerializer {
public:
    virtual ~IComponentSerializer() = default;

    virtual json Serialize(ECSWorld *world, entt::entity entity) = 0;

    virtual entt::entity Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) = 0;

    virtual bool CanSerialize(ECSWorld *world, entt::entity entity) const = 0;
};