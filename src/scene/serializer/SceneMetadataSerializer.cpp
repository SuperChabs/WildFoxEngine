#include "SceneMetadataSerializer.h"

#include "core/logging/Logger.h"
#include "ECS/components/Metadata.h"
#include "ECS/components/Camera.h"

SceneMetadataSerializer::SceneMetadataSerializer(SceneFileHandler &handler)
    : fileHandler(handler) {
}

json SceneMetadataSerializer::SerializeMetadata(ECSWorld *world, const std::string &sceneName, entt::entity mainCamera) {
    json metadata;
    metadata["name"] = sceneName;
    metadata["timestamp"] = SceneFileHandler::GetCurrentTimestamp();

    if (mainCamera != entt::null && world->HasComponent<c_ID>(mainCamera)) {
        metadata["mainCamera"] = world->GetComponent<c_ID>(mainCamera).id;
    }
    return metadata;
}

void SceneMetadataSerializer::DeserializeMetadata(const json &data, const std::unordered_map<uint64_t, entt::entity> &createdEntities,
        ECSWorld *world) {
    if (!data.contains("mainCamera")) return;

    const uint64_t camUUID = data["mainCamera"];
    const auto it = createdEntities.find(camUUID);
    if (it == createdEntities.end()) {
        Logger::Log(LogLevel::WARNING, "Main camera entity not found after load");
        return;
    }

    if (world->HasComponent<c_Camera>(it->second))
        world->GetComponent<c_Camera>(it->second).isMainCamera = true;
}