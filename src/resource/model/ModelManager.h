#pragma once

#include <string>
#include <memory>
#include <vector>

#include <entt/entt.hpp>

#include "resource/model/Model.h"
#include "resource/model/ModelLoader.h"
#include "resource/material/MaterialManager.h"

#include "ECS/World.h"

class ModelManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Model> > loadedModels;
    std::string assetsPath = "../assets/objects/";
    MaterialManager *materialManager = nullptr;

public:
    ModelManager();

    ~ModelManager();

    void SetMaterialManager(MaterialManager *mm);

    std::shared_ptr<Model> Load(const std::string &filepath);

    entt::entity LoadWithECS(const std::string &filepath, ECSWorld *world, bool isBaseShape = false);

    std::shared_ptr<Model> Get(const std::string &filepath);

    void Unload(const std::string &filepath);

    void CleanupUnused();

    void UnloadAll();

    bool IsLoaded(const std::string &filepath) const;

    size_t GetLoadedCount() const;

    std::vector<std::string> GetLoadedModelNames() const;

    void SetAssetsPath(const std::string &path);

    const std::string &GetAssetsPath() const;
};