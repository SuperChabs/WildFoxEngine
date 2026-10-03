#pragma once

#include <variant>
#include <glm/glm.hpp>

#include "physics/Bounds.h"
#include "physics/shapeFunc/ShapeFunctions.h"

struct BaseShape {
    glm::vec3 m_centerOfMass = {0.0f, 0.0f, 0.0f};
};

struct AABB : BaseShape {
    glm::vec3 min = {0.0f, 0.0f, 0.0f};
    glm::vec3 max = {0.0f, 0.0f, 0.0f};
};

struct Box : BaseShape {
    std::vector<glm::vec3> m_points;
    Bounds m_bounds;
};

struct Sphere : BaseShape {
    glm::vec3 m_center = {0.0f, 0.0f, 0.0f};
    float m_radius;
};

struct ConvexHull : BaseShape {
    std::vector<glm::vec3> m_points;
    Bounds m_bounds;
    glm::mat3 m_inertiaTensor = glm::mat3(0.0f);

    ConvexHull(const std::vector<glm::vec3> &points, int num) {
        ssf::Build(this, points, num);
    }
};

struct ColliderComponent {
    std::variant<AABB, Sphere, ConvexHull, Box> shape;
    bool isTrigger = false;

    [[nodiscard]] glm::mat3 GetInertiaTensor() const {
        return std::visit([](const auto& s){ return Shape::InertiaTensor(s); }, shape);
    }

    [[nodiscard]] Bounds GetBounds(const glm::vec3 &pos, const glm::quat &rot) {
        return std::visit([&](const auto& s) { return Shape::GetBounds(s, pos, rot); }, shape);
    }

    [[nodiscard]] Bounds GetBounds() {
        return std::visit([](const auto& s) { return Shape::GetBounds(s); }, shape);
    }

    [[nodiscard]] glm::vec3 Support(const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, float bias) {
        return std::visit([&](const auto& s){ return Shape::Support(s, dir, pos, rot, bias); }, shape);
    }
};

struct RigidBodyComponent {
    float m_invMass;
    float m_elasticity;
    float m_friction;
    glm::vec3 m_linearVelocity;
    glm::vec3 m_angularVelocity;
    glm::vec3 m_forceAccum;
    glm::vec3 m_torqueAccum;
};