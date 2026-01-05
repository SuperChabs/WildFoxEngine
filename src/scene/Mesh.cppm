module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>

export module XEngine.Scene.Mesh;

import XEngine.Core.Shader;
import XEngine.Rendering.Material;
import XEngine.Rendering.GPUMesh;
import XEngine.Rendering.MeshData;

export class Mesh 
{
private:
    std::unique_ptr<GPUMesh> gpuMesh;
    std::unique_ptr<Material> material;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        gpuMesh = std::make_unique<GPUMesh>(vertices, indices);
        material = std::make_unique<Material>(textures);    
    }

    Mesh(GPUMesh* gpuMeshPtr, Material* materialPtr)
    {
        gpuMesh = std::unique_ptr<GPUMesh>(gpuMeshPtr);
        material = std::unique_ptr<Material>(materialPtr);
    }

    void Draw(Shader& shader)
    {  
        material->Bind(shader);
        gpuMesh->Draw();
        material->Unbind();
    }

    void SetColor(const glm::vec3& color) { material->SetColor(color); }
    void SetTextures(const std::vector<Texture>& textures) { material->SetTextures(textures); }
};