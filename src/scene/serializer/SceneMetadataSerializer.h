#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "scene/serializer/SceneFileHandler.h"
#include "ECS/World.h"

using json = nlohmann::json;

class SceneMetadataSerializer {
private:
    SceneFileHandler &fileHandler;

public:
    explicit SceneMetadataSerializer(SceneFileHandler &handler);

    json SerializeMetadata(ECSWorld *world, const std::string &sceneName, entt::entity mainCamera = entt::null) const;

    static void DeserializeMetadata(
    const json &data,
    const std::unordered_map<uint64_t, entt::entity> &createdEntities,
    ECSWorld *world);
};