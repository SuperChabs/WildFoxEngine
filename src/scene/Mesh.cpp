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