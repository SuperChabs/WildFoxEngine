module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <memory>
#include <vector>

export module WFE.Scene.Mesh;

import WFE.Resource.Shader.ShaderManager;
import WFE.Resource.Material.Material;
import WFE.Rendering.MeshRenderer;
import WFE.Rendering.MeshData;

export class Mesh
{
private:
    std::unique_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<Material>     material;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<Texture> textures)
    {
        meshRenderer = std::make_unique<MeshRenderer>(vertices, indices);
    }

    Mesh(MeshRenderer* rendererPtr, Material* materialPtr)
    {
        meshRenderer = std::unique_ptr<MeshRenderer>(rendererPtr);
        (void)materialPtr;
    }

    void Draw(ShaderManager& shaderManager, const std::string& name)
    {
        if (!meshRenderer) return;

        if (material)
        {
            material->Bind(shaderManager, name);
            meshRenderer->Draw();
            material->Unbind();
        }
        else
        {
            meshRenderer->Draw();
        }
    }

    void Draw(ShaderManager& shaderManager, const std::string& name,
              std::shared_ptr<Material> externalMaterial)
    {
        if (!meshRenderer) return;

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
        else
        {
            meshRenderer->Draw();
        }
    }


    void DrawDepthOnly()
    {
        if (meshRenderer)
            meshRenderer->Draw();
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

    glm::vec3               GetColor()        const { return material ? material->GetColor() : glm::vec3(1.0f); }
    MeshRenderer*           GetMeshRenderer()       { return meshRenderer.get(); }
    std::shared_ptr<Material> GetMaterial()          { return material; }
};