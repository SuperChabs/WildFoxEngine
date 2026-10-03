#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "ConvexStruct.h"

namespace ConvexFunc {
    int FindPointFurthestInDir(const std::vector<glm::vec3>& points, const glm::vec3 &dir, int num);
    glm::vec3 FindPointFurthestFromLine(const std::vector<glm::vec3> &points, const glm::vec3 &ptA, const glm::vec3 &ptB, int num);
    glm::vec3 FindPointFurthestFromTriangle(const std::vector<glm::vec3>& points, const glm::vec3 &ptA, const glm::vec3 &ptB, const glm::vec3 &ptC, int num);

    float DistanceFromLine(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &pt);
    float DistanceFromTriangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &pt);

    void RemoveInertialPoints(const std::vector<glm::vec3> &hullPoints, const std::vector<Tri> &hullTris, std::vector<glm::vec3> &checkPts);
    void RemoveUnreferencedPoints(std::vector<glm::vec3> &hullPoints, std::vector<Tri> &hullTris);

    void ExpandConvexHull(std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris, const std::vector<glm::vec3> &verts);

    void AddPoint(std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris, const glm::vec3 &pt);

    bool IsEdgeUnique(const std::vector<Tri> &tris, const std::vector<int> &facingTris, const Edge &edge, int ignoreTri);

    void BuildTetrahedron(const std::vector<glm::vec3> &verts, std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris, int num);
    void BuildConvexHull(const std::vector<glm::vec3> &verts, std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris);
};