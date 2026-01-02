#include "rendering/primitive/PrimitivesFactory.h"
#include "rendering/primitive/PrimitiveGenerator.h"
#include "scene/mesh.h"

#include <memory>
#include <utility>

Mesh* PrimitivesFactory::CreatePrimitive(PrimitiveType type)
{
    switch (type)
    {
        case PrimitiveType::CUBE:
        {
            size_t dataSize;
            const float* data = PrimitiveGenerator::GetCubeData(dataSize);

            std::unique_ptr<GPUMesh> gpuMesh = std::make_unique<GPUMesh>(data, dataSize, 8);

            std::vector<Texture> textures; 
            std::unique_ptr<Material> material = std::make_unique<Material>(textures);

            std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(gpuMesh.release(), material.release());

            return mesh.release();
        }
        case PrimitiveType::QUAD:
        {
            size_t dataSize;
            const float* data = PrimitiveGenerator::GetQuadData(dataSize);

            std::unique_ptr<GPUMesh> gpuMesh = std::make_unique<GPUMesh>(data, dataSize, 8);

            std::vector<Texture> textures; 
            std::unique_ptr<Material> material = std::make_unique<Material>(textures);

            std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(gpuMesh.release(), material.release());

            return mesh.release();
        }
        case PrimitiveType::PLANE:
        {
            size_t dataSize;
            const float* data = PrimitiveGenerator::GetPlaneData(dataSize);

            std::unique_ptr<GPUMesh> gpuMesh = std::make_unique<GPUMesh>(data, dataSize, 8);

            std::vector<Texture> textures; 
            std::unique_ptr<Material> material = std::make_unique<Material>(textures);

            std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(gpuMesh.release(), material.release());

            return mesh.release();
        }
        default:
            return nullptr;
    }
}

SceneManager* PrimitivesFactory::CreatePrimitiveInScene(PrimitiveType type, SceneManager* sceneManager) 
{
    Mesh* primitiveMesh = CreatePrimitive(type);
    if (!primitiveMesh) 
        return sceneManager;

    std::unique_ptr<Model> model = std::make_unique<Model>(primitiveMesh, GetPrimitiveName(type));
    sceneManager->AddObject(GetPrimitiveName(type), std::move(model));

    return sceneManager;
}

std::string PrimitivesFactory::GetPrimitiveName(PrimitiveType type) 
{
    switch (type)
    {
        case PrimitiveType::CUBE:
            return "Cube";
        case PrimitiveType::QUAD:
            return "Quad";
        case PrimitiveType::PLANE:
            return "Plane";
        default:
            return "Unknown";
    }
}