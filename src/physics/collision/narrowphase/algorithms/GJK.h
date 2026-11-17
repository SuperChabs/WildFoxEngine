#pragma once

#include "physics/core/PhysicsContext.h"

namespace GJK {
    bool DoesIntersect(BodyInfo &bodyA, BodyInfo &bodyB, float bias, glm::vec3 &ptOnA, glm::vec3 &ptOnB);
    void CLosestPoints(BodyInfo &bodyA, BodyInfo &bodyB, glm::vec3 &ptOnA, glm::vec3 &ptOnB);
};
