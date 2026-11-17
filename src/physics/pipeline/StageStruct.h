#pragma once

#include <glm/vec3.hpp>

#include "physics/collision/narrowphase/manifold/ManifoldCollector.h"
#include "physics/collision/narrowphase/support/constraint/Constraint.h"
#include "physics/collision/narrowphase/support/constraint/ConstraintPenetration.h"
#include "physics/core/PhysicsContext.h"

struct StepContext {
    float dt;
    glm::vec3 gravity;
    int numContacts = 0;

    std::vector<BodyInfo> bodies;
    std::vector<CollisionPair> pairs;
    std::vector<ContactInfo> contacts;
    std::vector<std::unique_ptr<Constraint>> constraints;
    std::vector<ConstraintPenetration> constraintPenetrations;
    ManifoldCollector manifolds;
    //std::vector<TriggerPair> triggers;

    void Clear() {
        pairs.clear();
        contacts.clear();
        constraints.clear();
        constraintPenetrations.clear();
        numContacts = 0;
        // triggers.clear();
    }
};
