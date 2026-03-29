module;

#include <ext/stdlib.hpp>
#include <ext/json.hpp>
#include <ext/entt.hpp>
#include <ext/glm.hpp>

export module WFE.Scene.Serializer.Component.ModelSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;
import WFE.Resource.Model.ModelManager;

using json = nlohmann::json;

export class ModelSerializer
{
public:
    static bool HandleModelLoading(ECSWorld* world,
                                   entt::entity entity,
                                   const json& entityData,
                                   ModelManager* modelManager,
                                   std::unordered_map<uint64_t, entt::entity>& createdEntities)
    {
        if (!entityData.contains("mesh"))
            return false;

        std::string meshType = entityData["mesh"].value("type", "primitive");

        if (meshType == "model")
        {
            std::string modelPath = entityData["mesh"].value("path", "");
            if (!modelPath.empty())
            {
                entt::entity modelRoot = modelManager->LoadWithECS(modelPath, world);

                if (modelRoot == entt::null)
                {
                    Logger::Log(LogLevel::WARNING, "Failed to load model: " + modelPath);
                    world->DestroyEntity(entity);
                    uint64_t uuid = entityData["_id"];
                    createdEntities.erase(uuid);
                    return true;
                }

                UpdateEntityMapping(world, entity, modelRoot, entityData, createdEntities);

                return true;
            }
        }

        return false;
    }

private:
    static void ApplyTransform(ECSWorld* world, entt::entity entity, const json& entityData)
    {
        if (!entityData.contains("transform") || !world->HasComponent<TransformComponent>(entity))
            return;

        auto& t = entityData["transform"];
        glm::vec3 pos = {t["position"][0], t["position"][1], t["position"][2]};
        glm::vec3 eulerRot(t["rotation"][0], t["rotation"][1], t["rotation"][2]);
        glm::quat rot = glm::quat(glm::radians(eulerRot));
        glm::vec3 scl = {t["scale"][0], t["scale"][1], t["scale"][2]};
        world->GetComponent<TransformComponent>(entity) = {pos, rot, scl};
    }

    static void ApplyScript(ECSWorld* world, entt::entity entity, const json& entityData)
    {
        if (!entityData.contains("script"))
            return;

        std::string scriptPath = entityData["script"].value("path", "");
        if (!scriptPath.empty())
        {
            auto& s = world->AddComponent<ScriptComponent>(entity);
            s.scriptPath = scriptPath;
        }
    }

    static void UpdateEntityMapping(ECSWorld* world,
                                    entt::entity oldEntity,
                                    entt::entity newEntity,
                                    const json& entityData,
                                    std::unordered_map<uint64_t, entt::entity>& createdEntities)
    {
        world->DestroyEntity(oldEntity);
        uint64_t uuid = entityData["_id"];
        createdEntities[uuid] = newEntity;
    }
};