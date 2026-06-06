#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "rendering/MeshRenderer.h"
#include "resource/material/Material.h"
#include "PrimitiveGenerator.h"
#include "scene/Mesh.h"

enum class PrimitiveType {
    CUBE,
    QUAD,
    PLANE,
    NONE
};

class PrimitivesFactory {
public:
    static Mesh *CreatePrimitive(PrimitiveType type) {
        switch (type) {
            case PrimitiveType::CUBE: {
                size_t dataSize;
                const float *data = PrimitiveGenerator::GetCubeData(dataSize);

                auto meshRenderer = std::make_unique<MeshRenderer>(data, dataSize, 8);
                auto material = std::make_unique<Material>(glm::vec3(1.0f, 0.0f, 1.0f));

                auto mesh = std::make_unique<Mesh>(meshRenderer.release(), material.release());
                return mesh.release();
            }
            case PrimitiveType::QUAD: {
                size_t dataSize;
                const float *data = PrimitiveGenerator::GetQuadData(dataSize);

                auto meshRenderer = std::make_unique<MeshRenderer>(data, dataSize, 8);
                auto material = std::make_unique<Material>(glm::vec3(1.0f, 1.0f, 1.0f));

                auto mesh = std::make_unique<Mesh>(meshRenderer.release(), material.release());
                return mesh.release();
            }
            case PrimitiveType::PLANE: {
                size_t dataSize;
                const float *data = PrimitiveGenerator::GetPlaneData(dataSize);

                auto meshRenderer = std::make_unique<MeshRenderer>(data, dataSize, 8);
                auto material = std::make_unique<Material>(glm::vec3(1.0f, 1.0f, 1.0f));

                auto mesh = std::make_unique<Mesh>(meshRenderer.release(), material.release());
                return mesh.release();
            }
            default:
                return nullptr;
        }
    }

    static std::string GetPrimitiveName(PrimitiveType type) {
        switch (type) {
            case PrimitiveType::CUBE: return "Cube";
            case PrimitiveType::QUAD: return "Quad";
            case PrimitiveType::PLANE: return "Plane";
            default: return "Unknown";
        }
    }
};