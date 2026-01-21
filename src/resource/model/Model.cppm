module;

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

export module WildFoxEngine.Scene.Model;

import WildFoxEngine.Scene.Mesh;

import WildFoxEngine.Rendering.MeshData;
import WildFoxEngine.Rendering.Material;

import WildFoxEngine.Resource.Shader.ShaderManager;

export class Model 
{
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(void* mat, int type, const std::string& typeName);

public:
    Model(const char* path) 
    {
        loadModel(path);
    }

    Model(Mesh* mesh, std::string name = "") 
    { 
        meshes.push_back(std::move(*mesh)); 
    }

    void Draw(ShaderManager& shaderManager, const std::string& name);

    void SetColor(const glm::vec3& color);
    void SetTextures(const std::vector<Texture>& textures);

    void SetMaterialForAll(std::shared_ptr<Material> material)
    {
        for (auto& mesh : meshes)
            mesh.SetMaterial(material);
    }
    
    size_t GetMeshCount() const
    {
        return meshes.size();
    }

    Mesh* GetMesh(size_t index)
    {
        if (index < meshes.size())
            return &meshes[index];
        return nullptr;
    }
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);