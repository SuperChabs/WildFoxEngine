#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class VisibilitySerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override;


    entt::entity Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) override;


    bool CanSerialize(ECSWorld *world, entt::entity entity) const override;
};