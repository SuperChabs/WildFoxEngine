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

    void SetColor(const glm::vec3 &color) {
        if (!material) return;

        material->SetColor(color);
        material->SetColorUsing(true);
    }

    void SetTextures(const std::vector<Texture> &textures) {
        if (!material) return;

        material->SetTextures(textures);
        material->SetColorUsing(false);
    }

    void SetMaterial(std::shared_ptr<Material> newMaterial) {
        material = newMaterial;
    }

    glm::vec3 GetColor() const { return material ? material->GetColor() : glm::vec3(1.0f); }
    MeshRenderer *GetMeshRenderer() { return meshRenderer.get(); }
    std::shared_ptr<Material> GetMaterial() { return material; }
};