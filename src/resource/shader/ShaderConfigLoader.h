#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "core/logging/Logger.h"

using json = nlohmann::json;

struct ShaderConfig {
    std::string name;

    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;

    bool HasGeometry() { return !geometryPath.empty(); }
};

class ShaderConfigLoader {
    std::vector<ShaderConfig> shaderConfigs;

public:
    const std::vector<ShaderConfig> &LoadShaderConfigs(const std::string &path);
};