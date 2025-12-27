#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "scene/Mesh.h"
#include "core/Shader.h"

#include <vector>

using namespace std;

class Model 
{
public:
    //
    vector<Mesh>    meshes;
    vector<Texture> textures_loaded;
    string          directory;


    Model(const char* path) {loadModel(path);}
    void Draw(Shader &shader);

private:
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif