#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "rendering/MeshData.h"
#include "core/Shader.h"

class Material 
{
private:
    std::string name;
    std::vector<Texture> textures;
    glm::vec3 color;
    bool useColor; 

public:
    Material(const std::vector<Texture>& textures);
    Material(const glm::vec3& solidColor);
    
    void Bind(Shader& shader);
    void Unbind();

    void SetColor(glm::vec3 newColor) { color = newColor; }
    void SetTextures(std::vector<Texture> textures) {this->textures = textures; }
    void SetColorUsing(bool newUsing) { useColor = newUsing; }
    
    const std::vector<Texture>& GetTextures() const { return textures; }
    size_t GetTextureCount() const { return textures.size(); }
    const glm::vec3 GetColor() const {return color; }
    bool IsUsingColor () const {return useColor; }
};

#endif