#pragma once

#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "resource/material/MaterialManager.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class MaterialSerializer : public IComponentSerializer {
public:
    bool CanSerialize(ECSWorld *world, entt::entity entity) const override;

    json Serialize(ECSWorld *world, entt::entity entity) override;

    entt::entity Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) override;

private:
    static void ApplyMaterial(ECSWorld *ecs, entt::entity entity, const std::shared_ptr<Material> &material,
                              const json &data);
};