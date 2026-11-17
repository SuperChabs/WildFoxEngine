#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "../ConvexStruct.h"

namespace ConvexInertialFunc {
    bool IsExternal(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris, const glm::vec3 &pt);

    glm::vec3 CalculateCenterOfMass(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris);

    glm::mat3 CalculateInertiaTensor(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris, const glm::vec3 &cm);
};
