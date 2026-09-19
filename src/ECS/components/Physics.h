#pragma once

#include <variant>
#include <glm/glm.hpp>

#include "physics/Bounds.h"

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    [[nodiscard]] static glm::mat3 InertiaTensor() { return {1.0f}; }
    [[nodiscard]] Bounds GetBounds(const glm::vec3 &pos, const glm::quat &rot) const {
        Bounds tmp;
        return tmp;
    }

    [[nodiscard]] Bounds GetBounds() const {
        Bounds tmp;
        return tmp;
    }
};

struct Sphere
{
    glm::vec3 m_center;
    float m_radius;

    [[nodiscard]] glm::mat3 InertiaTensor() const {
        return {2.0f * m_radius * m_radius / 0.5f};
    }

    [[nodiscard]] Bounds GetBounds(const glm::vec3 &pos, const glm::quat &rot) const {
        Bounds tmp;
        tmp.SetMaxs(glm::vec3(m_radius) + pos);
        tmp.SetMins(glm::vec3(-m_radius) + pos);
        return tmp;
    };

    [[nodiscard]] Bounds GetBounds() const {
        Bounds tmp;
        tmp.SetMaxs(glm::vec3(m_radius));
        tmp.SetMins(glm::vec3(-m_radius));
        return tmp;
    }
};

struct ColliderComponent {
    std::variant<AABB, Sphere> shape;
    bool isTrigger = false;

    [[nodiscard]] static glm::mat3 GetInertiaTensor(const ColliderComponent &c) {
        return std::visit(
             [](const auto &shape){
                 return shape.InertiaTensor();
             },
             c.shape
         );
    }

    [[nodiscard]] Bounds GetBounds(const glm::vec3 &pos, const glm::quat &rot) {
        return std::visit(
             [&pos, &rot](const auto &shape){
                 return shape.GetBounds(pos, rot);
             },
             shape
         );
    }

    [[nodiscard]] Bounds GetBounds() {
        return std::visit(
             [](const auto &shape){
                 return shape.GetBounds();
             },
             shape
         );
    }

    glm::vec3 m_centerOfMass;
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