#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "resource/texture/TextureManager.h"

using json = nlohmann::json;

struct MaterialConfig {
    std::string name;
    std::string type;
    glm::vec3 color;

    std::string diffusePath;
    std::string specularPath;
    std::string normalPath;
    std::string heightPath;

    MaterialConfig()
        : name(""), type(""), color(1.0f),
          diffusePath(""), specularPath(""), normalPath(""), heightPath("") {
    }
};

class MaterialConfigLoader {
    std::vector<MaterialConfig> materialConfigs;

public:
    const std::vector<MaterialConfig> &LoadMaterialConfigs(TextureManager &textureManager, const std::string &path);

    const std::vector<MaterialConfig> &GetConfigs() const;

private:
    glm::vec3 ParseColor(const json &colorArray);
};