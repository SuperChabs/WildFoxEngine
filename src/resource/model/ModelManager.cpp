#include "ModelManager.h"

#include "core/logging/Logger.h"
#include "core/CommandManager.h"

ModelManager::ModelManager() {
    CommandManager::RegisterCommand("onModelManagerCacheCleaning",
                                    [this](const CommandArgs &) {
                                        UnloadAll();
                                    });
}

ModelManager::~ModelManager() {
    UnloadAll();
}

void ModelManager::SetMaterialManager(MaterialManager *mm) {
    materialManager = mm;
}

std::shared_ptr<Model> ModelManager::Load(const std::string &filepath) {
    auto it = loadedModels.find(filepath);
    if (it != loadedModels.end()) {
        Logger::Log(LogLevel::INFO,
                    "Model already loaded (from cache): " + filepath);
        return it->second;
    }

    if (!materialManager) {
        Logger::Log(LogLevel::ERROR,
                    "MaterialManager not set in ModelManager!");
        return nullptr;
    }

    std::string fullPath = assetsPath + filepath;
    auto [rawModel, _] = LoadModelFromFile(fullPath, *materialManager);

    if (!rawModel) {
        Logger::Log(LogLevel::ERROR,
                    "Failed to load model: " + filepath);
        return nullptr;
    }

    std::shared_ptr<Model> model(rawModel);
    loadedModels[filepath] = model;

    Logger::Log(LogLevel::INFO,
                "Model cached: " + filepath +
                " (total models: " + std::to_string(loadedModels.size()) + ")");

    return model;
}

entt::entity ModelManager::LoadWithECS(const std::string &filepath, ECSWorld *world, bool isBaseShape) {
    if (!materialManager) {
        Logger::Log(LogLevel::ERROR,
                    "MaterialManager not set in ModelManager!");
        return entt::null;
    }

    std::string fullPath = assetsPath + filepath;
    auto [rawModel, rootEntity] = LoadModelFromFile(fullPath, *materialManager, world, isBaseShape);

    if (!rawModel) {
        Logger::Log(LogLevel::ERROR,
                    "Failed to load model: " + filepath);
        return entt::null;
    }

    std::shared_ptr<Model> model(rawModel);
    loadedModels[filepath] = model;

    Logger::Log(LogLevel::INFO,
                "Model cached: " + filepath +
                " (total models: " + std::to_string(loadedModels.size()) + ")");

    return rootEntity;
}

std::shared_ptr<Model> ModelManager::Get(const std::string &filepath) {
    auto it = loadedModels.find(filepath);
    if (it != loadedModels.end()) {
        return it->second;
    }

    Logger::Log(LogLevel::WARNING,
                "Model not loaded: " + filepath);
    return nullptr;
}

void ModelManager::Unload(const std::string &filepath) {
    auto it = loadedModels.find(filepath);
    if (it != loadedModels.end()) {
        Logger::Log(LogLevel::INFO,
                    "Model unloaded: " + filepath +
                    " (use_count before erase: " +
                    std::to_string(it->second.use_count()) + ")");
        loadedModels.erase(it);
    }
}

void ModelManager::CleanupUnused() {
    auto it = loadedModels.begin();
    int removed = 0;

    while (it != loadedModels.end()) {
        if (it->second.use_count() == 1) {
            Logger::Log(LogLevel::DEBUG,
                        "Cleaning unused model: " + it->first);
            it = loadedModels.erase(it);
            removed++;
        } else {
            ++it;
        }
    }

    if (removed > 0) {
        Logger::Log(LogLevel::INFO,
                    "Cleaned " + std::to_string(removed) +
                    " unused models (remaining: " +
                    std::to_string(loadedModels.size()) + ")");
    }
}

void ModelManager::UnloadAll() {
    if (!loadedModels.empty()) {
        Logger::Log(LogLevel::INFO,
                    "Unloading all models (" +
                    std::to_string(loadedModels.size()) + ")");
        loadedModels.clear();
    }
}

bool ModelManager::IsLoaded(const std::string &filepath) const {
    return loadedModels.find(filepath) != loadedModels.end();
}

size_t ModelManager::GetLoadedCount() const {
    return loadedModels.size();
}

std::vector<std::string> ModelManager::GetLoadedModelNames() const {
    std::vector<std::string> names;
    for (const auto &[name, _]: loadedModels) {
        names.push_back(name);
    }
    return names;
}

void ModelManager::SetAssetsPath(const std::string &path) {
    assetsPath = path;
}

const std::string &ModelManager::GetAssetsPath() const {
    return assetsPath;
}