#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

struct ContactInfo {
    entt::entity a;
    entt::entity b;

    glm::vec3 ptOnA_WorldSpace;
    glm::vec3 ptOnB_WorldSpace;
    glm::vec3 ptOnA_LocalSpace;
    glm::vec3 ptOnB_LocalSpace;

    glm::vec3 normal;
    float separationDistance;
    float timeOfImpact;
};

struct PseudoBody {
    entt::entity id;
    float value;
    bool isMin;
};