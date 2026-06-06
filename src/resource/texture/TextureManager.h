#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "core/logging/Logger.h"

class TextureManager {
    std::unordered_map<std::string, unsigned int> loadedTextures;

public:
    TextureManager() = default;

    ~TextureManager();

    unsigned int LoadTexture(const std::string &path, bool gamma = false);

    unsigned int LoadCubemap(const std::vector<std::string> &faces);

    unsigned int CreateWhiteTexture();

    void BindTexture(unsigned int textureID, unsigned int slot = 0);

    void UnloadTexture(const std::string &path);

    void UnloadAll();

    bool IsLoaded(const std::string &path) const;

private:
    unsigned int LoadTextureFromFile(const char *path, bool gamma = false);

    unsigned int LoadCubemapFromFiles(const std::vector<std::string> &faces);
};