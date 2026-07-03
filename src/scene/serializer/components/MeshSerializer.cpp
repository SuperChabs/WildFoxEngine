#include "MeshSerializer.h"

json MeshSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    json meshData;

    if (world->HasComponent<ModelComponent>(entity)) {
        auto &m = world->GetComponent<ModelComponent>(entity);
        meshData["type"] = "model";
        meshData["path"] = m.filePath;
        if (world->HasComponent<TagComponent>(entity)) {
            meshData["_name"] = world->GetComponent<TagComponent>(entity).name;
        }
        return meshData;
    }

    return json::object();
}

entt::entity MeshSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    if (data.is_null() || data.empty())
        return entity;

    std::string meshType = data.value("type", "primitive");

    if (meshType == "model") {
        std::string modelPath = data.value("path", "");
        if (modelPath.empty() || !dcx.modelManager)
            return entity;

        entt::entity modelRoot = dcx.modelManager->LoadWithECS(modelPath, dcx.world);

        if (modelRoot == entt::null)
            return entity;

        std::string originalName = data.value("_name", "");
        if (!originalName.empty()) {
            if (dcx.world->HasComponent<TagComponent>(modelRoot)) {
                dcx.world->GetComponent<TagComponent>(modelRoot).name = originalName;
            } else {
                dcx.world->AddComponent<TagComponent>(modelRoot, originalName);
            }
        }

        dcx.world->DestroyEntity(entity);

        return modelRoot;
    }

    return entity;
}

bool MeshSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<MeshComponent>(entity) ||
           world->HasComponent<ModelComponent>(entity);
}