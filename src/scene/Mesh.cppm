module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <memory>

export module WFE.Scene.Mesh;

import WFE.Resource.Shader.ShaderManager;

import WFE.Resource.Material.Material;
import WFE.Rendering.MeshRenderer;
import WFE.Rendering.MeshData;

export class Mesh 
{
private:
    std::unique_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<Material> material;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        meshRenderer = std::make_unique<MeshRenderer>(vertices, indices);
        // material = std::make_unique<Material>(textures);    
    }

    Mesh(MeshRenderer* MeshRendererPtr, Material* materialPtr)
    {
        meshRenderer = std::unique_ptr<MeshRenderer>(MeshRendererPtr);
        // material = std::unique_ptr<Material>(materialPtr);
    }

    void Draw(ShaderManager& shaderManager, const std::string& name)
    {  
        if (material)
        {
            material->Bind(shaderManager, name);
            meshRenderer->Draw();
            material->Unbind();
        }
    }

    void Draw(ShaderManager& shaderManager, const std::string& name, std::shared_ptr<Material> externalMaterial)
    {  
        if (externalMaterial)
        {
            externalMaterial->Bind(shaderManager, name);
            meshRenderer->Draw();
            externalMaterial->Unbind();
        }
        else if (material)
        {
            material->Bind(shaderManager, name);
            meshRenderer->Draw();
            material->Unbind();
        }
    }

    void SetColor(const glm::vec3& color) 
    { 
        if (material)
        { 
            material->SetColor(color); 
            material->SetColorUsing(true);
        }
    }
    void SetTextures(const std::vector<Texture>& textures) 
    { 
        if (material)
        {
            material->SetTextures(textures); 
            material->SetColorUsing(false);
        }
    }

    void SetMaterial(std::shared_ptr<Material> newMaterial) 
    { 
        material = newMaterial; 
    }

    glm::vec3 GetColor() const { return material->GetColor(); }
    MeshRenderer* GetMeshRenderer() { return meshRenderer.get(); }
    std::shared_ptr<Material> GetMaterial() { return material; }
};