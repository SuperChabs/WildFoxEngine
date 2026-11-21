#pragma once

#include <glm/glm.hpp>

#include "ECS/components/Components.h"

struct BodyInfo {
    c_RigidBody *rigidBody;
    c_Transform *transform;
    c_Collider  *collider;

    entt::entity id = entt::null;

    BodyInfo &operator=(const BodyInfo &b) {
        if (this != &b) {
            rigidBody = b.rigidBody;
            transform = b.transform;
            collider = b.collider;
            id = b.id;
        }

        return *this;
    }
};

struct ContactInfo {
    BodyInfo *bodyA;
    BodyInfo *bodyB;

    glm::vec3 ptOnA_WorldSpace;
    glm::vec3 ptOnB_WorldSpace;
    glm::vec3 ptOnA_LocalSpace;
    glm::vec3 ptOnB_LocalSpace;

    glm::vec3 normal;
    float separationDistance;
    float timeOfImpact;
};

struct PseudoBody {
    BodyInfo *id;
    float value;
    bool isMin;
};

struct CollisionPair {
    BodyInfo *bodyA;
    BodyInfo *bodyB;
};

// stored by entity id, because BodyInfo pointers are invalidated when GatherStage rebuilds ctx.bodies
struct TriggerPair {
    entt::entity a;
    entt::entity b;

    bool operator==(const TriggerPair &t) const {
        return (a == t.a && b == t.b) || (a == t.b && b == t.a);
    }
};