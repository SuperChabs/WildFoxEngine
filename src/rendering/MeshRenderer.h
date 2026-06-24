#pragma once

#include <string>
#include <memory>
#include <vector>

#include <glad/glad.h>

#include "rendering/core/VertexBuffer.h"
#include "rendering/core/VertexArray.h"
#include "rendering/core/IndexBuffer.h"
#include "MeshData.h"

class MeshRenderer {
private:
    std::unique_ptr<VertexArray> VAO;
    std::unique_ptr<VertexBuffer> VBO;
    std::unique_ptr<IndexBuffer> EBO;

    size_t indexCount = 0;
    size_t vertexCount = 0;
    bool usedIndices = false;

public:
    MeshRenderer(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);

    MeshRenderer(const float *data, size_t dataSize, int stride);

    void Draw();

    size_t GetVertexCount() const;
};