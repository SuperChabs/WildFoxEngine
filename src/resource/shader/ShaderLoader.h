#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "core/logging/Logger.h"

std::string FileToString(const std::string &path);

struct ShaderSource {
    std::string vertex;
    std::string fragment;
    std::string geometry;
    std::string name;

    bool HasGeometry() const { return !geometry.empty(); }
};

namespace ShaderLoader {
    ShaderSource LoadShader(const char *shaderName, bool isGeometry = false);

    ShaderSource LoadShader(const std::string vs, const std::string fs, const std::string gs);

    std::string FileToString(const std::string &path);
} // namespace ShaderLoader