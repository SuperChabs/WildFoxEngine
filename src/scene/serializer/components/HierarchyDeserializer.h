#pragma once

#include <unordered_map>
#include <cstdint>

#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

#include "ECS/World.h"

using json = nlohmann::json;

class HierarchyDeserializer {
private:
    ECSWorld *world;
    const std::unordered_map<uint64_t, entt::entity> &idMap;

public:
    HierarchyDeserializer(ECSWorld *w, const std::unordered_map<uint64_t, entt::entity> &entityMap);


    void SetupHierarchy(const json &sceneData);
};