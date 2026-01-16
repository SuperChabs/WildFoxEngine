module;

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <cstddef>

#include <glm/glm.hpp>

export module XEngine.Rendering.MaterialManager;

import XEngine.Rendering.MeshData;
import XEngine.Rendering.Material;
import XEngine.Rendering.TextureManager;
import XEngine.Core.Logger;

export class MaterialManager 
{
private:
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    TextureManager* textureManager;

public:
    MaterialManager(TextureManager* tm) 
        : textureManager(tm) 
    { 
        CreateDefaultMaterials(); 
    }

    std::shared_ptr<Material> CreateColorMaterial(const std::string name, const glm::vec3& color)
    {
        if (materials.contains(name))
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, "Color material with name '" + name + "' alrady exists");
            return materials[name];
        }

        auto material = std::make_shared<Material>(color);
        if(!material)
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, "Unable to create " + name + " material");
            return materials[name];
        }

        materials[name] = material;

        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, "Created color material: " + name);
        return material;
    }

    std::shared_ptr<Material> CreateTextureMaterial(const std::string name, const std::string diffusePath, 
                                                                            const std::string specularPath = "", 
                                                                            const std::string normalPath = "",
                                                                            const std::string heightPath = "")
    {
        if (materials.contains(name))
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, "Material with name '" + name + "' alrady exists");
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

        auto material = std::make_shared<Material>(textures);
        if(!material)
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, "Unable to create " + name + " material");
            return materials[name];
        }

        materials[name] = material;

        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, "Created texture material " + name);
        return material;
    }

    std::shared_ptr<Material> GetMaterial(const std::string& name)
    {
        auto it = materials.find(name);
        if (it != materials.end())
            return it->second;

        Logger::Log(LogLevel::WARNING, "Material '" + name + "' not found");
        return materials["default"];
    }

    bool HasMaterial(const std::string& name)
    {
        return materials.contains(name);
    }

    void RemoveMaterial(const std::string& name)
    {
        if (!materials.contains(name))
            Logger::Log(LogLevel::WARNING, "Unable to delete '" + name + "' material");

        if (name == "default")
        {
            Logger::Log(LogLevel::WARNING, "Unable to delete default material");
            return;
        }

        auto it = materials.find(name);
        if (it != materials.end())
        {
            materials.erase(name);
            Logger::Log(LogLevel::INFO, "Material '" + name + "' was deleted");
        }
    }

    void ClearMaterials()
    {
        auto defaultMat = materials["default"];
        materials.clear();
        materials["default"] = defaultMat;
        Logger::Log(LogLevel::INFO, "All material were cleared");
    }

    size_t GetMaterialsCount() { return materials.size(); }

private:
    void CreateDefaultMaterials()
    {
        auto defaultMat = std::make_shared<Material>(glm::vec3(1.0f, 1.0f, 1.0f));
        materials["default"] = defaultMat;

        materials["red"] = std::make_shared<Material>(glm::vec3(1.0f, 0.0f, 0.0f));
        materials["green"] = std::make_shared<Material>(glm::vec3(0.0f, 1.0f, 0.0f));
        materials["blue"] = std::make_shared<Material>(glm::vec3(0.0f, 0.0f, 1.0f));
        materials["yellow"] = std::make_shared<Material>(glm::vec3(1.0f, 1.0f, 0.0f));
        materials["cyan"] = std::make_shared<Material>(glm::vec3(0.0f, 1.0f, 1.0f));
        materials["magenta"] = std::make_shared<Material>(glm::vec3(1.0f, 0.0f, 1.0f));
        materials["gray"] = std::make_shared<Material>(glm::vec3(0.5f, 0.5f, 0.5f));

        Logger::Log(LogLevel::INFO, "Default materials created");
    }
};