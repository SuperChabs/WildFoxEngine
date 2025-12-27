#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>

class TextureManager 
{
private:
    std::unordered_map<std::string, unsigned int> loadedTextures;
    
    unsigned int LoadTextureFromFile(const char* path, bool gamma = false);
    unsigned int LoadCubemapFromFiles(const std::vector<std::string>& faces);

public:
    TextureManager() = default;
    ~TextureManager();
    
    unsigned int LoadTexture(const std::string& path, bool gamma = false);
    unsigned int LoadCubemap(const std::vector<std::string>& faces);
    
    void BindTexture(unsigned int textureID, unsigned int slot = 0);
    void UnloadTexture(const std::string& path);
    void UnloadAll();
    
    bool IsLoaded(const std::string& path) const;
};

#endif