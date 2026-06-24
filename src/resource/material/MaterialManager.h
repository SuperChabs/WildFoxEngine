#pragma once

#include <string>
#include <memory>
#include <vector>

#include <unordered_map>
#include <glm/glm.hpp>

#include "resource/material/Material.h"
#include "resource/material/MaterialConfigLoader.h"
#include "resource/texture/TextureManager.h"

class MaterialManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Material> > materials;
    TextureManager *textureManager;
    MaterialConfigLoader configLoader;

    std::string runtimePath = "../assets/";
    std::string sourcePath = "../assets/";
    std::string materialConfigPath = "../assets/configs/materials.json";

public:
    explicit MaterialManager(TextureManager *tm);

    void LoadMaterialConfigs();

    std::shared_ptr<Material> CreateColorMaterial(const std::string &name, const glm::vec3 &color);

    std::shared_ptr<Material> CreateTextureMaterial(const std::string &name,
                                                    const std::string &diffusePath,
                                                    const std::string &specularPath = "",
                                                    const std::string &normalPath = "",
                                                    const std::string &heightPath = "");

    void AddMaterial(std::shared_ptr<Material> material);

    void RemoveMaterial(const std::string &name);

    void ClearMaterials();

    bool HasMaterial(const std::string &name) const;

    size_t GetMaterialsCount() const;


    std::shared_ptr<Material> GetMaterial(const std::string &name);

    std::vector<std::string> GetMaterialNames() const;

    const std::unordered_map<std::string, std::shared_ptr<Material> > &GetMaterialsMap() const;

private:
    void CreateDefaultMaterials();

    void RegisterCommands();
};