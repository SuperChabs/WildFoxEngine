#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class ScriptSerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override;


    void Deserialize(ECSWorld *world, entt::entity entity, const json &data) override;


    bool CanSerialize(ECSWorld *world, entt::entity entity) const override;
};