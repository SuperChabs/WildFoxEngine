#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <ctime>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

enum class FileType : uint8_t {
    ENTITY,
    MATERIAL,
    META
};

class SceneFileHandler {
    std::string savesDirectory;

    static constexpr auto ENTITIES_FILE = "entities.json";
    static constexpr auto MATERIALS_FILE = "materials.json";
    static constexpr auto META_FILE = "meta.json";

public:
    explicit SceneFileHandler(const std::string &directory = "../saves/");

    [[nodiscard]] static std::string EnsureJsonExtension(const std::string &filename) ;
    [[nodiscard]] static bool EnsureDirectoryExists(const std::string &path);

    json ReadFile(const std::string &filename, FileType type) const;
    void WriteFile(const std::string &filename, const json &sceneData, FileType type, bool pretty = true) const;
    void DeleteFile(const std::string &filename) const;

    [[nodiscard]] static std::string GetCurrentTimestamp() ;
    [[nodiscard]] std::vector<std::string> GetAvailableScenes() const;
    [[nodiscard]] std::string GetSavesDirectory() const { return savesDirectory; }

    void SetSavesDirectory(const std::string &directory);
};