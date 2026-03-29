module;

#include <ext/stdlib.hpp>
#include <ext/glm.hpp>
#include <ext/json.hpp>
#include <ext/entt.hpp>

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
            data["name"] = material.material->GetName();
        }
        
        return data;
    }

    void Deserialize(ECSWorld* world, 
                     entt::entity entity, 
                     const json& data) override
    {
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

            std::string materialName = matData.value("name", "");
            auto material = materialManager->GetMaterial(materialName);

            if (material)
            {
                world->AddComponent<MaterialComponent>(entity, material);
            }
            else
            {
                Logger::Log(LogLevel::WARNING,
                    "Material not found in manager: " + materialName);
            }
        }
    }
};