// src/resource/model/ModelLoader.cppm
module;

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

export module WFE.Resource.Model.ModelLoader;

import WFE.Resource.Model.Model;
import WFE.Scene.Mesh;
import WFE.Rendering.MeshData;
import WFE.Core.Logger;

/// @file ModelLoader.cppm
/// @brief Models mesh loader
/// @author SuperChabs
/// @date 2026-01-28

void ProcessNode(aiNode* node, const aiScene* scene, Model* model, const std::string& directory);
Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, 
                                          const std::string& typeName, const std::string& directory);
unsigned int TextureFromFile(const char* path, const std::string& directory);

/**
 * @brief Model loader function
 */
export Model* LoadModelFromFile(const std::string& path) 
{
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_FlipUVs | 
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        Logger::Log(LogLevel::ERROR, 
            "Assimp error: " + std::string(importer.GetErrorString()));
        return nullptr;
    }
    
    std::string directory = path.substr(0, path.find_last_of('/'));
    
    Model* model = new Model(path);
    ProcessNode(scene->mRootNode, scene, model, directory);
    
    Logger::Log(LogLevel::INFO, 
        "Model loaded: " + path + " (" + 
        std::to_string(model->GetMeshCount()) + " meshes)");
    
    return model;
}

void ProcessNode(aiNode* node, const aiScene* scene, Model* model, const std::string& directory)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh processedMesh = ProcessMesh(mesh, scene, directory);
        model->AddMesh(std::move(processedMesh));
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) 
    {
        ProcessNode(node->mChildren[i], scene, model, directory);
    }
}

Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
    {
        Vertex vertex;

        vertex.Position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals()) 
        {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        } 
        else 
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (mesh->mTextureCoords[0]) 
        {
            vertex.TexCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        } 
        else 
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) 
        {
            vertex.Tangent = glm::vec3(
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z
            );
            
            vertex.Bitangent = glm::vec3(
                mesh->mBitangents[i].x,
                mesh->mBitangents[i].y,
                mesh->mBitangents[i].z
            );
        } 
        else 
        {
            vertex.Tangent = glm::vec3(0.0f);
            vertex.Bitangent = glm::vec3(0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) 
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        std::vector<Texture> diffuseMaps = LoadMaterialTextures(
            material, aiTextureType_DIFFUSE, "texture_diffuse", directory
        );
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        
        std::vector<Texture> specularMaps = LoadMaterialTextures(
            material, aiTextureType_SPECULAR, "texture_specular", directory
        );
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> LoadMaterialTextures(
    aiMaterial* mat, aiTextureType type, 
    const std::string& typeName, const std::string& directory)
{
    std::vector<Texture> textures;
    
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        
        textures.push_back(texture);
    }
    
    return textures;
}

unsigned int TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = directory + '/' + std::string(path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    
    if (data) 
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        Logger::Log(LogLevel::ERROR, "Failed to load texture: " + filename);
        stbi_image_free(data);
    }

    return textureID;
}