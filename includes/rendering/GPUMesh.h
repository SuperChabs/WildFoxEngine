#ifndef GPUMESH_H
#define GPUMESH_H

#include <vector>
#include <memory>

#include "rendering/Buffer.h"
#include "rendering/MeshData.h"


class GPUMesh 
{
private:
    std::unique_ptr<VertexArray>   VAO;
    std::unique_ptr<VertexBuffer>  VBO;
    std::unique_ptr<IndexBuffer>   EBO;

    size_t indexCount;

public:
    GPUMesh(const std::vector<Vertex>& vertices, 
            const std::vector<unsigned int>& indices);

    void Draw();
};

#endif