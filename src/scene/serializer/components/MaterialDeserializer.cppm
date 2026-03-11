module;

#include <unordered_map>
#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

export module WFE.Scene.Serializer.MaterialDeserializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Resource.Material.MaterialManager;
import WFE.Core.Logger;

using json = nlohmann::json;

export class MaterialDeserializer
{
private:
    MaterialManager* materialManager;
    ECSWorld* world;

public:
    MaterialDeserializer(MaterialManager* mm, ECSWorld* w)
        : materialManager(mm), world(w)
    {
    }

    void DeserializeMaterials(const json& sceneData, const std::unordered_map<uint64_t, entt::entity>& idMap)
    {
        if (!materialManager)
            return;

        for (const auto& entityData : sceneData["scene"]["entities"])
        {
            if (!entityData.contains("material"))
                continue;

            uint64_t uuid = entityData["_id"];
            auto it = idMap.find(uuid);
            if (it == idMap.end())
                continue;

            entt::entity entity = it->second;

            // Skip if already has material
            if (world->HasComponent<MaterialComponent>(entity))
                continue;

            auto& matData = entityData["material"];
            std::string materialName = matData.value("name", "default");
            auto material = materialManager->GetMaterial(materialName);

            if (material)
            {
                world->AddComponent<MaterialComponent>(entity, material);
            }
            else if (matData.contains("color"))
            {
                // Try to create from color if available
                auto& col = matData["color"];
                glm::vec3 color = {col[0], col[1], col[2]};
                auto newMat = materialManager->CreateColorMaterial(materialName, color);
                if (newMat)
                    world->AddComponent<MaterialComponent>(entity, newMat);
            }
        }
    }
};
