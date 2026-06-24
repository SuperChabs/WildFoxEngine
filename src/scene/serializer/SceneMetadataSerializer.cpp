#include "SceneMetadataSerializer.h"

SceneMetadataSerializer::SceneMetadataSerializer(SceneFileHandler &handler)
    : fileHandler(handler) {
}

json SceneMetadataSerializer::SerializeMetadata(const std::string &sceneName, entt::entity mainCamera) {
    json metadata;
    metadata["name"] = sceneName;
    metadata["timestamp"] = fileHandler.GetCurrentTimestamp();
    if (mainCamera != entt::null)
        metadata["mainCamera"] = static_cast<uint32_t>(mainCamera);
    return metadata;
}

void SceneMetadataSerializer::DeserializeMetadata(const json &data) {
}