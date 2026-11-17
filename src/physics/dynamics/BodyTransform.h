#pragma once

#include <glm/glm.hpp>

#include "physics/core/PhysicsContext.h"

namespace BodyTransform {
    glm::vec3 WorldSpaceToBodySpace(const BodyInfo &body, const glm::vec3 &worldPt);
    glm::vec3 BodySpaceToWorldSpace(const BodyInfo &body, const glm::vec3 &worldPt);

    glm::vec3 GetCenterOfMassWorldSpace(const BodyInfo &body);
}
