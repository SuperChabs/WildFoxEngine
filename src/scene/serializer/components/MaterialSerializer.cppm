module;

#include <string>
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

export module WFE.Scene.Serializer.Component.MaterialSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;
import WFE.Resource.Material.MaterialManager;
import WFE.Scene.Serializer.Component.IComponentSerializer;

using json = nlohmann::json;

export class MaterialSerializer : public IComponentSerializer
{
public:
    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<MaterialComponent>(entity);
    }

    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        json data;
        
        if (!CanSerialize(world, entity))
            return data;
        
        auto& material = world->GetComponent<MaterialComponent>(entity);
        if (material.material)
        {
            data["material"]["name"] = material.material->GetName();
        }
        
        return data;
    }

    void Deserialize(ECSWorld* world, 
                     entt::entity entity, 
                     const json& data) override
    {
        // This is handled by DeserializeMaterials static method
    }

    static void DeserializeMaterials(const json& sceneData,
                                     MaterialManager* materialManager,
                                     ECSWorld* world,
                                     std::unordered_map<uint64_t, entt::entity>& idMap)
    {
        if (!materialManager)
        {
            Logger::Log(LogLevel::WARNING, "MaterialManager is null");
            return;
        }

        for (const auto& entityData : sceneData["scene"]["entities"])
        {
            if (!entityData.contains("material"))
                continue;

            uint64_t uuid = entityData["_id"];
            auto it = idMap.find(uuid);
            if (it == idMap.end())
                continue;

            entt::entity entity = it->second;
            const auto& matData = entityData["material"];

            std::string materialName = matData.value("name", "default");
            auto material = materialManager->GetMaterial(materialName);

            if (material)
            {
                world->AddComponent<MaterialComponent>(entity, material);
            }
            else if (matData.contains("color"))
            {
                auto& col = matData["color"];
                glm::vec3 color = {col[0], col[1], col[2]};
                auto newMat = materialManager->CreateColorMaterial(materialName, color);

                if (newMat)
                {
                    world->AddComponent<MaterialComponent>(entity, newMat);
                }
            }
        }
    }
};