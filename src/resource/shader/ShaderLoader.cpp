#include "ShaderLoader.h"

ShaderSource ShaderLoader::LoadShader(const char *shaderName, bool isGeometry) {
    ShaderSource source;
    source.name = shaderName;

    const std::string name(shaderName);

    const std::string vs = "assets/shaders/vertex/" + name + ".vs";
    const std::string fs = "assets/shaders/fragment/" + name + ".fs";

    source =
    {
        .vertex = FileToString(vs),
        .fragment = FileToString(fs)
    };

    if (isGeometry) {
        const std::string gs = "assets/shaders/geometry/" + name + ".gs";
        source =
        {
            .geometry = FileToString(gs)
        };
    }

    return source;
}

ShaderSource ShaderLoader::LoadShader(const std::string vs, const std::string fs, const std::string gs) {
    ShaderSource source;

    if (!gs.empty()) {
        source =
        {
            .vertex = FileToString(vs),
            .fragment = FileToString(fs),
            .geometry = FileToString(gs)
        };
    } else {
        source =
        {
            .vertex = FileToString(vs),
            .fragment = FileToString(fs)
        };
    }

    if (source.vertex.empty() || source.fragment.empty())
        Logger::Log(LogLevel::ERROR,
                    "Failed to load shader from paths: " + vs + ", " + fs + ", " + (!gs.empty() ? gs : ""));
    else
        Logger::Log(LogLevel::INFO, "Shader source loaded from paths: " + source.name);

    return source;
}

std::string ShaderLoader::FileToString(const std::string &path) {
    namespace fs = std::filesystem;

    if (!fs::exists(path)) {
        Logger::Log(LogLevel::ERROR, "Shader file not found: " + path);
        return "";
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        Logger::Log(LogLevel::ERROR, "Failed to open shader file: " + path);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string content = buffer.str();

    if (content.empty())
        Logger::Log(LogLevel::WARNING, "Shader file is empty: " + path);

    return content;
}