#pragma once

#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "resource/model/ModelManager.h"

using json = nlohmann::json;

class ModelSerializer {
public:
    static bool HandleModelLoading(ECSWorld *world,
                                   entt::entity entity,
                                   const json &entityData,
                                   ModelManager *modelManager,
                                   std::unordered_map<uint64_t, entt::entity> &createdEntities);

private:
    static void ApplyTransform(ECSWorld *world, entt::entity entity, const json &entityData);

    static void ApplyScript(ECSWorld *world, entt::entity entity, const json &entityData);

    static void UpdateEntityMapping(ECSWorld *world,
                                    entt::entity oldEntity,
                                    entt::entity newEntity,
                                    const json &entityData,
                                    std::unordered_map<uint64_t, entt::entity> &createdEntities);
};