#pragma once
#include "physics/core/PhysicsContext.h"

namespace ContactSolver {
    void ResolveContact(ContactInfo &contact);

    void ApplyImpulse(const BodyInfo &body, const glm::vec3 &impulsePoint, const glm::vec3 &impulse);
    void ApplyImpulseLinear(c_RigidBody &rb,  const glm::vec3 &impulse);
    void ApplyImpulseAngular(const BodyInfo &body, const glm::vec3 &impulse);
}
