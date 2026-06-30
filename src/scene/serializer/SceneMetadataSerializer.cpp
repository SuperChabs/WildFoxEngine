#include "SceneMetadataSerializer.h"

#include "core/logging/Logger.h"
#include "ECS/components/Metadata.h"
#include "ECS/components/Camera.h"

SceneMetadataSerializer::SceneMetadataSerializer(SceneFileHandler &handler)
    : fileHandler(handler) {
}

json SceneMetadataSerializer::SerializeMetadata(ECSWorld *world, const std::string &sceneName, entt::entity mainCamera) const {
    json metadata;
    metadata["name"] = sceneName;
    metadata["timestamp"] = fileHandler.GetCurrentTimestamp();

    // Зберігати UUID з IDComponent, а не сирий entity handle
    if (mainCamera != entt::null && world->HasComponent<IDComponent>(mainCamera)) {
        metadata["mainCamera"] = world->GetComponent<IDComponent>(mainCamera).id;
    }
    return metadata;
}

void SceneMetadataSerializer::DeserializeMetadata(
    const json &data,
    const std::unordered_map<uint64_t, entt::entity> &createdEntities,
    ECSWorld *world)
{
    if (!data.contains("mainCamera")) return;

    uint64_t camUUID = data["mainCamera"];
    auto it = createdEntities.find(camUUID);
    if (it == createdEntities.end()) {
        Logger::Log(LogLevel::WARNING, "Main camera entity not found after load");
        return;
    }

    if (world->HasComponent<CameraComponent>(it->second)) {
        world->GetComponent<CameraComponent>(it->second).isMainCamera = true;
    }
}