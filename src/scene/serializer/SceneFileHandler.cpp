#include "SceneFileHandler.h"

#include <fstream>
#include <chrono>

#include "core/logging/Logger.h"
#include "core/PlatformTime.h"

SceneFileHandler::SceneFileHandler(const std::string &directory)
    : savesDirectory(directory) {
}

std::string SceneFileHandler::EnsureJsonExtension(const std::string &filename) {
    const fs::path p(filename);
    if (p.extension() != ".json")
        return (p.string() + ".json");
    return p.string();
}

bool SceneFileHandler::EnsureDirectoryExists(const std::string &path) {
    try {
        if (!fs::exists(path)) {
            if (fs::create_directories(path)) {
                Logger::Log(LogLevel::INFO, "Created directory: " + path);
                return true;
            }
            Logger::Log(LogLevel::ERROR, "Failed to create directory: " + path);
            return false;
        }
        return true;
    } catch (const fs::filesystem_error &e) {
        Logger::Log(LogLevel::ERROR, "Filesystem error: " + std::string(e.what()));
        return false;
    }
}

json SceneFileHandler::ReadFile(const std::string &filename, FileType type) const {
    fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();
    fs::path filePath;

    switch (type) {
        case FileType::ENTITY:   filePath = scenePath / "entities.json";
            break;
        case FileType::MATERIAL: filePath = scenePath / "materials.json";
            break;
        case FileType::META:     filePath = scenePath / "meta.json";
            break;
    }

    if (!fs::exists(filePath)) {
        Logger::Log(LogLevel::ERROR, "File not found: " + filePath.string());
        return json::object();
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        Logger::Log(LogLevel::ERROR, "Failed to open file: " + filePath.string());
        return json::object();
    }

    try {
        json fileData = json::parse(file);
        file.close();
        return fileData;
    } catch (const json::parse_error &e) {
        Logger::Log(LogLevel::ERROR, "JSON parse error: " + std::string(e.what()));
        file.close();
        return json::object();
    }
}

void SceneFileHandler::WriteFile(const std::string &filename, const json &sceneData, const FileType type, bool pretty) const {
    fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();

    if (!EnsureDirectoryExists(scenePath.string())) {
        Logger::Log(LogLevel::ERROR, "Cannot create saves directory");
        return;
    }

    fs::path filePath;

    switch (type) {
        case FileType::ENTITY:   filePath = scenePath / "entities.json";
            break;
        case FileType::MATERIAL: filePath = scenePath / "materials.json";
            break;
        case FileType::META:     filePath = scenePath / "meta.json";
            break;
    }

    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) {
        Logger::Log(LogLevel::ERROR, "Failed to open file for writing: " + filePath.string());
        return;
    }

    try {
        if (pretty)
            file << sceneData.dump(4);
        else
            file << sceneData.dump();

        file.close();
        Logger::Log(LogLevel::INFO, "File saved: " + filePath.string());
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR, "Error writing scene: " + std::string(e.what()));
        file.close();
    }
}

void SceneFileHandler::DeleteFile(const std::string &filename) const {
    const fs::path scenePath = fs::path(savesDirectory) / fs::path(filename).stem();

    try {
        if (fs::exists(scenePath)) {
            fs::remove_all(scenePath);
            Logger::Log(LogLevel::INFO, "Scene deleted: " + scenePath.string());
        }
        Logger::Log(LogLevel::WARNING, "Scene file not found: " + scenePath.string());
    } catch (const fs::filesystem_error &e) {
        Logger::Log(LogLevel::ERROR, "Error deleting scene: " + std::string(e.what()));
    }
}

std::string SceneFileHandler::GetCurrentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm tm_snapshot{};
    PortableLocalTime(time, tm_snapshot);

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_snapshot);

    return buffer;
}

std::vector<std::string> SceneFileHandler::GetAvailableScenes() const {
    std::vector<std::string> scenes;

    if (!fs::exists(savesDirectory))
        return scenes;

    try {
        for (const auto &entry: fs::directory_iterator(savesDirectory)) {
            if (entry.is_directory())
                scenes.push_back(entry.path().filename().string());
        }
    } catch (const fs::filesystem_error &e) {
        Logger::Log(LogLevel::ERROR, "Error listing scenes: " + std::string(e.what()));
    }

    return scenes;
}

void SceneFileHandler::SetSavesDirectory(const std::string &directory) {
    savesDirectory = directory;
}