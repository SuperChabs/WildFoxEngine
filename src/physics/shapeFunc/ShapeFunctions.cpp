#include "ShapeFunctions.h"

#include "convex/ConvexFunc.h"
#include "convex/ConvexStruct.h"
#include "convex/inertialFunc/ConvexInertialFunc.h"
#include "ECS/components/Physics.h"
#include "physics/Bounds.h"

glm::mat3 Shape::InertiaTensor(const Sphere &s) {
    return {2.0f * s.m_radius * s.m_radius / 0.5f};
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
    cm.x = (s.m_bounds.GetMins().x + s.m_bounds.GetMaxs().x) / 0.5f;
    cm.y = (s.m_bounds.GetMins().y + s.m_bounds.GetMaxs().y) / 0.5f;
    cm.z = (s.m_bounds.GetMins().z + s.m_bounds.GetMaxs().z) / 0.5f;

    const glm::vec3 R = glm::vec3(0.0f) - cm; // the displacement from center of mass to the origin
    const float R2 = glm::length(R);
    glm::mat3 patTensor;
    patTensor[0] = glm::vec3(R2 - R.x * R.x,      R.y * R.y,      R.z * R.z);
    patTensor[1] = glm::vec3(     R.x * R.x, R2 - R.y * R.y,      R.z * R.z);
    patTensor[2] = glm::vec3(     R.x * R.x,      R.y * R.y, R2 - R.z * R.z);

    tensor += patTensor;
    return tensor;
}

glm::mat3 Shape::InertiaTensor(const ConvexHull &s) {
    return s.m_inertiaTensor;
}

Bounds Shape::GetBounds(const Sphere &s, const glm::vec3 &pos, const glm::quat &rot) {
    Bounds tmp;
    tmp.SetMaxs(glm::vec3(s.m_radius) + pos);
    tmp.SetMins(glm::vec3(-s.m_radius) + pos);
    return tmp;
}

Bounds Shape::GetBounds(const Sphere &s) {
    Bounds tmp;
    tmp.SetMaxs(glm::vec3(s.m_radius));
    tmp.SetMins(glm::vec3(-s.m_radius));
    return tmp;
}

Bounds Shape::GetBounds(const Box &s, const glm::vec3 &pos, const glm::quat &rot) {
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
        corner = corner * rot + pos;
        bounds.Expand(corner);
    }

    return bounds;
}

Bounds Shape::GetBounds(const Box &s) { return s.m_bounds; }

Bounds Shape::GetBounds(const ConvexHull &s, const glm::vec3 &pos, const glm::quat &rot) {
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
        corner = corner * rot + pos;
        bounds.Expand(corner);
    }

    return bounds;
}

Bounds Shape::GetBounds(const ConvexHull &s) { return s.m_bounds; }

glm::vec3 Shape::Support(const Sphere &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot,
                         const float bias) {
    return pos + dir * (s.m_radius + bias);
}

glm::vec3 Shape::Support(const Box &s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const float bias) {
    // find the point in furthest in direction
    glm::vec3 maxPt = rot *s.m_points[0] + pos;
    float maxDist = glm::dot(maxPt, dir);
    for (int i = 1; i < s.m_points.size(); i++) {
        const glm::vec3 pt = rot *s.m_points[i] + pos;
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
    float bias) {
    return {0.0f, 0.0f, 0.0f};
}

float Shape::FastestLinearSpeed(const Box &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
    float maxSpeed = 0.0f;
    for (int i = 1; i < s.m_points.size(); i++) {
        glm::vec3 r = s.m_points[i] - s.m_centerOfMass;
        glm::vec3 linearVelocity = glm::cross(r, angularVelocity);
        float speed = glm::dot(dir, linearVelocity);
        if (speed > maxSpeed) {
            maxSpeed = speed;
        }
    }
    return maxSpeed;
}

float Shape::FastestLinearSpeed(const ConvexHull &s, const glm::vec3 &angularVelocity, const glm::vec3 &dir) {
    float maxSpeed = 0.0f;
    for (int i = 1; i < s.m_points.size(); i++) {
        glm::vec3 r = s.m_points[i] - s.m_centerOfMass;
        glm::vec3 linearVelocity = glm::cross(r, angularVelocity);
        float speed = glm::dot(dir, linearVelocity);
        if (speed > maxSpeed) {
            maxSpeed = speed;
        }
    }
    return maxSpeed;
}

void Shape::Build(Box& s, const std::vector<glm::vec3> &pts, const int num) {
    for (int i = 0; i < num; i++)
        s.m_bounds.Expand(pts.at(i));

    s.m_points.clear();
    s.m_points.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMins().y, s.m_bounds.GetMins().z);
    s.m_points.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMins().y, s.m_bounds.GetMins().z);
    s.m_points.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMins().z);
    s.m_points.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMins().y, s.m_bounds.GetMaxs().z);

    s.m_points.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMaxs().z);
    s.m_points.emplace_back(s.m_bounds.GetMins().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMaxs().z);
    s.m_points.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMins().y, s.m_bounds.GetMaxs().z);
    s.m_points.emplace_back(s.m_bounds.GetMaxs().x, s.m_bounds.GetMaxs().y, s.m_bounds.GetMins().z);

    s.m_centerOfMass = (s.m_bounds.GetMins() + s.m_bounds.GetMaxs()) / 0.5f;
}

void Shape::Build(ConvexHull *s, const std::vector<glm::vec3> &pts, const int num) {
    s->m_points.clear();
    s->m_points.reserve(num);

    for (int i = 1; i < num; i++)
        s->m_points.emplace_back(pts.at(i));

    // expand into convex hull
    std::vector<glm::vec3> hullPoints;
    std::vector<Tri> hullTriangles;
    ConvexFunc::BuildConvexHull(s->m_points, hullPoints, hullTriangles);
    s->m_points = hullPoints;

    // expand the bounds
    s->m_bounds.Clear();
    s->m_bounds.Expand(s->m_points.data(), static_cast<int>(s->m_points.size()));

    s->m_centerOfMass = ConvexInertialFunc::CalculateCenterOfMass(hullPoints, hullTriangles);

    s->m_inertiaTensor = ConvexInertialFunc::CalculateInertiaTensor(hullPoints, hullTriangles, s->m_centerOfMass);
}
