#pragma once

#include "physics/core/PhysicsContext.h"
#include "ECS/components/Components.h"

namespace NarrowPhase {

    bool Intersect(BodyInfo &bodyA, BodyInfo &bodyB, ContactInfo &contact, float dt);

    bool Intersect(BodyInfo &bodyA, BodyInfo &bodyB, ContactInfo &contact);

    bool ConservativeAdvance(BodyInfo &bodyA, BodyInfo &bodyB, ContactInfo &contact, float dt);

    bool RaySphere(const glm::vec3 &rayStart, const glm::vec3 &rayDir, const glm::vec3 &sphereCenter,
                  const float& sphereRadius, float &t1, float &t2);

    bool SphereSphereDynamic(const Sphere *shapeA, const Sphere *shapeB, glm::vec3 posA, glm::vec3 posB,
        const glm::vec3 &velA, const glm::vec3 &velB, float dt, glm::vec3 &ptOnA, glm::vec3 &ptOnB, float &toi);

    bool SphereSphereStatic(const Sphere *shapeA, const Sphere *shapeB, glm::vec3 posA, glm::vec3 posB,
        glm::vec3 &ptOnA, glm::vec3 &ptOnB);
}
