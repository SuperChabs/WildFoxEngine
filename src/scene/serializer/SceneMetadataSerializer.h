#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "scene/serializer/SceneFileHandler.h"

using json = nlohmann::json;

class SceneMetadataSerializer {
private:
    SceneFileHandler &fileHandler;

public:
    explicit SceneMetadataSerializer(SceneFileHandler &handler);


    json SerializeMetadata(const std::string &sceneName, entt::entity mainCamera = entt::null);


    void DeserializeMetadata(const json &data);
};