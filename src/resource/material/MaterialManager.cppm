module;

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <cstddef>

#include <glm/glm.hpp>

export module WFE.Resource.Material.MaterialManager;

import WFE.Rendering.MeshData;
import WFE.Resource.Material.Material;

import WFE.Resource.Texture.TextureManager;
import WFE.Resource.Material.MaterialConfigLoader;

import WFE.Core.Logger;

export class MaterialManager 
{
private:
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    TextureManager* textureManager;
    MaterialConfigLoader configLoader;

    std::string runtimePath = "../assets/";
    std::string sourcePath = "../assets/";
    std::string materialConfigPath = "../assets/configs/materials.json";

public:
    MaterialManager(TextureManager* tm) 
        : textureManager(tm) 
    { 
        CreateDefaultMaterials(); 
    }

    void LoadMaterialConfigs()
    {
        const std::vector<MaterialConfig>& configs = configLoader.LoadMaterialConfigs(*textureManager, materialConfigPath);
        
        for (const auto& config : configs)
        {
            if (materials.contains(config.name))
            {
                Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                    "Material already loaded, skipping: " + config.name);
                continue;
            }

            if (config.type == "color")
            {
                CreateColorMaterial(config.name, config.color);
            }
            else if (config.type == "texture")
            {
                CreateTextureMaterial(config.name, 
                    runtimePath + config.diffusePath,
                    runtimePath + config.specularPath,
                    runtimePath + config.normalPath,
                    runtimePath + config.heightPath);
            }
        }
    }

    std::shared_ptr<Material> CreateColorMaterial(const std::string name, const glm::vec3& color)
    {
        if (materials.contains(name))
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                "Color material with name '" + name + "' already exists");
            return materials[name];
        }

        auto material = std::make_shared<Material>(color, name);
        if (!material)
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, 
                "Unable to create color material: " + name);
            return nullptr;
        }

        materials[name] = material;
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, "Created color material: " + name);
        return material;
    }

    std::shared_ptr<Material> CreateTextureMaterial(const std::string name, 
                                                    const std::string diffusePath, 
                                                    const std::string specularPath = "", 
                                                    const std::string normalPath = "",
                                                    const std::string heightPath = "")
    {
        if (materials.contains(name))
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                "Material with name '" + name + "' already exists");
            return materials[name];
        }

        std::vector<Texture> textures;

        if (!diffusePath.empty())
        {
            unsigned int diffuseMap = textureManager->LoadTexture(diffusePath);
            if (diffuseMap != 0)
            {
                Texture diff
                {
                    .id = diffuseMap,
                    .type = "texture_diffuse",
                    .path = diffusePath
                };
                textures.push_back(diff);
            }
        }

        if (!specularPath.empty())
        {
            unsigned int specularMap = textureManager->LoadTexture(specularPath);
            if (specularMap != 0)
            {
                Texture spec
                {
                    .id = specularMap,
                    .type = "texture_specular",
                    .path = specularPath
                };
                textures.push_back(spec);
            }
        }

        if (!normalPath.empty())
        {
            unsigned int normalMap = textureManager->LoadTexture(normalPath);
            if (normalMap != 0)
            {
                Texture norm
                {
                    .id = normalMap,
                    .type = "texture_normal",
                    .path = normalPath
                };
                textures.push_back(norm);
            }
        }

        if (!heightPath.empty())
        {
            unsigned int heightMap = textureManager->LoadTexture(heightPath);
            if (heightMap != 0)
            {
                Texture height
                {
                    .id = heightMap,
                    .type = "texture_height",
                    .path = heightPath
                };
                textures.push_back(height);
            }
        }

        auto material = std::make_shared<Material>(textures, name);
        if (!material)
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, 
                "Unable to create texture material: " + name);
            return nullptr;
        }

        materials[name] = material;
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, "Created texture material: " + name);
        return material;
    }

    void AddMaterial(std::shared_ptr<Material> material)
    {
        if (material)
            materials[material->GetName()] = material;
    }

    std::shared_ptr<Material> GetMaterial(const std::string& name)
    {
        auto it = materials.find(name);
        if (it != materials.end())
            return it->second;

        Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, "Material '" + name + "' not found");
        return materials["default"];
    }

    bool HasMaterial(const std::string& name) const
    {
        return materials.contains(name);
    }

    void RemoveMaterial(const std::string& name)
    {
        if (!materials.contains(name))
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                "Unable to delete - material not found: '" + name + "'");
            return;
        }

        if (name == "default")
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                "Unable to delete default material");
            return;
        }

        materials.erase(name);
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, "Material '" + name + "' was deleted");
    }

    void ClearMaterials()
    {
        auto defaultMat = materials["default"];
        materials.clear();
        materials["default"] = defaultMat;
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, "All materials were cleared");
    }

    size_t GetMaterialsCount() const { return materials.size(); }

    const std::unordered_map<std::string, std::shared_ptr<Material>>& GetMaterialsMap() const
    {
        return materials;
    }

    std::vector<std::string> GetMaterialNames() const
    {
        std::vector<std::string> names;
        for (const auto& [name, _] : materials)
            names.push_back(name);
        return names;
    }

private:
    void CreateDefaultMaterials()
    {
        auto defaultMat = std::make_shared<Material>(glm::vec3(1.0f, 1.0f, 1.0f), "default");
        materials["default"] = defaultMat;

        materials["gray"] = std::make_shared<Material>(glm::vec3(0.5f, 0.5f, 0.5f), "gray");

        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, "Default materials created");
    }
};