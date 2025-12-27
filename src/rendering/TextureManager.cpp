#include "rendering/TextureManager.h"
#include <stb_image.h>
#include <iostream>

TextureManager::~TextureManager()
{
    UnloadAll();
}

unsigned int TextureManager::LoadTexture(const std::string& path, bool gamma)
{
    if (IsLoaded(path))
    {
        return loadedTextures[path];
    }
    
    unsigned int textureID = LoadTextureFromFile(path.c_str(), gamma);
    if (textureID != 0)
    {
        loadedTextures[path] = textureID;
    }
    
    return textureID;
}

unsigned int TextureManager::LoadCubemap(const std::vector<std::string>& faces)
{
    std::string key = "cubemap_";
    for (const auto& face : faces)
        key += face;
    
    if (IsLoaded(key))
    {
        return loadedTextures[key];
    }
    
    unsigned int textureID = LoadCubemapFromFiles(faces);
    if (textureID != 0)
    {
        loadedTextures[key] = textureID;
    }
    
    return textureID;
}

unsigned int TextureManager::LoadTextureFromFile(const char* path, bool gamma)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format, texWrap;
        if (nrComponents == 1)
        {    
            format = GL_RED;
            texWrap = GL_REPEAT;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
            texWrap = GL_REPEAT;
        }   
        else if (nrComponents == 4) 
        {
            format = GL_RGBA;
            texWrap = GL_CLAMP_TO_EDGE;
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}

unsigned int TextureManager::LoadCubemapFromFiles(const std::vector<std::string>& faces)
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
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
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

void TextureManager::BindTexture(unsigned int textureID, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void TextureManager::UnloadTexture(const std::string& path)
{
    auto it = loadedTextures.find(path);
    if (it != loadedTextures.end())
    {
        glDeleteTextures(1, &it->second);
        loadedTextures.erase(it);
    }
}

void TextureManager::UnloadAll()
{
    for (auto& pair : loadedTextures)
    {
        glDeleteTextures(1, &pair.second);
    }
    loadedTextures.clear();
}

bool TextureManager::IsLoaded(const std::string& path) const
{
    return loadedTextures.find(path) != loadedTextures.end();
}