#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "rendering/MeshData.h"
#include "resource/shader/ShaderManager.h"

class Material {
private:
    std::string name;
    std::string type;
    std::vector<Texture> textures;
    glm::vec3 color;
    bool useColor = false;

public:
    Material(const std::vector<Texture> &textures, const std::string &materialName = "unnamed");

    Material(const glm::vec3 &solidColor, const std::string &materialName = "unnamed");

    Material &operator=(Material &other);

    void Bind(ShaderManager &shaderManager, const std::string &shaderName);

    void Unbind();

    void SetColor(glm::vec3 newColor);

    void SetTextures(std::vector<Texture> newTextures);

    void SetColorUsing(bool newUsing) { useColor = newUsing; }
    void SetName(const std::string &newName) { name = newName; }
    void SetType(const std::string &newType) { type = newType; }

    const std::vector<Texture> &GetTextures() const { return textures; }
    size_t GetTextureCount() const { return textures.size(); }
    glm::vec3 GetColor() const { return color; }
    bool IsUsingColor() const { return useColor; }
    const std::string &GetName() const { return name; }
    const std::string &GetType() const { return type; }
};