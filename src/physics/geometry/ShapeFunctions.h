#pragma once

#include "Bounds.h"
#include "ECS/components/Physics.h"

namespace ShapeFunc {
    [[deprecated("There's no implemetation. Dont use ist")]]
    void Build(Sphere     &s, std::vector<glm::vec3> &pts, int num);
    void Build(Box        &s, std::vector<glm::vec3> &pts, int num);
    bool Build(ConvexHull *s, const std::vector<glm::vec3> &pts, int num, int maxVerts = 1024);

    Bounds GetBounds(const Sphere& s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale);
    Bounds GetBounds(const Sphere& s);
    Bounds GetBounds(const Box& s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale);
    Bounds GetBounds(const Box& s);
    Bounds GetBounds(const ConvexHull& s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale);
    Bounds GetBounds(const ConvexHull& s);

    float FastestLinearSpeed(const Sphere& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
    float FastestLinearSpeed(const Box& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
    float FastestLinearSpeed(const ConvexHull& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
}
