#include "SceneSerializer.h"

#include <glm/glm.hpp>

#include "core/logging/Logger.h"
#include "scene/serializer/SceneMetadataSerializer.h"
#include "scene/serializer/components/HierarchyDeserializer.h"

SceneSerializer::SceneSerializer(ECSWorld *w)
    : world(w), fileHandler("../saves/") {
}

void SceneSerializer::SetSavesDirectory(const std::string &directory) {
    fileHandler.SetSavesDirectory(directory);
}

std::string SceneSerializer::GetSavesDirectory() const {
    return fileHandler.GetSavesDirectory();
}

bool SceneSerializer::SaveScene(const std::string &filename, MaterialManager *materialManager) {
    return SaveScene(filename, materialManager, true);
}

bool SceneSerializer::SaveScene(const std::string &filename, MaterialManager *materialManager, bool pretty) {
    json sceneData;
    SceneMetadataSerializer metaSerializer(fileHandler);

    entt::entity mainCam = entt::null;
    world->Each<CameraComponent>([&](entt::entity e, CameraComponent &cam) {
        if (cam.isMainCamera)
            mainCam = e;
    });
    sceneData["scene"]["metadata"] = metaSerializer.SerializeMetadata(world, filename, mainCam);
    SerializeEntities(sceneData);

    json materialsData = m_material.Serialize(materialManager);
    fileHandler.WriteMaterials(filename, materialsData, pretty);

    return WriteSceneToFile(filename, sceneData, pretty);
}

bool SceneSerializer::LoadScene(const std::string &filename,
                                MaterialManager *materialManager,
                                ModelManager *modelManager) {
    json sceneData = fileHandler.ReadScene(filename);

    if (sceneData.is_null() || !sceneData.contains("scene")) {
        Logger::Log(LogLevel::ERROR, "Invalid scene data");
        return false;
    }

    json materialsData = fileHandler.ReadMaterials(filename);
    if (materialsData.is_array())
        m_material.Deserialize(materialManager, materialsData);

    world->Clear();
    std::unordered_map<uint64_t, entt::entity> createdEntities;

    int loadedCount = DeserializeEntities(sceneData, modelManager, materialManager, createdEntities);

    SetupHierarchies(sceneData, createdEntities);

    SceneMetadataSerializer metaSerializer(fileHandler);
    metaSerializer.DeserializeMetadata(
        sceneData["scene"]["metadata"],
        createdEntities,
        world
    );

    ApplyColors(sceneData, createdEntities);

    Logger::Log(LogLevel::INFO,
                "Scene loaded: " + filename + " (" + std::to_string(loadedCount) + " entities)");

    world->Each<CameraComponent>([&](entt::entity e, CameraComponent &cam) {
        if (!world->HasComponent<CameraTypeComponent>(e)) {
            auto type = cam.isMainCamera
                ? CameraTypeComponent::Type::GAME
                : CameraTypeComponent::Type::EDITOR;
            world->AddComponent<CameraTypeComponent>(e, type);
        }
    });

    return true;
}

std::vector<std::string> SceneSerializer::GetAvailableScenes() {
    return fileHandler.GetAvailableScenes();
}

bool SceneSerializer::DeleteScene(const std::string &filename) {
    return fileHandler.DeleteScene(filename);
}

void SceneSerializer::SerializeEntities(json &sceneData) {
    sceneData["scene"]["entities"] = json::array();

    world->Each<IDComponent, TagComponent>(
        [&](entt::entity entity, IDComponent &id, TagComponent &tag) {
            if (IsModelChild(world, entity))
                return;

            json entityData;
            entityData["_id"] = id.id;
            entityData["_name"] = tag.name;

            json componentData = registry.SerializeAllComponents(world, entity);
            if (componentData.contains("mesh")) {
                componentData["mesh"]["_name"] = tag.name;
            }
            entityData.merge_patch(componentData);

            SerializeHierarchyParent(world, entity, entityData);

            sceneData["scene"]["entities"].push_back(entityData);
        });
}

void SceneSerializer::SerializeHierarchyParent(ECSWorld *w, entt::entity entity, json &entityData) {
    if (!w->HasComponent<HierarchyComponent>(entity))
        return;

    auto &h = w->GetComponent<HierarchyComponent>(entity);
    if (h.parent != entt::null && w->HasComponent<IDComponent>(h.parent)) {
        entityData["_parentId"] = w->GetComponent<IDComponent>(h.parent).id;
    }
}

bool SceneSerializer::WriteSceneToFile(const std::string &filename, const json &sceneData, bool pretty) {
    bool success = fileHandler.WriteScene(filename, sceneData, pretty);

    if (success) {
        Logger::Log(LogLevel::INFO, "Scene saved: " + filename +
                                    " (" + std::to_string(sceneData["scene"]["entities"].size()) + " entities)");
    }

    return success;
}

int SceneSerializer::DeserializeEntities(const json &sceneData,
                                         ModelManager *modelManager,
                                         MaterialManager *materialManager,
                                         std::unordered_map<uint64_t, entt::entity> &createdEntities) {
    int loadedCount = 0;

    DeserializeContext ctx{world, modelManager, materialManager, &createdEntities};

    for (const auto &entityData: sceneData["scene"]["entities"]) {
        uint64_t uuid = entityData["_id"];
        entt::entity entity = createdEntities.count(uuid)
            ? createdEntities[uuid]
            : world->CreateEntity(entityData.value("_name", "Entity"));
        createdEntities[uuid] = entity;

        entt::entity previousEntity = entity;
        entity = registry.DeserializeAllComponents(ctx, entity, entityData);
        if (entity != previousEntity && entity != entt::null && entityData.contains("_name")) {
            const std::string originalName = entityData.value("_name", "");
            if (!originalName.empty()) {
                if (world->HasComponent<TagComponent>(entity)) {
                    world->GetComponent<TagComponent>(entity).name = originalName;
                } else {
                    world->AddComponent<TagComponent>(entity, originalName);
                }
            }
        }
        createdEntities[uuid] = entity;

        if (!world->HasComponent<VisibilityComponent>(entity))
            world->AddComponent<VisibilityComponent>(entity, true);

        loadedCount++;
    }

    return loadedCount;
}

void SceneSerializer::SetupHierarchies(const json &sceneData,
                                       std::unordered_map<uint64_t, entt::entity> &createdEntities) {
    HierarchyDeserializer hierarchyDeserializer(world, createdEntities);
    hierarchyDeserializer.SetupHierarchy(sceneData);
}

void SceneSerializer::ApplyColors(const json &sceneData,
                                  std::unordered_map<uint64_t, entt::entity> &createdEntities) {
    for (const auto &entityData: sceneData["scene"]["entities"]) {
        if (!entityData.contains("color"))
            continue;

        uint64_t uuid = entityData["_id"];
        auto it = createdEntities.find(uuid);
        if (it == createdEntities.end())
            continue;

        entt::entity entity = it->second;

        if (!world->HasComponent<MaterialComponent>(entity)) {
            const auto &col = entityData["color"];
            glm::vec3 color = {col[0], col[1], col[2]};
            world->AddComponent<ColorComponent>(entity, color);
        }
    }
}

bool SceneSerializer::IsModelChild(ECSWorld *w, entt::entity entity) {
    if (!w->HasComponent<HierarchyComponent>(entity))
        return false;

    const auto &h = w->GetComponent<HierarchyComponent>(entity);
    if (h.parent == entt::null)
        return false;

    return w->HasComponent<ModelComponent>(h.parent);
}
