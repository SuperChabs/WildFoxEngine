#include "SupportFunc.h"

glm::vec3 Support::Support(const Sphere &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot,
                         const glm::vec3 &scale, const float bias) {
    const glm::vec3 localDir = glm::inverse(rot) * dir;
    const glm::vec3 scaledDir = localDir * scale;
    const float directionLength = glm::length(scaledDir);
    if (directionLength <= 1e-8f)
        return pos + rot * (s.m_center * scale)
            + (glm::length(dir) > 1e-8f ? glm::normalize(dir) * bias : glm::vec3(0.0f));

    const glm::vec3 surface = s.m_radius * scale * scale * localDir / directionLength;
    const glm::vec3 normalizedDir = glm::length(dir) > 1e-8f ? glm::normalize(dir) : glm::vec3(0.0f);
    return pos + rot * (s.m_center * scale + surface) + normalizedDir * bias;
}

glm::vec3 Support::Support(const Box &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale,
    const float bias) {
    // find the point in furthest in direction
    glm::vec3 maxPt = rot * (s.m_points[0] * scale) + pos;
    float maxDist = glm::dot(maxPt, dir);
    for (int i = 1; i < s.m_points.size(); i++) {
        const glm::vec3 pt = rot * (s.m_points[i] * scale) + pos;
        float dist = glm::dot(pt, dir);

        if (dist > maxDist) {
            maxDist = dist;
            maxPt = pt;
        }
    }

    glm::vec3 norm = dir;
    norm = glm::normalize(norm);
    norm *= bias;

    return norm + maxPt;
}

glm::vec3 Support::Support(const ConvexHull &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot,
                         const glm::vec3 &scale, const float bias) {
    // find the point in the furthest direction
    glm::vec3 maxPt = rot * (s.m_points[0] * scale) + pos;
    float maxDist = glm::dot(maxPt, dir);
    for (int i = 1; i < s.m_points.size(); i++) {
        const glm::vec3 pt = rot * (s.m_points[i] * scale) + pos;
        float dist = glm::dot(pt, dir);

        if (dist > maxDist) {
            maxDist = dist;
            maxPt = pt;
        }
    }

    glm::vec3 norm = dir;
    norm = glm::normalize(norm);
    norm *= bias;

    return norm + maxPt;
}

Point Support::Support(const BodyInfo &bodyA, const BodyInfo &bodyB, glm::vec3 dir, const float bias) {
    dir = glm::normalize(dir);

    Point point;

    auto &col_a = bodyA.collider;
    auto &col_b = bodyB.collider;
    const auto &t_a = bodyA.transform;
    const auto &t_b = bodyB.transform;

    // find the point in A furthest in direction
    point.ptA = col_a->Support(dir, t_a->position, t_a->rotation, t_a->scale, bias);

    dir *= -1.0f;

    // find the point in B furthest in direction
    point.ptB = col_b->Support(dir, t_b->position, t_b->rotation, t_b->scale, bias);

    // return the point, in the minkowski sum, furthest in the direction
    point.xyz = point.ptA - point.ptB;
    return point;
}