#pragma once
#include <glm/vec3.hpp>


struct Point {
    glm::vec3 xyz = glm::vec3(0.0f);
    glm::vec3 ptA = glm::vec3(0.0f);
    glm::vec3 ptB = glm::vec3(0.0f);

    Point &operator=(const Point &rhs) = default;

    bool operator==(const Point &rhs) const {
        return xyz == rhs.xyz && ptA == rhs.ptA && ptB == rhs.ptB;
    }
};
