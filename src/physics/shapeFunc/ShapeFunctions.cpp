#include "ShapeFunctions.h"

#include "convex/ConvexFunc.h"
#include "convex/ConvexStruct.h"
#include "convex/inertialFunc/ConvexInertialFunc.h"
#include "ECS/components/Physics.h"
#include "ECS/components/Transform.h"
#include "physics/Bounds.h"

glm::mat3 Shape::InertiaTensor(const Sphere &s) {
    return {2.0f * s.m_radius * s.m_radius / 5.0f};
}

glm::mat3 Shape::InertiaTensor(const Box &s) {
    // inertia tensor for box centered around zero
    const float dx = s.m_bounds.GetMaxs().x - s.m_bounds.GetMins().x;
    const float dy = s.m_bounds.GetMaxs().y - s.m_bounds.GetMins().y;
    const float dz = s.m_bounds.GetMaxs().z - s.m_bounds.GetMins().z;

    glm::mat3 tensor(0.0f);
    tensor[0][0] = (dy * dy + dz * dz) / 12.0f;
    tensor[1][1] = (dx * dx + dz * dz) / 12.0f;
    tensor[2][2] = (dx * dx + dy * dy) / 12.0f;

    // now we need to use the parallel axis theorem to get the inertia tensor for a box
    // that is not centered around the origin

    glm::vec3 cm;
    cm.x = (s.m_bounds.GetMins().x + s.m_bounds.GetMaxs().x) * 0.5f;
    cm.y = (s.m_bounds.GetMins().y + s.m_bounds.GetMaxs().y) * 0.5f;
    cm.z = (s.m_bounds.GetMins().z + s.m_bounds.GetMaxs().z) * 0.5f;

    const glm::vec3 R = glm::vec3(0.0f) - cm; // the displacement from center of mass to the origin
    const float R2 = glm::dot(R, R);
    glm::mat3 patTensor(R2);
    // patTensor[0] = glm::vec3(R2 - R.x * R.x,      R.y * R.y,      R.z * R.z);
    // patTensor[1] = glm::vec3(     R.x * R.x, R2 - R.y * R.y,      R.z * R.z);
    // patTensor[2] = glm::vec3(     R.x * R.x,      R.y * R.y, R2 - R.z * R.z);
    patTensor -= glm::outerProduct(R, R);

    tensor += patTensor;
    return tensor;
}

glm::mat3 Shape::InertiaTensor(const ConvexHull &s) {
    return s.m_inertiaTensor;
}

Bounds Shape::GetBounds(const Sphere &s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale) {
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

Bounds Shape::GetBounds(const Sphere &s) {
    Bounds tmp;
    tmp.SetMaxs(glm::vec3(s.m_radius));
    tmp.SetMins(glm::vec3(-s.m_radius));
    return tmp;
}

Bounds Shape::GetBounds(const Box &s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale) {
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

Bounds Shape::GetBounds(const Box &s) { return s.m_bounds; }

Bounds Shape::GetBounds(const ConvexHull &s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale) {
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

Bounds Shape::GetBounds(const ConvexHull &s) { return s.m_bounds; }

glm::vec3 Shape::Support(const Sphere &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot,
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

glm::vec3 Shape::Support(const Box &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale,
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

glm::vec3 Shape::Support(const ConvexHull &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot,
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

Point Shape::Support(ECSWorld &ecs, const entt::entity &bodyA, const entt::entity &bodyB, glm::vec3 dir,
    const float bias) {
    dir = glm::normalize(dir);

    Point point;

    auto &col_a = ecs.GetComponent<ColliderComponent>(bodyA);
    auto &col_b = ecs.GetComponent<ColliderComponent>(bodyB);
    const auto &t_a = ecs.GetComponent<TransformComponent>(bodyA);
    const auto &t_b = ecs.GetComponent<TransformComponent>(bodyB);

    // find the point in A furthest in direction
    point.ptA = col_a.Support(dir, t_a.position, t_a.rotation, t_a.scale, bias);

    dir *= -1.0f;

    // find the point in B furthest in direction
    point.ptB = col_b.Support(dir, t_b.position, t_b.rotation, t_b.scale, bias);

    // return the point, in the minkowski sum, furthest in the direction
    point.xyz = point.ptA - point.ptB;
    return point;
}

float Shape::FastestLinearSpeed(const Sphere &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
    // TODO: idk man, i need to do something here
    return 1.0f;
}

float Shape::FastestLinearSpeed(const Box &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
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

float Shape::FastestLinearSpeed(const ConvexHull &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
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

void Shape::Build(Box& s, std::vector<glm::vec3> &pts, const int num) {
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

bool Shape::Build(ConvexHull *s, const std::vector<glm::vec3> &pts, const int num, int maxVerts) {
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
