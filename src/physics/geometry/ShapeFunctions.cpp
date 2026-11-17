#include "ShapeFunctions.h"

#include <glm/gtc/quaternion.hpp>

#include "shapes/convex/ConvexFunc.h"
#include "shapes/convex/inertialFunc/ConvexInertialFunc.h"

void ShapeFunc::Build(Box& s, std::vector<glm::vec3> &pts, const int num) {
    if (num <= 0 || static_cast<size_t>(num) > pts.size())
        return;

    s.m_bounds.Clear();
    for (int i = 0; i < num; i++)
        s.m_bounds.Expand(pts.at(i));

    const glm::vec3 min = s.m_bounds.GetMins();
    const glm::vec3 max = s.m_bounds.GetMaxs();

    s.m_points.clear();

    s.m_points.emplace_back(min.x, min.y, min.z);
    s.m_points.emplace_back(max.x, min.y, min.z);
    s.m_points.emplace_back(max.x, max.y, min.z);
    s.m_points.emplace_back(min.x, max.y, min.z);

    s.m_points.emplace_back(min.x, min.y, max.z);
    s.m_points.emplace_back(max.x, min.y, max.z);
    s.m_points.emplace_back(max.x, max.y, max.z);
    s.m_points.emplace_back(min.x, max.y, max.z);

    s.m_centerOfMass = (min + max) * 0.5f;
}

bool ShapeFunc::Build(ConvexHull *s, const std::vector<glm::vec3> &pts, const int num, int maxVerts) {
    if (!s || num < 4 || static_cast<size_t>(num) > pts.size())
        return false;

    // 'num' points of the input (fixes the old loop that skipped pts[0])
    const std::vector input(pts.begin(), pts.begin() + num);

    // 1) weld duplicates, 2) convex hull, 3) simplify
    const std::vector<glm::vec3> welded = ConvexFunc::WeldVertices(input);
    if (welded.size() < 4)
        return false;

    std::vector<glm::vec3> hullPoints;
    std::vector<Tri> hullTriangles;
    ConvexFunc::BuildConvexHull(welded, hullPoints, hullTriangles);
    if (hullPoints.empty()) return false;

    ConvexFunc::SimplifyHull(hullPoints, hullTriangles, maxVerts);
    if (hullPoints.size() < 4 || hullTriangles.empty())
        return false;

    s->m_points = std::move(hullPoints);
    s->m_tris   = std::move(hullTriangles); // needed by the debug renderer
    s->m_bounds.Clear();
    s->m_bounds.Expand(s->m_points.data(), static_cast<int>(s->m_points.size()));

    s->m_centerOfMass  = ConvexInertialFunc::CalculateCenterOfMass(s->m_points, s->m_tris);
    s->m_inertiaTensor = ConvexInertialFunc::CalculateInertiaTensor(s->m_points, s->m_tris, s->m_centerOfMass);
    return true;
}

Bounds ShapeFunc::GetBounds(const Sphere &s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale) {
    Bounds tmp;
    const glm::vec3 center = pos + rot * (s.m_center * scale);
    glm::mat3 scaleMatrix(0.0f);
    scaleMatrix[0][0] = scale.x;
    scaleMatrix[1][1] = scale.y;
    scaleMatrix[2][2] = scale.z;
    const glm::mat3 scaledRotation = glm::mat3_cast(rot) * scaleMatrix;
    const glm::vec3 extent(
        s.m_radius * glm::length(glm::vec3(scaledRotation[0][0], scaledRotation[1][0], scaledRotation[2][0])),
        s.m_radius * glm::length(glm::vec3(scaledRotation[0][1], scaledRotation[1][1], scaledRotation[2][1])),
        s.m_radius * glm::length(glm::vec3(scaledRotation[0][2], scaledRotation[1][2], scaledRotation[2][2]))
    );
    tmp.SetMaxs(center + extent);
    tmp.SetMins(center - extent);
    return tmp;
}

Bounds ShapeFunc::GetBounds(const Sphere &s) {
    Bounds tmp;
    tmp.SetMaxs(glm::vec3(s.m_radius));
    tmp.SetMins(glm::vec3(-s.m_radius));
    return tmp;
}

Bounds ShapeFunc::GetBounds(const Box &s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale) {
    std::vector<glm::vec3> corners;
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMins().y, s.m_bounds.GetMins().z);
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMins().y, s.m_bounds.GetMaxs().z);
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMins().z);
    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMins().y, s.m_bounds.GetMins().z);

    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMaxs().z);
    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMins().z);
    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMins().y, s.m_bounds.GetMaxs().z);
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMaxs().z);

    Bounds bounds;
    for (auto & corner : corners) {
        corner = rot * (corner * scale) + pos;
        bounds.Expand(corner);
    }

    return bounds;
}

Bounds ShapeFunc::GetBounds(const Box &s) { return s.m_bounds; }

Bounds ShapeFunc::GetBounds(const ConvexHull &s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale) {
    std::vector<glm::vec3> corners;
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMins().y, s.m_bounds.GetMins().z);
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMins().y, s.m_bounds.GetMaxs().z);
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMins().z);
    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMins().y, s.m_bounds.GetMins().z);

    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMaxs().z);
    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMins().z);
    corners.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMins().y, s.m_bounds.GetMaxs().z);
    corners.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMaxs().z);

    Bounds bounds;
    for (auto & corner : corners) {
        corner = rot * (corner * scale) + pos;
        bounds.Expand(corner);
    }

    return bounds;
}

Bounds ShapeFunc::GetBounds(const ConvexHull &s) { return s.m_bounds; }

float ShapeFunc::FastestLinearSpeed(const Sphere &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
    // TODO: idk man, i need to do something here
    return 1.0f;
}

float ShapeFunc::FastestLinearSpeed(const Box &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
    float maxSpeed = 0.0f;
    for (int i = 0; i < s.m_points.size(); i++) {
        glm::vec3 r = s.m_points[i] - s.m_centerOfMass;
        glm::vec3 linearVelocity = glm::cross(angularVelocity, r);
        float speed = glm::dot(dir, linearVelocity);
        if (speed > maxSpeed) {
            maxSpeed = speed;
        }
    }
    return maxSpeed;
}

float ShapeFunc::FastestLinearSpeed(const ConvexHull &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
    float maxSpeed = 0.0f;
    for (int i = 0; i < s.m_points.size(); i++) {
        glm::vec3 r = s.m_points[i] - s.m_centerOfMass;
        glm::vec3 linearVelocity = glm::cross(angularVelocity, r);
        float speed = glm::dot(dir, linearVelocity);
        if (speed > maxSpeed) {
            maxSpeed = speed;
        }
    }
    return maxSpeed;
}