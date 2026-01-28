module;

#include <memory>
#include <cstddef>
#include <vector>

#include <glad/glad.h>

export module WFE.Rendering.MeshRenderer;

import WFE.Rendering.Core.VertexBuffer;
import WFE.Rendering.Core.VertexArray;
import WFE.Rendering.Core.IndexBuffer;
import WFE.Rendering.MeshData; 
import WFE.Core.Logger;

export class MeshRenderer
{
private:
    std::unique_ptr<VertexArray>  VAO;
    std::unique_ptr<VertexBuffer> VBO;
    std::unique_ptr<IndexBuffer>  EBO;

    size_t indexCount = 0;
    size_t vertexCount = 0;
    bool   usedIndices = false;

public:
    MeshRenderer(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        usedIndices = true;
        indexCount = indices.size();

        Logger::Log(LogLevel::INFO, "MeshRenderer created with " + 
            std::to_string(vertices.size()) + " vertices and " + 
            std::to_string(indexCount) + " indices");

        VAO = std::make_unique<VertexArray>();
        VBO = std::make_unique<VertexBuffer>();
        EBO = std::make_unique<IndexBuffer>();

        VAO->Bind();

        VBO->Bind();
        VBO->SetData(vertices.data(), vertices.size() * sizeof(Vertex), GL_STATIC_DRAW);

        EBO->Bind();
        EBO->SetData(indices.data(), static_cast<unsigned int>(indices.size()), GL_STATIC_DRAW);

        VAO->AddAttribute(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);                          // Position
        VAO->AddAttribute(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, Normal));   // Normal
        VAO->AddAttribute(2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, TexCoords));// TexCoords
        VAO->AddAttribute(3, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, Tangent)); // Tangent
        VAO->AddAttribute(4, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, Bitangent));// Bitangent

        VAO->Unbind();

        Logger::Log(LogLevel::INFO, "MeshRenderer attributes configured successfully");
    }

    MeshRenderer(const float* data, size_t dataSize, int stride)
    {
        usedIndices = false;
        vertexCount = dataSize / (stride * sizeof(float));

        Logger::Log(LogLevel::INFO, "MeshRenderer created from raw data: " + 
            std::to_string(vertexCount) + " vertices, stride=" + std::to_string(stride));

        VAO = std::make_unique<VertexArray>();
        VBO = std::make_unique<VertexBuffer>();

        VBO->SetData(data, dataSize);

        VAO->Bind();
        VBO->Bind();

        VAO->AddAttribute(0, 3, GL_FLOAT, false, stride * sizeof(float), 0);
        VAO->AddAttribute(1, 3, GL_FLOAT, false, stride * sizeof(float), 3 * sizeof(float));
        VAO->AddAttribute(2, 2, GL_FLOAT, false, stride * sizeof(float), 6 * sizeof(float));

        VAO->Unbind();
    }

    void Draw()
    {
        VAO->Bind();
        if (usedIndices)
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
        VAO->Unbind(); 
    }

    size_t GetVertexCount() const { return vertexCount; }
};
