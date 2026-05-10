module;

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

export module WFE.Scene.Serializer.SceneMetadataSerializer;

import WFE.Scene.Serializer.SceneFileHandler;

using json = nlohmann::json;

export class SceneMetadataSerializer
{
private:
    SceneFileHandler& fileHandler;

public:
    explicit SceneMetadataSerializer(SceneFileHandler& handler)
        : fileHandler(handler)
    {
    }

    json SerializeMetadata(const std::string& sceneName, entt::entity mainCamera = entt::null)
    {
        json metadata;
        metadata["name"] = sceneName;
        metadata["timestamp"] = fileHandler.GetCurrentTimestamp();
        if (mainCamera != entt::null)
            metadata["mainCamera"] = static_cast<uint32_t>(mainCamera);
        return metadata;
    }

    void DeserializeMetadata(const json& data)
    {
    }
};
