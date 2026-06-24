#include "ModelSerializer.h"
#include <string>
#include <glm/glm.hpp>
#include "core/logging/Logger.h"

bool ModelSerializer::HandleModelLoading(ECSWorld *world,
                                         entt::entity entity,
                                         const json &entityData,
                                         ModelManager *modelManager,
                                         std::unordered_map<uint64_t, entt::entity> &createdEntities) {
    if (!entityData.contains("mesh"))
        return false;

    std::string meshType = entityData["mesh"].value("type", "primitive");

    if (meshType != "model")
        return false;

    std::string modelPath = entityData["mesh"].value("path", "");
    if (modelPath.empty())
        return false;

    if (!modelManager) {
        Logger::Log(LogLevel::ERROR, "ModelManager is null, cannot load: " + modelPath);
        world->DestroyEntity(entity);
        uint64_t uuid = entityData["_id"];
        createdEntities.erase(uuid);
        return true;
    }

    entt::entity modelRoot = modelManager->LoadWithECS(modelPath, world);

    if (modelRoot == entt::null) {
        Logger::Log(LogLevel::WARNING, "Failed to load model: " + modelPath);
        world->DestroyEntity(entity);
        uint64_t uuid = entityData["_id"];
        createdEntities.erase(uuid);
        return true;
    }

    ApplyTransform(world, modelRoot, entityData);
    ApplyScript(world, modelRoot, entityData);
    UpdateEntityMapping(world, entity, modelRoot, entityData, createdEntities);

    return true;
}

void ModelSerializer::ApplyTransform(ECSWorld *world, entt::entity entity, const json &entityData) {
    if (!entityData.contains("transform"))
        return;
    if (!world->HasComponent<TransformComponent>(entity))
        return;

    const auto &t = entityData["transform"];
    glm::vec3 pos = {t["position"][0], t["position"][1], t["position"][2]};
    glm::vec3 eulerDeg(t["rotation"][0], t["rotation"][1], t["rotation"][2]);
    glm::quat rot = glm::quat(glm::radians(eulerDeg));
    glm::vec3 scl = {t["scale"][0], t["scale"][1], t["scale"][2]};
    world->GetComponent<TransformComponent>(entity) = {pos, rot, scl};
}

void ModelSerializer::ApplyScript(ECSWorld *world, entt::entity entity, const json &entityData) {
    if (!entityData.contains("script"))
        return;

    std::string scriptPath = entityData["script"].value("path", "");
    if (scriptPath.empty())
        return;

    if (!world->HasComponent<ScriptComponent>(entity)) {
        auto &s = world->AddComponent<ScriptComponent>(entity);
        s.scriptPath = scriptPath;
    } else {
        world->GetComponent<ScriptComponent>(entity).scriptPath = scriptPath;
    }
}

void ModelSerializer::UpdateEntityMapping(ECSWorld *world,
                                          entt::entity oldEntity,
                                          entt::entity newEntity,
                                          const json &entityData,
                                          std::unordered_map<uint64_t, entt::entity> &createdEntities) {
    world->DestroyEntity(oldEntity);
    uint64_t uuid = entityData["_id"];
    createdEntities[uuid] = newEntity;
}
