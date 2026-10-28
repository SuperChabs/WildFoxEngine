#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "physics/shapeFunc/ShapeFuncStruckt.h"

namespace SignedVolumeFunc {
    int CompareSigns(float a, float b);
    bool HasPoint(Point simplexPoints[4], const Point &newPt);

    glm::vec2 SignedVolume1D(const glm::vec3 &s1, const glm::vec3 &s2);
    glm::vec3 SignedVolume2D(const glm::vec3 &s1, const glm::vec3 &s2, const glm::vec3 &s3);
    glm::vec4 SignedVolume4D(const glm::vec3 &s1, const glm::vec3 &s2, const glm::vec3 &s3, const glm::vec3 &s4);

    bool SimplexSignedVolume(Point pts[4], int num, glm::vec3 &newDir, glm::vec4 &lambdasOut);

    void SortValids(Point simplexPoints[4], glm::vec4 &lambdas);
    int NumValids(const glm::vec4 &lambdas);
};