#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <ctime>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

class SceneFileHandler {
private:
    std::string savesDirectory;

    static constexpr const char *ENTITIES_FILE = "entities.json";
    static constexpr const char *MATERIALS_FILE = "materials.json";

public:
    explicit SceneFileHandler(const std::string &directory = "../saves/");

    std::string EnsureJsonExtension(const std::string &filename) const;

    bool EnsureDirectoryExists(const std::string &path) const;

    json ReadScene(const std::string &filename);

    bool WriteScene(const std::string &filename, const json &sceneData, bool pretty = true);

    json ReadMaterials(const std::string &filename);

    bool WriteMaterials(const std::string &filename, const json &sceneData, bool pretty = true);

    bool DeleteScene(const std::string &filename);

    std::string GetCurrentTimestamp() const;

    std::vector<std::string> GetAvailableScenes() const;

    std::string GetSavesDirectory() const;

    void SetSavesDirectory(const std::string &directory);
};