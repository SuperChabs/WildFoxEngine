module;

#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.SceneSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;
import WFE.Scene.Light;
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Texture.TextureManager;
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.Scene.Mesh;
import WFE.UI.ImGuiManager;
import WFE.Resource.Model.ModelManager;
import WFE.Scene.Serializer.SceneFileHandler;
import WFE.Scene.Serializer.SerializerRegistry;
import WFE.Scene.Serializer.SceneMetadataSerializer;
import WFE.Scene.Serializer.HierarchyDeserializer;
import WFE.Scene.Serializer.Component;

using json = nlohmann::json;

export class SceneSerializer
{
private:
    ECSWorld* world;
    SceneFileHandler fileHandler;
    SerializerRegistry registry;

public:
    explicit SceneSerializer(ECSWorld* w)
        : world(w), fileHandler("../saves/")
    {
    }

    void SetSavesDirectory(const std::string& directory)
    {
        fileHandler.SetSavesDirectory(directory);
    }

    std::string GetSavesDirectory() const
    {
        return fileHandler.GetSavesDirectory();
    }

    bool SaveScene(const std::string& filename)
    {
        return SaveScene(filename, true);
    }

    bool SaveScene(const std::string& filename, bool pretty)
    {
        json sceneData;
        SceneMetadataSerializer metaSerializer(fileHandler);

        entt::entity mainCam = world->FindEditorCamera();
        sceneData["scene"]["metadata"] = metaSerializer.SerializeMetadata(filename, mainCam);

        SerializeEntities(sceneData);
        return WriteSceneToFile(filename, sceneData, pretty);
    }

    bool LoadScene(const std::string& filename,
                   MaterialManager* materialManager,
                   TextureManager* textureManager,
                   ModelManager* modelManager)
    {
        json sceneData = fileHandler.ReadScene(filename);

        if (sceneData.is_null() || !sceneData.contains("scene"))
        {
            Logger::Log(LogLevel::ERROR, "Invalid scene data");
            return false;
        }

        world->Clear();
        std::unordered_map<uint64_t, entt::entity> createdEntities;

        int loadedCount = 0;

        loadedCount = DeserializeEntities(sceneData, modelManager, createdEntities);

        SetupHierarchies(sceneData, createdEntities);

        DeserializeMaterials(sceneData, materialManager, createdEntities);

        ApplyColors(sceneData, createdEntities);

        Logger::Log(LogLevel::INFO,
            "Scene loaded: " + filename + " (" + std::to_string(loadedCount) + " entities)");

        return true;
    }

    std::vector<std::string> GetAvailableScenes()
    {
        return fileHandler.GetAvailableScenes();
    }

    bool DeleteScene(const std::string& filename)
    {
        return fileHandler.DeleteScene(filename);
    }

private:
    void SerializeEntities(json& sceneData)
    {
        sceneData["scene"]["entities"] = json::array();

        world->Each<IDComponent, TagComponent>(
            [&](entt::entity entity, IDComponent& id, TagComponent& tag)
        {
            json entityData;
            entityData["_id"] = id.id;
            entityData["_name"] = tag.name;

            registry.SerializeAllComponents(world, entity);
            json componentData = registry.SerializeAllComponents(world, entity);
            entityData.merge_patch(componentData);

            SerializeHierarchyParent(world, entity, entityData);

            sceneData["scene"]["entities"].push_back(entityData);
        });
    }

    void SerializeHierarchyParent(ECSWorld* w, entt::entity entity, json& entityData)
    {
        if (!w->HasComponent<HierarchyComponent>(entity))
            return;

        auto& h = w->GetComponent<HierarchyComponent>(entity);
        if (h.parent != entt::null)
        {
            entityData["_parentId"] = w->GetComponent<IDComponent>(h.parent).id;
        }
    }

    bool WriteSceneToFile(const std::string& filename, const json& sceneData, bool pretty)
    {
        bool success = fileHandler.WriteScene(filename, sceneData, pretty);

        if (success)
        {
            Logger::Log(LogLevel::INFO, "Scene saved: " + filename +
                        " (" + std::to_string(sceneData["scene"]["entities"].size()) + " entities)");
        }

        return success;
    }

    int DeserializeEntities(const json& sceneData,
                            ModelManager* modelManager,
                            std::unordered_map<uint64_t, entt::entity>& createdEntities)
    {
        int loadedCount = 0;

        for (auto& entityData : sceneData["scene"]["entities"])
        {
            std::string entityName = entityData.value("_name", "Entity");
            auto entity = world->CreateEntity(entityName);
            uint64_t uuid = entityData["_id"];

            if (createdEntities.count(uuid))
            {
                entity = createdEntities[uuid]; 
            }
            else
            {
                entity = world->CreateEntity(entityName);
                createdEntities[uuid] = entity;
            }

            bool isModelChild = entityData.value("modelChild", false);
            if (isModelChild)
            {
                registry.DeserializeAllComponents(world, entity, entityData);
                loadedCount++;
                continue;
            }

            bool meshHandled = false;
            if (entityData.contains("mesh"))
            {
                meshHandled = ModelSerializer::HandleModelLoading(
                    world, entity, entityData, modelManager, createdEntities);
            }

            if (!meshHandled)
            {
                registry.DeserializeAllComponents(world, entity, entityData);
            }
            else
            {
                uint64_t uuid = entityData["_id"];
                auto it = createdEntities.find(uuid);
                if (it == createdEntities.end())
                {
                    loadedCount++;
                    continue;
                }
                entity = it->second;

                json otherComponents = entityData;
                otherComponents.erase("mesh");
                registry.DeserializeAllComponents(world, entity, otherComponents);
            }

            if (!world->HasComponent<VisibilityComponent>(entity))
            {
                world->AddComponent<VisibilityComponent>(entity, true);
            }

            loadedCount++;
        }

        return loadedCount;
    }

    void SetupHierarchies(const json& sceneData,
                          std::unordered_map<uint64_t, entt::entity>& createdEntities)
    {
        HierarchyDeserializer hierarchyDeserializer(world, createdEntities);
        hierarchyDeserializer.SetupHierarchy(sceneData);
    }

    void DeserializeMaterials(const json& sceneData,
                              MaterialManager* materialManager,
                              std::unordered_map<uint64_t, entt::entity>& createdEntities)
    {
        MaterialSerializer::DeserializeMaterials(sceneData, materialManager, world, createdEntities);
    }

    void ApplyColors(const json& sceneData,
                     std::unordered_map<uint64_t, entt::entity>& createdEntities)
    {
        for (const auto& entityData : sceneData["scene"]["entities"])
        {
            if (!entityData.contains("color"))
                continue;

            uint64_t uuid = entityData["_id"];
            auto it = createdEntities.find(uuid);
            if (it == createdEntities.end())
                continue;

            entt::entity entity = it->second;

            if (!world->HasComponent<MaterialComponent>(entity))
            {
                auto& col = entityData["color"];
                glm::vec3 color = {col[0], col[1], col[2]};
                world->AddComponent<ColorComponent>(entity, color);
            }
        }
    }
};
