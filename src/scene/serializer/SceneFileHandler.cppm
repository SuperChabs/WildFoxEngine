module;

#include <ext/stdlib.hpp>
#include <ext/json.hpp>

export module WFE.Scene.Serializer.SceneFileHandler;

import WFE.Core.Logger;

using json = nlohmann::json;
namespace fs = std::filesystem;

export class SceneFileHandler
{
private:
    std::string savesDirectory;

    static constexpr const char* ENTITIES_FILE  = "entities.json";
    static constexpr const char* MATERIALS_FILE = "materials.json";

public:
    explicit SceneFileHandler(const std::string& directory = "../saves/")
        : savesDirectory(directory)
    {
    }

    std::string EnsureJsonExtension(const std::string& filename) const
    {
        fs::path p(filename);
        if (p.extension() != ".json")
            return (p.string() + ".json");
        return p.string();
    }

    bool EnsureDirectoryExists(const std::string& path) const
    {
        try
        {
            if (!fs::exists(path))
            {
                if (fs::create_directories(path))
                {
                    Logger::Log(LogLevel::INFO, "Created directory: " + path);
                    return true;
                }
                else
                {
                    Logger::Log(LogLevel::ERROR, "Failed to create directory: " + path);
                    return false;
                }
            }
            return true;
        }
        catch (const fs::filesystem_error& e)
        {
            Logger::Log(LogLevel::ERROR, "Filesystem error: " + std::string(e.what()));
            return false;
        }
    }

    json ReadScene(const std::string& filename)
    {
        fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();
        fs::path entitiesPath = scenePath / ENTITIES_FILE;

        if (!fs::exists(entitiesPath))
        {
            Logger::Log(LogLevel::ERROR, "Scene file not found: " + entitiesPath.string());
            return json::object();
        }

        std::ifstream file(entitiesPath);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open scene: " + entitiesPath.string());
            return json::object();
        }

        try
        {
            json sceneData = json::parse(file);
            file.close();
            return sceneData;
        }
        catch (const json::parse_error& e)
        {
            Logger::Log(LogLevel::ERROR, "JSON parse error: " + std::string(e.what()));
            file.close();
            return json::object();
        }
    }

    bool WriteScene(const std::string& filename, const json& sceneData, bool pretty = true)
    {
        fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();

        if (!EnsureDirectoryExists(scenePath.string()))
        {
            Logger::Log(LogLevel::ERROR, "Cannot create saves directory");
            return false;
        }

        fs::path entitiesPath = scenePath / ENTITIES_FILE;

        std::ofstream file(entitiesPath, std::ios::trunc);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open file for writing: " + entitiesPath.string());
            return false;
        }

        try
        {
            if (pretty)
                file << sceneData.dump(4);
            else
                file << sceneData.dump();
            
            file.close();
            Logger::Log(LogLevel::INFO, "Scene saved: " + entitiesPath.string());
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, "Error writing scene: " + std::string(e.what()));
            file.close();
            return false;
        }
    }

    json ReadMaterials(const std::string& filename)
    {
        fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();
        fs::path materialsPath = scenePath / MATERIALS_FILE;

        if (!fs::exists(materialsPath))
        {
            Logger::Log(LogLevel::ERROR, "Scene file not found: " + materialsPath.string());
            return json::object();
        }

        std::ifstream file(materialsPath);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open scene: " + materialsPath.string());
            return json::object();
        }

        try
        {
            json sceneData = json::parse(file);
            file.close();
            return sceneData;
        }
        catch (const json::parse_error& e)
        {
            Logger::Log(LogLevel::ERROR, "JSON parse error: " + std::string(e.what()));
            file.close();
            return json::object();
        }
    }

    bool WriteMaterials(const std::string& filename, const json& sceneData, bool pretty = true)
    {
        fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();

        if (!EnsureDirectoryExists(scenePath.string()))
        {
            Logger::Log(LogLevel::ERROR, "Cannot create saves directory");
            return false;
        }

        fs::path materialsPath = scenePath / MATERIALS_FILE;

        std::ofstream file(materialsPath, std::ios::trunc);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open file for writing: " + materialsPath.string());
            return false;
        }

        try
        {
            if (pretty)
                file << sceneData.dump(4);
            else
                file << sceneData.dump();
            
            file.close();
            Logger::Log(LogLevel::INFO, "Material saved: " + materialsPath.string());
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, "Error writing materials: " + std::string(e.what()));
            file.close();
            return false;
        }
    }

    bool DeleteScene(const std::string& filename)
    {
        fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();

        try
        {
            if (fs::exists(scenePath))
            {
                fs::remove(scenePath);
                Logger::Log(LogLevel::INFO, "Scene deleted: " + scenePath.string());
                return true;
            }
            else
            {
                Logger::Log(LogLevel::WARNING, "Scene file not found: " + scenePath.string());
                return false;
            }
        }
        catch (const fs::filesystem_error& e)
        {
            Logger::Log(LogLevel::ERROR, "Error deleting scene: " + std::string(e.what()));
            return false;
        }
    }

    std::string GetCurrentTimestamp() const
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        std::tm tm_snapshot;
        localtime_r(&time, &tm_snapshot);

        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_snapshot);

        return std::string(buffer);
    }

    std::vector<std::string> GetAvailableScenes() const
    {
        std::vector<std::string> scenes;

        if (!fs::exists(savesDirectory))
            return scenes;

        try
        {
            for (const auto& entry : fs::directory_iterator(savesDirectory))
            {
                if (entry.is_directory())
                    scenes.push_back(entry.path().filename().string());
            }
        }
        catch (const fs::filesystem_error& e)
        {
            Logger::Log(LogLevel::ERROR, "Error listing scenes: " + std::string(e.what()));
        }

        return scenes;
    }

    std::string GetSavesDirectory() const { return savesDirectory; }

    void SetSavesDirectory(const std::string& directory) { savesDirectory = directory; }
};
