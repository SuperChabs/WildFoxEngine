#pragma once
#include <limits>

#include <glm/vec3.hpp>

#include "entt/entity/entity.hpp"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct HitInfo {
    bool hit = false;
    glm::vec3 hitPoint = glm::vec3(0.0f);
    glm::vec3 normal   = glm::vec3(0.0f);
    float distance = std::numeric_limits<float>::max();
};

struct RayCastHit {
    HitInfo hitInfo;
    entt::entity entity = entt::null;
};
