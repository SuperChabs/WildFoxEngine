module;

#include <string>
#include <memory>
#include <utility>

#include <glm/glm.hpp>

export module WFE.Rendering.Primitive.PrimitivesFactory;

import WFE.Rendering.MeshRenderer;
import WFE.Resource.Material.Material;
import WFE.Rendering.MeshData;
import WFE.Rendering.Primitive.PrimitiveGenerator;

import WFE.Scene.Mesh;

import WFE.Core.Logger;

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

                auto meshRenderer = std::make_unique<MeshRenderer>(data, dataSize, 8);
                auto material = std::make_unique<Material>(glm::vec3(1.0f, 0.0f, 1.0f));

                auto mesh = std::make_unique<Mesh>(meshRenderer.release(), material.release());
                return mesh.release();
            }
            case PrimitiveType::QUAD:
            {
                size_t dataSize;
                const float* data = PrimitiveGenerator::GetQuadData(dataSize);

                auto meshRenderer = std::make_unique<MeshRenderer>(data, dataSize, 8);
                auto material = std::make_unique<Material>(glm::vec3(1.0f, 1.0f, 1.0f));

                auto mesh = std::make_unique<Mesh>(meshRenderer.release(), material.release());
                return mesh.release();
            }
            case PrimitiveType::PLANE:
            {
                size_t dataSize;
                const float* data = PrimitiveGenerator::GetPlaneData(dataSize);

                auto meshRenderer = std::make_unique<MeshRenderer>(data, dataSize, 8);
                auto material = std::make_unique<Material>(glm::vec3(1.0f, 1.0f, 1.0f));

                auto mesh = std::make_unique<Mesh>(meshRenderer.release(), material.release());
                return mesh.release();
            }
            default:
                return nullptr;
        }
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
