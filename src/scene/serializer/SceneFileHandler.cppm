module;

#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.SceneFileHandler;

import WFE.Core.Logger;

using json = nlohmann::json;
namespace fs = std::filesystem;

export class SceneFileHandler
{
private:
    std::string savesDirectory;

public:
    explicit SceneFileHandler(const std::string& directory = "../saves/")
        : savesDirectory(directory)
    {
    }

    void SetSavesDirectory(const std::string& directory)
    {
        savesDirectory = directory;
    }

    std::string GetSavesDirectory() const
    {
        return savesDirectory;
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
        std::string name = EnsureJsonExtension(filename);
        std::string filepath = savesDirectory + name;

        if (!fs::exists(filepath))
        {
            Logger::Log(LogLevel::ERROR, "Scene file not found: " + filepath);
            return json::object();
        }

        std::ifstream file(filepath);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open scene: " + filepath);
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
        if (!EnsureDirectoryExists(savesDirectory))
        {
            Logger::Log(LogLevel::ERROR, "Cannot create saves directory");
            return false;
        }

        std::string name = EnsureJsonExtension(filename);
        std::string filepath = savesDirectory + name;

        std::ofstream file(filepath, std::ios::trunc);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open file for writing: " + filepath);
            return false;
        }

        try
        {
            if (pretty)
                file << sceneData.dump(4);
            else
                file << sceneData.dump();
            
            file.close();
            Logger::Log(LogLevel::INFO, "Scene saved: " + filepath);
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, "Error writing scene: " + std::string(e.what()));
            file.close();
            return false;
        }
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
                if (entry.is_regular_file() && entry.path().extension() == ".json")
                    scenes.push_back(entry.path().filename().string());
            }
        }
        catch (const fs::filesystem_error& e)
        {
            Logger::Log(LogLevel::ERROR, "Error listing scenes: " + std::string(e.what()));
        }

        return scenes;
    }

    bool DeleteScene(const std::string& filename)
    {
        std::string name = EnsureJsonExtension(filename);
        std::string filepath = savesDirectory + name;

        try
        {
            if (fs::exists(filepath))
            {
                fs::remove(filepath);
                Logger::Log(LogLevel::INFO, "Scene deleted: " + filepath);
                return true;
            }
            else
            {
                Logger::Log(LogLevel::WARNING, "Scene file not found: " + filepath);
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
};
