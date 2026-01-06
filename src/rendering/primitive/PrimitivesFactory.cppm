module;

#include <string>
#include <vector>
#include <memory>
#include <utility>

export module XEngine.Rendering.Primitive.PrimitivesFactory;

import XEngine.Rendering.GPUMesh;
import XEngine.Rendering.Material;
import XEngine.Rendering.MeshData;
import XEngine.Rendering.Primitive.PrimitiveGenerator;
import XEngine.Scene.Mesh;
import XEngine.Scene.SceneManager;
import XEngine.Scene.Model;
import XEngine.Core.Logger;

export
enum class PrimitiveType
{
    CUBE,
    QUAD,
    PLANE
};

export
class PrimitivesFactory
{
public:
    static Mesh* CreatePrimitive(PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::CUBE:
            {
                size_t dataSize;
                const float* data = PrimitiveGenerator::GetCubeData(dataSize);

                auto gpuMesh = std::make_unique<GPUMesh>(data, dataSize, 8);
                auto material = std::make_unique<Material>(std::vector<Texture>{});

                auto mesh = std::make_unique<Mesh>(gpuMesh.release(), material.release());
                return mesh.release();
            }
            case PrimitiveType::QUAD:
            {
                size_t dataSize;
                const float* data = PrimitiveGenerator::GetQuadData(dataSize);

                auto gpuMesh = std::make_unique<GPUMesh>(data, dataSize, 8);
                auto material = std::make_unique<Material>(std::vector<Texture>{});

                auto mesh = std::make_unique<Mesh>(gpuMesh.release(), material.release());
                return mesh.release();
            }
            case PrimitiveType::PLANE:
            {
                size_t dataSize;
                const float* data = PrimitiveGenerator::GetPlaneData(dataSize);

                auto gpuMesh = std::make_unique<GPUMesh>(data, dataSize, 8);
                auto material = std::make_unique<Material>(std::vector<Texture>{});

                auto mesh = std::make_unique<Mesh>(gpuMesh.release(), material.release());
                return mesh.release();
            }
            default:
                return nullptr;
        }
    }

    static SceneManager* CreatePrimitiveInScene(PrimitiveType type, SceneManager* sceneManager)
    {
        Mesh* primitiveMesh = CreatePrimitive(type);
        if (!primitiveMesh) 
            return sceneManager;

        std::unique_ptr<Model> model = std::make_unique<Model>(primitiveMesh, GetPrimitiveName(type));
        sceneManager->AddObject(GetPrimitiveName(type), std::move(model));

        return sceneManager;
    }

    static std::string GetPrimitiveName(PrimitiveType type)
    {
        switch (type)
        {
            case PrimitiveType::CUBE:  return "Cube";
            case PrimitiveType::QUAD:  return "Quad";
            case PrimitiveType::PLANE: return "Plane";
            default: return "Unknown";
        }
    }
};
