#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <vector>

#include "resource/shader/ShaderManager.h"
#include "resource/material/Material.h"
#include "rendering/MeshRenderer.h"
#include "rendering/MeshData.h"

class Mesh {
private:
    std::unique_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<Material> material;

public:
    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
         std::vector<Texture> textures);

    Mesh(MeshRenderer *rendererPtr, Material *materialPtr);

    void Draw(ShaderManager &shaderManager, const std::string &name);

    void Draw(ShaderManager &shaderManager,
              const std::string &name,
              std::shared_ptr<Material> externalMaterial);

    void DrawDepthOnly();

    void SetColor(const glm::vec3 &color);


    void SetTextures(const std::vector<Texture> &textures);


    void SetMaterial(std::shared_ptr<Material> newMaterial);


    glm::vec3 GetColor() const;

    MeshRenderer *GetMeshRenderer();

    std::shared_ptr<Material> GetMaterial();
};