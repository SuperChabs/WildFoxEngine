#pragma once

#include <glm/fwd.hpp>

#include "ECS/components/Physics.h"
#include "physics/core/PhysicsContext.h"

namespace MassProperties {
    glm::mat3 InertiaTensor(const Sphere& s);
    glm::mat3 InertiaTensor(const Box& s);
    glm::mat3 InertiaTensor(const ConvexHull& s);

    glm::mat3 GetInverseInertiaTensorBodySpace(const glm::mat3 &inertia, const float& invMass);
    glm::mat3 GetInverseInertiaTensorWorldSpace(const BodyInfo &info);
}
