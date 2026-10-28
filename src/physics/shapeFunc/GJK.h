#pragma once
#include "ECS/World.h"

namespace GJK {
    bool DoesIntersect(ECSWorld &ecs, entt::entity entityA, entt::entity entityB, float bias, glm::vec3 &ptOnA, glm::vec3 &ptOnB);
    void CLosestPoints(ECSWorld &ecs, entt::entity entityA, entt::entity entityB, glm::vec3 &ptOnA, glm::vec3 &ptOnB);
};
