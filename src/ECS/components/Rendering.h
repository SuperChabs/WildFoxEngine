#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <utility>

#include "scene/Mesh.h"
#include "resource/material/Material.h"

struct MeshComponent {
    std::shared_ptr<Mesh> mesh;

    MeshComponent() = default;

    explicit MeshComponent(Mesh *m) : mesh(m) {}
    explicit MeshComponent(std::shared_ptr<Mesh> m) : mesh(std::move(m)) {}
};

struct MaterialComponent {
    std::shared_ptr<Material> material;
    glm::vec2 tiling = {1.0f, 1.0f};

    MaterialComponent() = default;

    explicit MaterialComponent(std::shared_ptr<Material> mat) : material(std::move(mat)) {}
};

struct ColorComponent {
    glm::vec3 color{1.0f};

    ColorComponent() = default;

    explicit ColorComponent(const glm::vec3 &col) : color(col) {}

    ColorComponent(const float r, const float g, const float b) : color(r, g, b) {}
};

struct VisibilityComponent {
    bool isActive = true;
    bool visible = true;

    VisibilityComponent() = default;

    explicit VisibilityComponent(const bool active) : isActive(active) {}
};