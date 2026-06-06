#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "scene/Mesh.h"
#include "resource/material/Material.h"
#include "rendering/primitive/PrimitivesFactory.h"

struct MeshComponent {
    std::shared_ptr<Mesh> mesh;
    PrimitiveType type;

    MeshComponent() = default;

    MeshComponent(Mesh *m) : mesh(m) {
    }

    MeshComponent(Mesh *m, PrimitiveType t) : mesh(m), type(t) {
    }

    MeshComponent(std::shared_ptr<Mesh> m) : mesh(m) {
    }
};

struct MaterialComponent {
    std::shared_ptr<Material> material;
    glm::vec2 tiling = {1.0f, 1.0f};

    MaterialComponent() = default;

    MaterialComponent(std::shared_ptr<Material> mat) : material(mat) {
    }
};

struct ColorComponent {
    glm::vec3 color{1.0f};

    ColorComponent() = default;

    ColorComponent(const glm::vec3 &col) : color(col) {
    }

    ColorComponent(float r, float g, float b) : color(r, g, b) {
    }
};

struct VisibilityComponent {
    bool isActive = true;
    bool visible = true;

    VisibilityComponent() = default;

    VisibilityComponent(bool active) : isActive(active) {
    }
};