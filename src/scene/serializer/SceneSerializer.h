#pragma once

#include <string>

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/Light.h"
#include "resource/material/MaterialManager.h"
#include "resource/texture/TextureManager.h"
#include "rendering/primitive/PrimitivesFactory.h"
#include "scene/Mesh.h"
#include "UI/ImGuiManager.h"
#include "resource/model/ModelManager.h"

#include "scene/serializer/SceneFileHandler.h"
#include "scene/serializer/SerializerRegistry.h"
#include "scene/serializer/components/ComponentSerializers.h"
#include "scene/serializer/SceneMaterialSerializer.h"

using json = nlohmann::json;

class SceneSerializer {
private:
    ECSWorld *world;
    SceneFileHandler fileHandler;
    SerializerRegistry registry;
    SceneMaterialSerializer m_material;

public:
    explicit SceneSerializer(ECSWorld *w);

    void SetSavesDirectory(const std::string &directory);

    std::string GetSavesDirectory() const;

    bool SaveScene(const std::string &filename, MaterialManager *materialManager);

    bool SaveScene(const std::string &filename, MaterialManager *materialManager, bool pretty);

    bool LoadScene(const std::string &filename,
                   MaterialManager *materialManager,
                   TextureManager *textureManager,
                   ModelManager *modelManager);

    std::vector<std::string> GetAvailableScenes();

    bool DeleteScene(const std::string &filename);

private:
    void SerializeEntities(json &sceneData);

    void SerializeHierarchyParent(ECSWorld *w, entt::entity entity, json &entityData);

    bool WriteSceneToFile(const std::string &filename, const json &sceneData, bool pretty);

    int DeserializeEntities(const json &sceneData,
                            ModelManager *modelManager,
                            std::unordered_map<uint64_t, entt::entity> &createdEntities);

    void SetupHierarchies(const json &sceneData,
                          std::unordered_map<uint64_t, entt::entity> &createdEntities);

    void DeserializeMaterials(const json &sceneData,
                              MaterialManager *materialManager,
                              std::unordered_map<uint64_t, entt::entity> &createdEntities);

    void ApplyColors(const json &sceneData,
                     std::unordered_map<uint64_t, entt::entity> &createdEntities);

    bool IsModelChild(ECSWorld *w, entt::entity entity);
};