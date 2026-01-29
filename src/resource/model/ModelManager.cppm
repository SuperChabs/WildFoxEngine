module;

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

export module WFE.Resource.Model.ModelManager;

import WFE.Resource.Model.Model;
import WFE.Resource.Model.ModelLoader;
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Material.Material;
import WFE.Core.Logger;
import WFE.ECS.ECSWorld;

export class ModelManager 
{
private:
    std::unordered_map<std::string, std::shared_ptr<Model>> loadedModels;
    std::string assetsPath = "../assets/objects/";
    MaterialManager* materialManager = nullptr;

public:
    ModelManager() = default;
    
    ~ModelManager() 
    {
        UnloadAll();
    }
    
    void SetMaterialManager(MaterialManager* mm)
    {
        materialManager = mm;
    }

    std::shared_ptr<Model> Load(const std::string& filepath) 
    {
        auto it = loadedModels.find(filepath);
        if (it != loadedModels.end()) 
        {
            Logger::Log(LogLevel::INFO, 
                "Model already loaded (from cache): " + filepath);
            return it->second;
        }
        
        if (!materialManager)
        {
            Logger::Log(LogLevel::ERROR, 
                "MaterialManager not set in ModelManager!");
            return nullptr;
        }
        
        std::string fullPath = assetsPath + filepath;
        Model* rawModel = LoadModelFromFile(fullPath, *materialManager);
        
        if (!rawModel) 
        {
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
    
    std::shared_ptr<Model> LoadWithECS(const std::string& filepath, ECSWorld* world) 
    {
        auto it = loadedModels.find(filepath);
        if (it != loadedModels.end()) 
        {
            Logger::Log(LogLevel::INFO, 
                "Model already loaded (from cache): " + filepath);
            return it->second;
        }
        
        if (!materialManager)
        {
            Logger::Log(LogLevel::ERROR, 
                "MaterialManager not set in ModelManager!");
            return nullptr;
        }
        
        std::string fullPath = assetsPath + filepath;
        Model* rawModel = LoadModelFromFile(fullPath, *materialManager, world);
        
        if (!rawModel) 
        {
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

    std::shared_ptr<Model> Get(const std::string& filepath) 
    {
        auto it = loadedModels.find(filepath);
        if (it != loadedModels.end()) 
        {
            return it->second;
        }
        
        Logger::Log(LogLevel::WARNING, 
            "Model not loaded: " + filepath);
        return nullptr;
    }
    
    void Unload(const std::string& filepath) 
    {
        auto it = loadedModels.find(filepath);
        if (it != loadedModels.end()) 
        {
            Logger::Log(LogLevel::INFO, 
                "Model unloaded: " + filepath + 
                " (use_count before erase: " + 
                std::to_string(it->second.use_count()) + ")");
            loadedModels.erase(it);
        }
    }
    
    void CleanupUnused() 
    {
        auto it = loadedModels.begin();
        int removed = 0;
        
        while (it != loadedModels.end()) 
        {
            if (it->second.use_count() == 1)  
            {
                Logger::Log(LogLevel::DEBUG, 
                    "Cleaning unused model: " + it->first);
                it = loadedModels.erase(it);
                removed++;
            } 
            else 
            {
                ++it;
            }
        }
        
        if (removed > 0) 
        {
            Logger::Log(LogLevel::INFO, 
                "Cleaned " + std::to_string(removed) + 
                " unused models (remaining: " + 
                std::to_string(loadedModels.size()) + ")");
        }
    }
    
    void UnloadAll() 
    {
        if (!loadedModels.empty())
        {
            Logger::Log(LogLevel::INFO, 
                "Unloading all models (" + 
                std::to_string(loadedModels.size()) + ")");
            loadedModels.clear();
        }
    }


    bool IsLoaded(const std::string& filepath) const 
    {
        return loadedModels.find(filepath) != loadedModels.end();
    }
    
    size_t GetLoadedCount() const 
    {
        return loadedModels.size();
    }

    std::vector<std::string> GetLoadedModelNames() const 
    {
        std::vector<std::string> names;
        for (const auto& [name, _] : loadedModels) 
        {
            names.push_back(name);
        }
        return names;
    }
    
    void SetAssetsPath(const std::string& path) 
    {
        assetsPath = path;
    }
    
    const std::string& GetAssetsPath() const 
    {
        return assetsPath;
    }
};