#include "RayCast.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/core/utility.hpp>

#include "ECS/components/Physics.h"

Ray RayCast::ToLocal(const Ray &r, const glm::mat4 &invModel) {
    return {glm::vec3(invModel * glm::vec4(r.origin, 1.0f)), glm::vec3(invModel * glm::vec4(r.direction, 0.0f))};
}

glm::vec3 RayCast::NormalToWorld(const glm::mat4 &invModel, const glm::vec3 nLocal) {
    return glm::normalize(glm::vec3(glm::transpose(invModel) * glm::vec4(nLocal, 0.0f)));
}

HitInfo RayCast::Sphere(const Ray &r, glm::vec3 centre, float radius, float maxDist) {
    HitInfo out;
    const glm::vec3 m = r.origin - centre;
    const float b = glm::dot(m, r.direction);
    const float c = glm::dot(m, m) - radius * radius;
    if (c > 0.0f && b > 0.0f) return out;
    const float dist = b*b - c;
    if (dist < 0.0f) return out;

    const float t = std::max(0.0f, -b - std::sqrt(dist));
    if (t > maxDist) return out;

    out.hit = true;
    out.distance = t;
    out.hitPoint = r.origin + r.direction * t;
    out.normal = glm::normalize(out.hitPoint - centre);
    return out;
}

HitInfo RayCast::Box(const Ray &ray, const glm::mat4 &model, glm::vec3 half, float maxDist) {
    HitInfo out;
    glm::mat4 inv = glm::inverse(model);
    Ray l = ToLocal(ray, inv);

    float tMin = 0.f, tmax = maxDist;
    glm::vec3 nLocal(0.f);

    for (int i = 0; i < 3; ++i) {
        if (std::abs(l.direction[i]) < 1e-8f) {
            if (l.origin[i] < -half[i] || l.origin[i] > half[i]) return out;
            continue;
        }
        float invD = 1.f / l.direction[i];
        float t1 = (-half[i] - l.origin[i]) * invD;
        float t2 = ( half[i] - l.origin[i]) * invD;
        float s = -1.f;
        if (t1 > t2) { std::swap(t1, t2); s = 1.f; }
        if (t1 > tMin) { tMin = t1; nLocal = glm::vec3(0.f); nLocal[i] = s; }
        tmax = std::min(tmax, t2);
        if (tMin > tmax) return out;
    }

    out.hit      = true;
    out.distance = tMin;
    out.hitPoint = ray.origin + ray.direction * tMin;
    out.normal   = (nLocal == glm::vec3(0.f)) ? -ray.direction
                                              : NormalToWorld(inv, nLocal);
    return out;
}

HitInfo RayCast::Convex(const Ray &r, const glm::mat4 &model, std::span<Plane> planes, float maxDist,
                        bool hitFromInside) {
    HitInfo out;
    glm::mat4 inv = glm::inverse(model);
    Ray l = ToLocal(r, inv);

    float tNear = -std::numeric_limits<float>::infinity();
    float tFar = maxDist;
    int nearIdx = -1, farIdx = -1;

    for (int i = 0; i < (int)planes.size(); i++) {
        float vd = glm::dot(l.direction, planes[i].n);
        float vn = glm::dot(l.origin, planes[i].n) + planes[i].w;

        if (std::abs(vd) < 1e-8f) {
            if (vn > 0.0f) return out;
            continue;
        }

        float t = -vn / vd;
        if (vd < 0.0f) {
            if (t > tFar) return out;
            if (t > tNear) { tNear = t; nearIdx = i; }
        }
        else {
            if (t < tFar) { tFar = t; farIdx = i; }
            if (t < tNear) return out;
        }
    }

    float t;
    int idx;
    if (tNear >= 0.0f) {
        t = tNear;
        idx = nearIdx;
    }
    else if (hitFromInside && tFar < maxDist) {
        t = tFar;
        idx = farIdx;
    }
    else return out;

    out.hit = true;
    out.distance = t;
    out.hitPoint = r.origin + r.direction * t;
    out.normal = NormalToWorld(inv, planes[idx].n);
    return out; // wtf is happening. i dont understand ts
}

HitInfo RayCast::LocalSphere(const Ray &r, const glm::mat4 &model, glm::vec3 centre, float radius, float maxDist) {
    HitInfo out;
    const glm::mat4 inv = glm::inverse(model);
    Ray l = ToLocal(r, inv);

    // local direction is not unit length under scale: normalize and convert distances back
    const float len = glm::length(l.direction);
    if (len < 1e-8f) return out;
    l.direction /= len;

    HitInfo local = Sphere(l, centre, radius, maxDist * len);
    if (!local.hit) return out;

    out.hit = true;
    out.distance = local.distance / len;
    out.hitPoint = r.origin + r.direction * out.distance;
    out.normal = NormalToWorld(inv, local.normal);
    return out;
}

HitInfo RayCast::LocalAABB(const Ray &r, const glm::mat4 &model, glm::vec3 mins, glm::vec3 maxs, float maxDist) {
    const glm::vec3 centre = (mins + maxs) * 0.5f;
    const glm::vec3 half = glm::max((maxs - mins) * 0.5f, glm::vec3(1e-4f)); // flat meshes (planes) still need volume
    return Box(r, glm::translate(model, centre), half, maxDist);
}

HitInfo RayCast::Collider(const Ray &r, const c_Collider &collider, const glm::mat4 &model, float maxDist) {
    return std::visit(entt::overloaded{
        [&](const ::Sphere &s) {
            return LocalSphere(r, model, s.m_center, s.m_radius, maxDist);
        },
        [&](const ::Box &b) {
            if (b.m_points.empty()) return HitInfo{};
            glm::vec3 mins(std::numeric_limits<float>::max()), maxs(std::numeric_limits<float>::lowest());
            for (const glm::vec3 &p : b.m_points) {
                mins = glm::min(mins, p);
                maxs = glm::max(maxs, p);
            }
            return LocalAABB(r, model, mins, maxs, maxDist);
        },
        [&](const ConvexHull &h) {
            std::vector<Plane> planes = HullPlanes(h);
            if (planes.empty()) return HitInfo{};
            return Convex(r, model, planes, maxDist);
        },
    }, collider.shape);
}

std::vector<Plane> RayCast::HullPlanes(const ConvexHull &hull) {
    std::vector<Plane> planes;
    if (hull.m_points.empty()) return planes;

    glm::vec3 centroid(0.0f);
    for (const glm::vec3 &p : hull.m_points) centroid += p;
    centroid /= static_cast<float>(hull.m_points.size());

    planes.reserve(hull.m_tris.size());
    for (const Tri &t : hull.m_tris) {
        const glm::vec3 &a = hull.m_points[t.a];
        const glm::vec3 &b = hull.m_points[t.b];
        const glm::vec3 &c = hull.m_points[t.c];

        glm::vec3 n = glm::cross(b - a, c - a);
        const float len = glm::length(n);
        if (len < 1e-8f) continue;
        n /= len;

        float w = -glm::dot(n, a);
        if (glm::dot(n, centroid) + w > 0.0f) { // winding is not guaranteed, make the normal point outwards
            n = -n;
            w = -w;
        }
        planes.push_back({n, w});
    }
    return planes;
}
