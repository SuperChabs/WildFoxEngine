#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "ShapeFuncStruckt.h"
#include "convex/ConvexStruct.h"
#include "ECS/World.h"

namespace EPA {
    float Expand(ECSWorld &ecs, entt::entity entityA, entt::entity entityB, const float bias, const Point simplexPoints[4],
        glm::vec3 &ptOnA, glm::vec3 &ptOnB);

    glm::vec3 BarycentricCoordinates(glm::vec3 s1, glm::vec3 s2, glm::vec3 s3, const glm::vec3 &pt);
    glm::vec3 NormalDirection(const Tri &tri, const std::vector<Point> &points);

    float SignedDistanceToTriangle(const Tri &tri, const glm::vec3 &pt, const std::vector<Point> &points);

    int ClosestTriangle(const std::vector<Tri> &triangles, const std::vector<Point> &points);
    int RemoveTrianglesFacingPoints(const glm::vec3 &pt, std::vector<Tri> &triangles, const std::vector<Point> &points);

    bool HasPoint(const glm::vec3 &w, std::vector<Tri> &triangles, const std::vector<Point> &points);

    void FindDanglingEdges(std::vector<Edge> &danglingEdges, std::vector<Tri> &tris);
}
