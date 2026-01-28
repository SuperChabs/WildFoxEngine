// src/resource/model/ModelLoader.cppm
module;

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

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
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Material.Material;

/// @file ModelLoader.cppm
/// @brief Models mesh loader
/// @author SuperChabs
/// @date 2026-01-28

static std::unordered_map<std::string, unsigned int> loadedTexturesCache;
static int globalMeshCounter = 0;

void ProcessNode(aiNode* node, const aiScene* scene, Model* model, 
                 const std::string& directory, MaterialManager& materialManager);
std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, 
                 const std::string& directory, MaterialManager& materialManager, int meshIndex);
std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, 
                                          const std::string& typeName, const std::string& directory);
unsigned int TextureFromFile(const char* path, const std::string& directory);

/**
 * @brief Model loader function
 */
export Model* LoadModelFromFile(const std::string& path, MaterialManager& materialManager) 
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
    
    globalMeshCounter = 0;
    
    ProcessNode(scene->mRootNode, scene, model, directory, materialManager);
    
    Logger::Log(LogLevel::INFO, 
        "Model loaded: " + path + " (" + 
        std::to_string(model->GetMeshCount()) + " meshes)");
    
    loadedTexturesCache.clear();
    
    return model;
}

void ProcessNode(aiNode* node, const aiScene* scene, Model* model, 
                 const std::string& directory, MaterialManager& materialManager)
{
    static int meshCounter = 0;
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::shared_ptr<Mesh> processedMesh = ProcessMesh(mesh, scene, directory, 
                                                           materialManager, meshCounter++);
        model->AddMesh(processedMesh);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++) 
    {
        ProcessNode(node->mChildren[i], scene, model, directory, materialManager);
    }
}

std::shared_ptr<Mesh> ProcessMesh(
    aiMesh* mesh, const aiScene* scene,
    const std::string& directory,
    MaterialManager& materialManager,
    int meshIndex)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Logger::Log(LogLevel::INFO, "=== Processing Mesh #" + std::to_string(meshIndex) + " ===");

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal   = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0.0f,1.0f,0.0f);
        vertex.TexCoords = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f);
        vertex.Tangent   = mesh->HasTangentsAndBitangents() ? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z) : glm::vec3(0.0f);
        vertex.Bitangent = mesh->HasTangentsAndBitangents() ? glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z) : glm::vec3(0.0f);
        vertices.push_back(vertex);
    }
    Logger::Log(LogLevel::INFO, "Vertices processed: " + std::to_string(vertices.size()));

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    Logger::Log(LogLevel::INFO, "Indices processed: " + std::to_string(indices.size()));

    std::shared_ptr<Material> meshMaterial;
    Logger::Log(LogLevel::INFO, "Material index: " + std::to_string(mesh->mMaterialIndex));

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiString matName;
        material->Get(AI_MATKEY_NAME, matName);
        std::string materialName = std::string(matName.C_Str()) + "_mesh" + std::to_string(meshIndex);

        Logger::Log(LogLevel::INFO, "Material name from model: " + std::string(matName.C_Str()));
        Logger::Log(LogLevel::INFO, "Generated material name: " + materialName);

        Logger::Log(LogLevel::INFO, "Loading DIFFUSE textures...");
        std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
        Logger::Log(LogLevel::INFO, "Found " + std::to_string(diffuseMaps.size()) + " diffuse textures");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        Logger::Log(LogLevel::INFO, "Loading SPECULAR textures...");
        std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
        Logger::Log(LogLevel::INFO, "Found " + std::to_string(specularMaps.size()) + " specular textures");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        Logger::Log(LogLevel::INFO, "Loading NORMAL textures...");
        std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", directory);
        Logger::Log(LogLevel::INFO, "Found " + std::to_string(normalMaps.size()) + " normal textures");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        Logger::Log(LogLevel::INFO, "Loading HEIGHT textures...");
        std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height", directory);
        Logger::Log(LogLevel::INFO, "Found " + std::to_string(heightMaps.size()) + " height textures");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        Logger::Log(LogLevel::INFO, "Total textures loaded: " + std::to_string(textures.size()));

        if (!textures.empty())
        {
            Logger::Log(LogLevel::INFO, "Creating TEXTURE material with " + std::to_string(textures.size()) + " textures");
            for (size_t i = 0; i < textures.size(); i++)
            {
                Logger::Log(LogLevel::INFO, "  Texture " + std::to_string(i) + ": " + textures[i].path + " (ID: " + std::to_string(textures[i].id) + ", type: " + textures[i].type + ")");
            }
            meshMaterial = std::make_shared<Material>(textures, materialName);
            Logger::Log(LogLevel::INFO, "Texture material created successfully");
        }
        else
        {
            aiColor3D color(0.8f, 0.8f, 0.8f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            glm::vec3 matColor(color.r, color.g, color.b);

            Logger::Log(LogLevel::INFO, "No textures found. Creating COLOR material with color: (" + std::to_string(matColor.r) + ", " + std::to_string(matColor.g) + ", " + std::to_string(matColor.b) + ")");
            meshMaterial = std::make_shared<Material>(matColor, materialName);
            Logger::Log(LogLevel::INFO, "Color material created successfully");
        }
    }
    else
    {
        Logger::Log(LogLevel::WARNING, "No material index found, using default material");
        meshMaterial = materialManager.GetMaterial("default");
    }

    Logger::Log(LogLevel::INFO, "Creating Mesh object...");
    auto createdMesh = std::make_shared<Mesh>(vertices, indices, textures);
    Logger::Log(LogLevel::INFO, "Mesh object created");

    if (meshMaterial)
    {
        Logger::Log(LogLevel::INFO, "Setting material to mesh...");
        createdMesh->SetMaterial(meshMaterial);
        Logger::Log(LogLevel::INFO, "✓ Material successfully set to mesh! (textures: " + std::to_string(textures.size()) + ")");

        auto checkMaterial = createdMesh->GetMaterial();
        if (checkMaterial)
        {
            Logger::Log(LogLevel::INFO, "✓ Verified: Material is set (name: " + checkMaterial->GetName() + ", type: " + checkMaterial->GetType() + ", using color: " + (checkMaterial->IsUsingColor() ? "YES" : "NO") + ")");
        }
        else
        {
            Logger::Log(LogLevel::ERROR, "✗ ERROR: Material is NULL after SetMaterial!");
        }
    }
    else
    {
        Logger::Log(LogLevel::ERROR, "✗ ERROR: meshMaterial is NULL, cannot set material!");
    }

    Logger::Log(LogLevel::INFO, "=== Mesh #" + std::to_string(meshIndex) + " processing complete ===\n");

    return createdMesh;
}


std::vector<Texture> LoadMaterialTextures(
    aiMaterial* mat, aiTextureType type, 
    const std::string& typeName, const std::string& directory)
{
    std::vector<Texture> textures;
    
    unsigned int textureCount = mat->GetTextureCount(type);
    Logger::Log(LogLevel::DEBUG, 
        "Loading " + std::to_string(textureCount) + " textures of type: " + typeName);
    
    for (unsigned int i = 0; i < textureCount; i++) 
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        std::string texturePath = std::string(str.C_Str());
        
        bool skip = false;
        unsigned int textureID = 0;
        
        std::string fullPath = directory + "/" + texturePath;
        
        if (loadedTexturesCache.find(fullPath) != loadedTexturesCache.end())
        {
            textureID = loadedTexturesCache[fullPath];
            skip = true;
            Logger::Log(LogLevel::DEBUG, 
                "Using cached texture: " + texturePath);
        }
        
        if (!skip)
        {
            textureID = TextureFromFile(str.C_Str(), directory);
            if (textureID != 0)
            {
                loadedTexturesCache[fullPath] = textureID;
                Logger::Log(LogLevel::INFO, 
                    "Loaded texture: " + texturePath + " (ID: " + std::to_string(textureID) + ")");
            }
        }
        
        if (textureID != 0)
        {
            Texture texture;
            texture.id = textureID;
            texture.type = typeName;
            texture.path = texturePath;
            
            textures.push_back(texture);
        }
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
        
        Logger::Log(LogLevel::INFO, 
            "Texture loaded successfully: " + filename + 
            " (" + std::to_string(width) + "x" + std::to_string(height) + 
            ", " + std::to_string(nrComponents) + " channels)");
    }
    else
    {
        Logger::Log(LogLevel::ERROR, "Failed to load texture: " + filename);
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}