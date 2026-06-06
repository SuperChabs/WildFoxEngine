#include "ShaderConfigLoader.h"

#include <fstream>

const std::vector<ShaderConfig> &ShaderConfigLoader::LoadShaderConfigs(const std::string &path) {
    shaderConfigs.clear();

    std::ifstream f(path);
    if (!f.is_open()) {
        Logger::Log(LogLevel::ERROR, "Failed to open JSON file: " + path);
        return shaderConfigs;
    }

    try {
        json data = json::parse(f);

        for (auto &s: data["shader"]) {
            ShaderConfig config;
            if (s.contains("name"))
                config.name = s["name"];
            if (s.contains("vertex"))
                config.vertexPath = s["vertex"];
            if (s.contains("fragment"))
                config.fragmentPath = s["fragment"];
            if (s.contains("geometry"))
                config.geometryPath = s["geometry"];

            shaderConfigs.push_back(config);
        }
    } catch (const json::parse_error &e) {
        Logger::Log(LogLevel::ERROR, "JSON parse error: " + std::string(e.what()));
    } catch (const json::type_error &e) {
        Logger::Log(LogLevel::ERROR, "JSON type error: " + std::string(e.what()));
    }

    return shaderConfigs;
}