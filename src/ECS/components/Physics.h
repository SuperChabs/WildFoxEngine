#pragma once

#include <variant>
#include <glm/glm.hpp>

#include "physics/geometry/Bounds.h"
#include "physics/geometry/shapes/convex/ConvexStruct.h"
#include "physics/queries/RayStruct.h"

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
    float m_radius = 1.0f;
};

struct ConvexHull : BaseShape {
    std::vector<glm::vec3> m_points;
    std::vector<Tri> m_tris;
    Bounds m_bounds;
    glm::mat3 m_inertiaTensor = glm::mat3(0.0f);

    ConvexHull() = default;
    ConvexHull(const std::vector<glm::vec3> &points, int num);
};

struct c_Collider {
    std::variant<Sphere, ConvexHull, Box> shape;
    bool isTrigger = false;

    [[nodiscard]] glm::mat3 GetInertiaTensor() const;
    Bounds GetBounds(const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale);
    Bounds GetBounds();
    glm::vec3 Support(const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale, float bias);
    [[nodiscard]] float FastestLinearSpeed(const glm::vec3 &angularVelocity, const glm::vec3 &dir) const;
    bool GenerateConvexHull(const std::vector<glm::vec3> &points, int maxVerts = 256);
    glm::vec3 GetCenterOfMass();
};

struct c_RigidBody {
    float m_invMass = 0.0f;
    float m_elasticity = 0.0f;
    float m_friction = 0.5f;
    glm::vec3 m_linearVelocity = glm::vec3(0.0f);
    glm::vec3 m_angularVelocity = glm::vec3(0.0f);
    glm::vec3 m_forceAccum = glm::vec3(0.0f);
    glm::vec3 m_torqueAccum = glm::vec3(0.0f);
};