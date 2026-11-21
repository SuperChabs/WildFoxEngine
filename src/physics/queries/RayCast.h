#pragma once
#include <span>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "RayStruct.h"

struct c_Collider;
struct ConvexHull;

struct Plane {
    glm::vec3 n;
    float w;
};

namespace RayCast {
    Ray ToLocal(const Ray &r, const glm::mat4 &invModel);
    glm::vec3 NormalToWorld(const glm::mat4 &invModel, glm::vec3 nLocal);

    HitInfo Sphere(const Ray &r, glm::vec3 centre, float radius, float maxDist);
    HitInfo Box(const Ray& ray, const glm::mat4& model, glm::vec3 half, float maxDist);
    HitInfo Convex(const Ray &r, const glm::mat4 &model, std::span<Plane> planes, float maxDist, bool hitFromInside = false);

    // sphere in the local space of model, so non-uniform scale turns it into an ellipsoid
    HitInfo LocalSphere(const Ray &r, const glm::mat4 &model, glm::vec3 centre, float radius, float maxDist);
    // axis-aligned box given by local min/max, transformed by model
    HitInfo LocalAABB(const Ray &r, const glm::mat4 &model, glm::vec3 mins, glm::vec3 maxs, float maxDist);
    // any collider shape; ray direction must be normalized, distances are in world units
    HitInfo Collider(const Ray &r, const c_Collider &collider, const glm::mat4 &model, float maxDist);

    // outward planes of hull faces (inside: dot(n, p) + w <= 0)
    std::vector<Plane> HullPlanes(const ConvexHull &hull);
}
