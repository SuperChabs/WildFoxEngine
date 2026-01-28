module;

#include <vector>
#include <string>
#include <memory>
#include <cstddef>

#include <glm/glm.hpp>

export module WFE.Resource.Model.Model;

import WFE.Scene.Mesh;
import WFE.Resource.Shader.ShaderManager;
import WFE.Resource.Material.Material;
import WFE.Rendering.MeshData;

/// @file Model.cppm
/// @brief Model Object Class
/// @author SuperChabs
/// @date 2026-01-28

export class Model
{
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::string filepath;
    std::string name;

public:
    Model(const std::string& path)
        : filepath(path)
    {
        size_t lastSlash = path.find_last_of("/");
        size_t lastDot   = path.find_last_of(".");

        if (lastSlash != std::string::npos && lastDot != std::string::npos)
            name = path.substr(lastSlash + 1, lastDot - lastSlash - 1);
        else
            name = path;
    }

    void AddMesh(Mesh&& mesh)
    {
        meshes.push_back(std::make_shared<Mesh>(std::move(mesh)));
    }

    void AddMesh(std::shared_ptr<Mesh> mesh)
    {
        meshes.push_back(mesh);
    }

    std::shared_ptr<Mesh> GetMesh(size_t index)
    {
        if (index < meshes.size())
            return meshes[index];
        return nullptr;
    }

    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const
    {
        return meshes;
    }

    void Draw(ShaderManager& shaderManager, const std::string& shaderName) 
    {
        for (auto& mesh : meshes) 
            if (mesh)
                mesh->Draw(shaderManager, shaderName);
    }

    // void SetColor(const glm::vec3& color) 
    // {
    //     for (auto& mesh : meshes) 
    //         if (mesh)
    //             mesh->SetColor(color);
    // }

    void SetTextures(const std::vector<Texture>& textures)
    {
        for (auto& mesh : meshes) 
            if (mesh)
                mesh->SetTextures(textures);
    }

    void SetMaterial(std::shared_ptr<Material> material, size_t index)
    {
        if (meshes[index])
            meshes[index]->SetMaterial(material);
    }

    void SetMaterialForAll(std::shared_ptr<Material> material) 
    {
        for (auto& mesh : meshes) 
            if (mesh)
                mesh->SetMaterial(material);
    }
    
    size_t GetMeshCount() const { return meshes.size(); }
    const std::string& GetName() const { return name; }
    const std::string& GetFilepath() const { return filepath; }
    
    void SetName(const std::string& n) { name = n; }
    void SetFilepath(const std::string& path) { filepath = path; }
};