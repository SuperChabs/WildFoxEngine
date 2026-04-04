module;

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.ModelLoader;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;
import WFE.Resource.Model.ModelManager;

using json = nlohmann::json;

export class ModelLoader
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
            if (!modelPath.empty() && modelManager)
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

                // Apply transform to loaded model
                if (entityData.contains("transform") && world->HasComponent<TransformComponent>(modelRoot))
                {
                    auto& t = entityData["transform"];
                    glm::vec3 pos = {t["position"][0], t["position"][1], t["position"][2]};
                    glm::vec3 eulerRot(t["rotation"][0], t["rotation"][1], t["rotation"][2]);
                    glm::quat rot = glm::quat(glm::radians(eulerRot));
                    glm::vec3 scl = {t["scale"][0], t["scale"][1], t["scale"][2]};
                    world->GetComponent<TransformComponent>(modelRoot) = {pos, rot, scl};
                }

                // Apply script to model root if exists
                if (entityData.contains("script"))
                {
                    std::string scriptPath = entityData["script"].value("path", "");
                    if (!scriptPath.empty())
                    {
                        auto& s = world->AddComponent<ScriptComponent>(modelRoot);
                        s.scriptPath = scriptPath;
                    }
                }

                // Replace entity mapping with model root
                world->DestroyEntity(entity);
                uint64_t uuid = entityData["_id"];
                createdEntities[uuid] = modelRoot;

                return true;
            }
        }

        return false;
    }
};
