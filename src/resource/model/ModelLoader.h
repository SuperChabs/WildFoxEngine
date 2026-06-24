#pragma once

#include <string>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/matrix4x4.h>

#include "resource/model/Model.h"
#include "resource/model/ModelNode.h"
#include "scene/Mesh.h"
#include "rendering/MeshData.h"
#include "resource/material/MaterialManager.h"
#include "ECS/World.h"

/// @file ModelLoader.cppm
/// @brief Models mesh loader
/// @author SuperChabs
/// @date 2026-01-28

static std::unordered_map<std::string, unsigned int> loadedTexturesCache;
static int globalMeshCounter = 0;

std::pair<Model *, entt::entity> LoadModelFromFile(
    std::string & path,
    MaterialManager & materialManager,
    ECSWorld * world = nullptr,
    const bool isBaseShape = false);

std::shared_ptr<ModelNode> ProcessNode(
    aiNode *node,
    const aiScene *scene,
    Model *model,
    const std::string &directory,
    MaterialManager &materialManager,
    ECSWorld *world = nullptr,
    entt::entity parentEntity = entt::null,
    bool isBaseShape = false
);

std::shared_ptr<Mesh> ProcessMesh(
    aiMesh *mesh,
    const aiScene *scene,
    const std::string &directory,
    MaterialManager &materialManager,
    int meshIndex,
    bool isBaseShape = false
);

std::vector<Texture> LoadMaterialTextures(
    aiMaterial *mat,
    aiTextureType type,
    const std::string &typeName,
    const std::string &directory
);

unsigned int TextureFromFile(const char *path, const std::string &directory);

void DecomposeTransform(
    const glm::mat4 &transform,
    glm::vec3 &position,
    glm::vec3 &rotation,
    glm::vec3 &scale
);

inline glm::mat4 ConvertAssimpMatrix(const aiMatrix4x4 from) {
    glm::mat4 to;
    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
}
