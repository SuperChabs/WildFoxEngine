#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>

#include "core/Shader.h"
#include "rendering/Material.h"
#include "rendering/GPUMesh.h"

class Mesh 
{
private:
    std::unique_ptr<GPUMesh> gpuMesh;
    std::unique_ptr<Material> material;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(Shader& shader);
};

#endif