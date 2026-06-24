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

    void Deserialize(ECSWorld *world,
                     entt::entity entity,
                     const json &data) override;

    static void DeserializeMaterials(const json &sceneData,
                                     MaterialManager *materialManager,
                                     ECSWorld *world,
                                     std::unordered_map<uint64_t, entt::entity> &idMap);
};