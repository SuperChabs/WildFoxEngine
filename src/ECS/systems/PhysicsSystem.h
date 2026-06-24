#pragma once

#include <vector>
#include <variant>
#include <set>
#include <utility>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ECS/components/Components.h"
#include "ECS/World.h"

struct ContactInfo {
    entt::entity a;
    entt::entity b;
    glm::vec3 normal;
    float depth;
};

class PhysicsSystem {
    glm::vec3 gravity = {0, -9.8, 0};
    std::vector<entt::entity> entities;

    std::set<std::pair<entt::entity, entt::entity> > m_activeTriggers;

public:
    void Update(ECSWorld &world, float dt);

    glm::vec3 GetGravity();

    void SetGravity(glm::vec3 newGravity);

private:
    bool TestAABB(const AABB &a, const AABB &b, ContactInfo &contact);
};
