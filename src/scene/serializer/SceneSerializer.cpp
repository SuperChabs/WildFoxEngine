#include "SceneSerializer.h"

#include <glm/glm.hpp>

#include "core/logging/Logger.h"
#include "scene/serializer/SceneMetadataSerializer.h"
#include "scene/serializer/components/HierarchyDeserializer.h"

SceneSerializer::SceneSerializer(ECSWorld *w)
    : world(w), fileHandler("../saves/") {
}

bool SceneSerializer::SaveScene(const std::string &filename, MaterialManager *materialManager, bool pretty) {
    json sceneData;
    json metaData;
    SceneMetadataSerializer metaSerializer(fileHandler);

    entt::entity mainCam = entt::null;
    world->Each<c_Camera>([&](const entt::entity e, const c_Camera &cam) {
        if (cam.isMainCamera)
            mainCam = e;
    });
    metaData["metadata"] = metaSerializer.SerializeMetadata(world, filename, mainCam);

    sceneData["scene"]["entities"] = json::array();
    world->Each<c_ID, c_Tag>(
        [&](entt::entity entity, c_ID &id, c_Tag &tag) {
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

    const json materialsData = m_material.Serialize(materialManager);
    try {
        fileHandler.WriteFile(filename, sceneData,     FileType::ENTITY, pretty);
        fileHandler.WriteFile(filename, materialsData, FileType::MATERIAL, pretty);
        fileHandler.WriteFile(filename, metaData,      FileType::META, pretty);

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR, e.what());
        return false;
    }
}

bool SceneSerializer::LoadScene(const std::string &filename, MaterialManager *materialManager, ModelManager *modelManager) {
    json sceneData = fileHandler.ReadFile(filename, FileType::ENTITY);

    if (sceneData.is_null() || !sceneData.contains("scene")) {
        Logger::Log(LogLevel::ERROR, "Invalid scene data");
        return false;
    }

    const json materialsData = fileHandler.ReadFile(filename, FileType::MATERIAL);
    if (materialsData.is_null()) {
        Logger::Log(LogLevel::ERROR, "Invalid materials data");
        return false;
    }
    if (materialsData.is_array())
        m_material.Deserialize(materialManager, materialsData);

    const json metaData = fileHandler.ReadFile(filename, FileType::META);
    if (metaData.is_null()) {
        Logger::Log(LogLevel::ERROR, "Invalid meta data");
        return false;
    }

    world->Clear();
    std::unordered_map<uint64_t, entt::entity> createdEntities;

    const int loadedCount = DeserializeEntities(sceneData, modelManager, materialManager, createdEntities);

    SetupHierarchies(sceneData, createdEntities);

    // SceneMetadataSerializer::DeserializeMetadata(metaData["metadata"], createdEntities, world);

    Logger::Log(LogLevel::INFO, "Scene loaded: " + filename + " (" + std::to_string(loadedCount) + " entities)");

    return true;
}

void SceneSerializer::DeleteScene(const std::string &filename) const {
    fileHandler.DeleteFile(filename);
}

void SceneSerializer::SerializeHierarchyParent(ECSWorld *w, entt::entity entity, json &entityData) {
    if (!w->HasComponent<c_Hierarchy>(entity))
        return;

    auto &h = w->GetComponent<c_Hierarchy>(entity);
    if (h.parent != entt::null && w->HasComponent<c_ID>(h.parent)) {
        entityData["_parentId"] = w->GetComponent<c_ID>(h.parent).id;
    }
}

int SceneSerializer::DeserializeEntities(const json &sceneData, ModelManager *modelManager, MaterialManager *materialManager,
        std::unordered_map<uint64_t, entt::entity> &createdEntities) {
    int loadedCount = 0;

    const DeserializeContext ctx{world, modelManager, materialManager, &createdEntities};

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
                if (world->HasComponent<c_Tag>(entity)) {
                    world->GetComponent<c_Tag>(entity).name = originalName;
                } else {
                    world->AddComponent<c_Tag>(entity, originalName);
                }
            }
        }
        createdEntities[uuid] = entity;

        if (!world->HasComponent<c_Visibility>(entity))
            world->AddComponent<c_Visibility>(entity, true);

        if (world->HasComponent<c_Camera>(entity)) {
            auto &camera = world->GetComponent<c_Camera>(entity);
            auto &orientation = world->GetComponent<c_CameraOrientation>(entity);
            auto &transform = world->GetComponent<c_Transform>(entity);

            world->Each<c_Camera, c_Tag, c_CameraOrientation>(
                [&](const entt::entity e, c_Camera &cam, const c_Tag &tag, c_CameraOrientation &orien) {
                    if (tag.name != "Game Camera") return;

                    auto &trans = world->GetComponent<c_Transform>(e);

                    // coping data from scene files and putting in existing game camera to not break my engine (pls)

                    // camera data
                    cam.fov = camera.fov;
                    cam.nearPlane = camera.nearPlane;
                    cam.farPlane = camera.farPlane;

                    orien.yaw = orientation.yaw;
                    orien.pitch = orientation.pitch;

                    // trans🏳️‍⚧️ data
                    trans.position = transform.position;
                    trans.rotation = transform.rotation;
                    // we dont need scale
            });

            world->DestroyEntity(entity);
        }

        loadedCount++;
    }

    return loadedCount;
}

void SceneSerializer::SetupHierarchies(const json &sceneData, std::unordered_map<uint64_t, entt::entity> &createdEntities) {
    HierarchyDeserializer hierarchyDeserializer(world, createdEntities);
    hierarchyDeserializer.SetupHierarchy(sceneData);
}

bool SceneSerializer::IsModelChild(ECSWorld *w, entt::entity entity) {
    if (!w->HasComponent<c_Hierarchy>(entity))
        return false;

    const auto &h = w->GetComponent<c_Hierarchy>(entity);
    if (h.parent == entt::null)
        return false;

    return w->HasComponent<c_Model>(h.parent);
}