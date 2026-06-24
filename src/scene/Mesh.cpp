#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures) {
    meshRenderer = std::make_unique<MeshRenderer>(vertices, indices);
}

Mesh::Mesh(MeshRenderer *rendererPtr, Material *materialPtr) {
    meshRenderer = std::unique_ptr<MeshRenderer>(rendererPtr);
    (void) materialPtr;
}

void Mesh::Draw(ShaderManager &shaderManager, const std::string &name) {
    if (!meshRenderer) return;

    if (material) {
        material->Bind(shaderManager, name);
        meshRenderer->Draw();
        material->Unbind();
    } else {
        meshRenderer->Draw();
    }
}

void Mesh::Draw(ShaderManager &shaderManager, const std::string &name,
                std::shared_ptr<Material> externalMaterial) {
    if (!meshRenderer) return;

    if (externalMaterial) {
        externalMaterial->Bind(shaderManager, name);
        meshRenderer->Draw();
        externalMaterial->Unbind();
    } else if (material) {
        material->Bind(shaderManager, name);
        meshRenderer->Draw();
        material->Unbind();
    } else {
        meshRenderer->Draw();
    }
}

void Mesh::DrawDepthOnly() {
    if (meshRenderer)
        meshRenderer->Draw();
}

// --- split_headers: auto-generated ---

void Mesh::SetColor(const glm::vec3 &color) {
    if (!material) return;

    material->SetColor(color);
    material->SetColorUsing(true);
}

void Mesh::SetTextures(const std::vector<Texture> &textures) {
    if (!material) return;

    material->SetTextures(textures);
    material->SetColorUsing(false);
}

void Mesh::SetMaterial(std::shared_ptr<Material> newMaterial) {
    material = newMaterial;
}

glm::vec3 Mesh::GetColor() const {
    return material ? material->GetColor() : glm::vec3(1.0f);
}

MeshRenderer *Mesh::GetMeshRenderer() {
    return meshRenderer.get();
}

std::shared_ptr<Material> Mesh::GetMaterial() {
    return material;
}