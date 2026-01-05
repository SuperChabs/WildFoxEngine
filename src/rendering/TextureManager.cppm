module;

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <stb_image.h>

export module XEngine.Rendering.TextureManager;

import XEngine.Utils.Logger;

export class TextureManager
{
private:
    std::unordered_map<std::string, unsigned int> loadedTextures;

    unsigned int LoadTextureFromFile(const char* path, bool gamma = false)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (!data)
        {
            Logger::Log(LogLevel::ERROR, "Texture failed to load at path: " + std::string(path));
            return 0;
        }

        GLenum format = GL_RGB;
        GLenum texWrap = GL_REPEAT;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) { format = GL_RGBA; texWrap = GL_CLAMP_TO_EDGE; }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return textureID;
    }

    unsigned int LoadCubemapFromFiles(const std::vector<std::string>& faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrComponents;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                Logger::Log(LogLevel::ERROR, "Cubemap texture failed to load at path: " + faces[i]);
                return 0;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

public:
    TextureManager() = default;

    ~TextureManager()
    {
        UnloadAll();
    }

    unsigned int LoadTexture(const std::string& path, bool gamma = false)
    {
        if (IsLoaded(path)) return loadedTextures[path];
        unsigned int textureID = LoadTextureFromFile(path.c_str(), gamma);
        if (textureID != 0)
            loadedTextures[path] = textureID;
        return textureID;
    }

    unsigned int LoadCubemap(const std::vector<std::string>& faces)
    {
        std::string key = "cubemap_";
        for (const auto& face : faces) key += face;
        if (IsLoaded(key)) return loadedTextures[key];
        unsigned int textureID = LoadCubemapFromFiles(faces);
        if (textureID != 0) loadedTextures[key] = textureID;
        return textureID;
    }

    void BindTexture(unsigned int textureID, unsigned int slot = 0)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void UnloadTexture(const std::string& path)
    {
        auto it = loadedTextures.find(path);
        if (it != loadedTextures.end())
        {
            glDeleteTextures(1, &it->second);
            loadedTextures.erase(it);
        }
    }

    void UnloadAll()
    {
        for (auto& pair : loadedTextures)
            glDeleteTextures(1, &pair.second);
        loadedTextures.clear();
    }

    bool IsLoaded(const std::string& path) const
    {
        return loadedTextures.find(path) != loadedTextures.end();
    }
};
