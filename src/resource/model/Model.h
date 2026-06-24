#pragma once

#include <string>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "scene/Mesh.h"
#include "resource/shader/ShaderManager.h"
#include "resource/material/Material.h"
#include "resource/model/ModelNode.h"
#include "rendering/MeshData.h"

/// @file Model.cppm
/// @brief Model Object Class
/// @author SuperChabs
/// @date 2026-01-28

class Model {
    std::vector<std::shared_ptr<Mesh> > meshes;
    std::shared_ptr<ModelNode> rootNode;

    std::string filepath;
    std::string name;

public:
    Model(const std::string &path);

    void AddMesh(Mesh &&mesh);
    void AddMesh(std::shared_ptr<Mesh> mesh);

    void Draw(ShaderManager &shaderManager, const std::string &shaderName);

    // void SetColor(const glm::vec3& color) 
    // {
    //     for (auto& mesh : meshes) 
    //         if (mesh)
    //             mesh->SetColor(color);
    // }

    void SetTextures(const std::vector<Texture> &textures);
    void SetMaterial(std::shared_ptr<Material> material, size_t index);
    void SetMaterialForAll(std::shared_ptr<Material> material);
    void SetName(const std::string &n);
    void SetFilepath(const std::string &path);
    void SetRootNode(std::shared_ptr<ModelNode> node);

    size_t GetMeshCount() const;
    const std::string &GetName() const;
    const std::string &GetFilepath() const;
    std::shared_ptr<ModelNode> GetRootNode() const;
    std::shared_ptr<Mesh> GetMesh(size_t index);
    const std::vector<std::shared_ptr<Mesh> > &GetMeshes() const;
};