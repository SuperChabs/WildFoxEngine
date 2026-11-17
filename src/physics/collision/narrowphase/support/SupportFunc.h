#pragma once

#include <glm/glm.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "physics/core/PhysicsContext.h"
#include "physics/geometry/ShapeFuncStruckt.h"

namespace Support {
    glm::vec3 Support(const Sphere& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale, float bias);
    glm::vec3 Support(const Box& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale, float bias);
    glm::vec3 Support(const ConvexHull& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale, float bias);

    Point Support(const BodyInfo &bodyA, const BodyInfo &bodyB, glm::vec3 dir, float bias);
}
