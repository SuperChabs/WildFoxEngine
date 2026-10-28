#include "EPA.h"

#include "ShapeFunctions.h"
#include "scripting/ASBindings.h"
#include "scripting/ASBindings.h"

float EPA::Expand(ECSWorld &ecs, entt::entity entityA, entt::entity entityB, const float bias,
                  const Point simplexPoints[4], glm::vec3 &ptOnA, glm::vec3 &ptOnB) {
    std::vector<Point> points;
    std::vector<Tri> triangles;
    std::vector<Edge> danglingEdges;

    glm::vec3 centre = glm::vec3(0.0f);
    for (int i = 0; i < 4; i++) {
        points.push_back(simplexPoints[i]);
        centre += simplexPoints[i].xyz;
    }
    centre *= 0.25f;

    // build the triangles
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        int k = (i + 2) % 4;
        Tri tri;
        tri.a = i;
        tri.b = j;
        tri.c = k;

        int unusedPt = (i + 3) % 4;
        float dist = SignedDistanceToTriangle(tri, points[unusedPt].xyz, points);

        // the unused point is always on the negative/inside of the triangle. Make sure the normal points away
        if (dist > 0.0f)
            std::swap(tri.a, tri.b);

        triangles.push_back(tri);
    }

    //
    // Expand the simplex to find the closest face of the CSO to the origin
    //
    while (true) {
        const int idx = ClosestTriangle(triangles, points);
        if (idx < 0) break;
        glm::vec3 normal = NormalDirection(triangles[idx], points);

        const Point newPt = Shape::Support(ecs, entityA, entityB, normal, bias);

        // if w already exists, then just stop because it means we can't expand any further
        if (HasPoint(newPt.xyz, triangles, points)) break;

        float dist = SignedDistanceToTriangle(triangles[idx], newPt.xyz, points);
        if (dist <= 0.0f) break; // can't expand

        const int newIdx = static_cast<int>(points.size());
        points.push_back(newPt);

        // remove triangles that face this point
        int numRemoved = RemoveTrianglesFacingPoints(newPt.xyz, triangles, points);
        if (numRemoved == 0) break;

        // find dangling edges
        FindDanglingEdges(danglingEdges, triangles);
        if (danglingEdges.size() == 0) break;

        /**
         * in theory the edges should be a proper CCW order
         * so we only need to add the new point as 'a' in order
         * to create new triangles that face away from origin
         */
        for (int i = 0; i < danglingEdges.size(); i++) {
            const Edge &edge = danglingEdges[i];

            Tri triangle;
            triangle.a = newIdx;
            triangle.b = edge.b;
            triangle.c = edge.a;

            // make sure it's oriented properly
            float dist2 = SignedDistanceToTriangle(triangle, centre, points);
            if (dist2 > 0.0f) std::swap(triangle.a, triangle.b);

            triangles.push_back(triangle);
        }
    }

    // get the projection of the origin on the closest triangle
    const int idx = ClosestTriangle(triangles, points);
    const Tri &tri = triangles[idx];
    glm::vec3 ptA_w = points[tri.a].xyz;
    glm::vec3 ptB_w = points[tri.b].xyz;
    glm::vec3 ptC_w = points[tri.c].xyz;
    glm::vec3 lambdas = BarycentricCoordinates(ptA_w, ptB_w, ptC_w, glm::vec3(0.0f));

    // get the point on shape A
    glm::vec3 ptA_a = points[tri.a].ptA;
    glm::vec3 ptB_a = points[tri.b].ptA;
    glm::vec3 ptC_a = points[tri.c].ptA;
    ptOnA = ptA_a * lambdas[0] + ptB_a * lambdas[1] + ptC_a * lambdas[2];

    // get the point on shape B
    glm::vec3 ptA_b = points[tri.a].ptB;
    glm::vec3 ptB_b = points[tri.b].ptB;
    glm::vec3 ptC_b = points[tri.c].ptB;
    ptOnB = ptA_b * lambdas[0] + ptB_b * lambdas[1] + ptC_b * lambdas[2];

    // return the penetration distance
    glm::vec3 delta = ptOnB - ptOnA;

    float mag = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
    mag = glm::sqrt(mag);

    return mag;
}

glm::vec3 EPA::BarycentricCoordinates(glm::vec3 s1, glm::vec3 s2, glm::vec3 s3, const glm::vec3 &pt) {
    s1 = s1 - pt;
    s2 = s2 - pt;
    s3 = s3 - pt;

    glm::vec3 normal = glm::cross(s3 - s1, s2 - s1);
    glm::vec3 p0 = normal * (glm::dot(normal, s1) / glm::dot(normal, normal));

    // find the axis with the greatest projected area
    int idx = 0;
    float areaMax = 0;
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;

        glm::vec2 a = glm::vec2(s1[j], s1[k]);
        glm::vec2 b = glm::vec2(s2[j], s2[k]);
        glm::vec2 c = glm::vec2(s3[j], s3[k]);
        glm::vec2 ab = b - a;
        glm::vec2 ac = c - a;

        float area = ab.x * ac.y - ab.y * ac.x;
        if (area * area > areaMax * areaMax) {
            idx = i;
            areaMax = area;
        }
    }

    // project onto the appropriate axis
    int x = (idx + 1) % 3;
    int y = (idx + 2) % 3;
    glm::vec2 s[3];
    s[0] = glm::vec2(s1[x], s1[y]);
    s[1] = glm::vec2(s2[x], s2[y]);
    s[2] = glm::vec2(s3[x], s3[y]);
    glm::vec2 p = glm::vec2(p0[x], p0[y]);

    // get the sub-areas of the triangles formed from the projected origin and the edges
    glm::vec3 areas;
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;

        glm::vec2 a = p;
        glm::vec2 b = s[j];
        glm::vec2 c = s[k];
        glm::vec2 ab = b - a;
        glm::vec2 ac = c - a;

        areas[i] = ab.x * ac.y - ab.y * ac.x;
    }

    glm::vec3 lambdas = areas / areaMax;
    if (lambdas.x * 0.0f != lambdas.x * 0.0f && lambdas.y * 0.0f != lambdas.y * 0.0f)
        lambdas = glm::vec3(1, 0, 0);

    return lambdas;
}

glm::vec3 EPA::NormalDirection(const Tri &tri, const std::vector<Point> &points) {
    const glm::vec3 &a = points[tri.a].xyz;
    const glm::vec3 &b = points[tri.b].xyz;
    const glm::vec3 &c = points[tri.c].xyz;

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 normal = glm::cross(ac, ab);
    normal = glm::normalize(normal);
    return normal;
}

float EPA::SignedDistanceToTriangle(const Tri &tri, const glm::vec3 &pt, const std::vector<Point> &points) {
    const glm::vec3 normal = NormalDirection(tri, points);
    const glm::vec3 &a = points[tri.a].xyz;
    const glm::vec3 a2pt = pt - a;
    const float dist = glm::dot(a2pt, normal);
    return dist;
}

int EPA::ClosestTriangle(const std::vector<Tri> &triangles, const std::vector<Point> &points) {
    float minDistSqr = 1e10;

    int idx = -1;
    for (int i = 0; i < triangles.size(); i++) {
        const Tri tri = triangles[i];

        const float dist = SignedDistanceToTriangle(tri, glm::vec3(0.0f), points);
        const float distSqr = dist * dist;
        if (distSqr < minDistSqr) {
            idx = i;
            minDistSqr = distSqr;
        }
    }

    return idx;
}

int EPA::RemoveTrianglesFacingPoints(const glm::vec3 &pt, std::vector<Tri> &triangles,
    const std::vector<Point> &points) {
    int numRemoved = 0;
    for (int i = 0; i < triangles.size(); i++) {
        const Tri &tri = triangles[i];

        float dist = SignedDistanceToTriangle(tri, pt, points);
        if (dist > 0.0f) {
            // this triangle faces the point. Remove it
            triangles.erase(triangles.begin() + i);
            i--;
            numRemoved++;
        }
    }
    return numRemoved;
}

bool EPA::HasPoint(const glm::vec3 &w, std::vector<Tri> &triangles, const std::vector<Point> &points) {
    constexpr float epsilon = 0.001f * 0.001f;
    for (const auto &p : points) {
        const glm::vec3 d = w - p.xyz;
        if (glm::dot(d, d) < epsilon) return true;
    }
    return false;
}

void EPA::FindDanglingEdges(std::vector<Edge> &danglingEdges, std::vector<Tri> &tris) {
    danglingEdges.clear();
    for (size_t i = 0; i < tris.size(); i++) {
        const Tri &t = tris[i];
        const Edge e[3] = {{t.a, t.b}, {t.b, t.c}, {t.c, t.a}};
        int counts[3] = {};
        for (size_t j = 0; j < tris.size(); j++) {
            if (j == i) continue;
            const Tri &u = tris[j];
            const Edge f[3] = {{u.a, u.b}, {u.b, u.c}, {u.c, u.a}};
            for (int k = 0; k < 3; k++)
                for (int m = 0; m < 3; m++)
                    if (e[k] == f[m]) counts[k]++;
        }
        for (int k = 0; k < 3; k++)
            if (counts[k] == 0) danglingEdges.push_back(e[k]);
    }
}