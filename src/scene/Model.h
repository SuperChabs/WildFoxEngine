#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "scene/Mesh.h"
#include "core/Shader.h"
#include "rendering/TextureManager.h"

#include <vector>
#include <memory>
#include <string>
#include <utility>

class Model 
{
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

public:
    Model(const char* path) 
    {
        loadModel(path);
    }

    Model(Mesh* mesh, std::string name = "") 
    { 
        meshes.push_back(std::move(*mesh)); 
    }

    void Draw(Shader &shader);

    void SetColor(const glm::vec3& color);
    void SetTextures(const std::vector<Texture>& textures);

private:
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif