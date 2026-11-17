#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <utility>

#include "scene/Mesh.h"
#include "resource/material/Material.h"

struct c_Mesh {
    std::shared_ptr<Mesh> mesh;

    c_Mesh() = default;

    explicit c_Mesh(std::shared_ptr<Mesh> m) : mesh(std::move(m)) {
    }
};

struct c_Material {
    std::shared_ptr<Material> material;
    glm::vec2 tiling = {1.0f, 1.0f};

    c_Material() = default;

    explicit c_Material(std::shared_ptr<Material> mat) : material(std::move(mat)) {
    }
};

struct c_Visibility {
    bool isActive = true;
    bool visible = true;

    c_Visibility() = default;

    explicit c_Visibility(const bool active) : isActive(active) {
    }
};