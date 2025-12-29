#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <vector>

#include "rendering/MeshData.h"
#include "core/Shader.h"

class Material 
{
private:
    std::string name;
    std::vector<Texture> textures;

public:
    Material(const std::vector<Texture>& textures);
    
    void Bind(Shader& shader);
    void Unbind();
    
    const std::vector<Texture>& GetTextures() const { return textures; }
    size_t GetTextureCount() const { return textures.size(); }
};

#endif