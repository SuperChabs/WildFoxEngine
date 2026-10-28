#include "ConvexFunc.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <set>
#include <tuple>
#include <unordered_map>

namespace {
    // a point is "in front of" a face only if it is farther than this
    constexpr float kEps = 1e-5f;

    std::vector<int> FindCoplanarExtremeVertices(const std::vector<glm::vec3> &points,
                                                  const std::vector<Tri> &tris, const float epsilon) {
        struct Face {
            glm::vec3 normal;
            float distance;
        };
        struct ProjectedPoint {
            glm::vec2 point;
            int index;
        };

        std::vector<Face> faces;
        faces.reserve(tris.size());
        std::unordered_map<uint64_t, std::vector<int>> edgeFaces;
        edgeFaces.reserve(tris.size() * 2);

        for (size_t i = 0; i < tris.size(); ++i) {
            const Tri &tri = tris[i];
            const glm::vec3 &a = points[tri.a];
            const glm::vec3 normal = glm::normalize(glm::cross(points[tri.c] - points[tri.b],
                                                                points[tri.a] - points[tri.b]));
            faces.push_back({normal, glm::dot(normal, a)});

            const int indices[] = {tri.a, tri.b, tri.c};
            for (int edge = 0; edge < 3; ++edge) {
                const uint32_t low = static_cast<uint32_t>(std::min(indices[edge], indices[(edge + 1) % 3]));
                const uint32_t high = static_cast<uint32_t>(std::max(indices[edge], indices[(edge + 1) % 3]));
                const uint64_t key = (static_cast<uint64_t>(low) << 32) | high;
                edgeFaces[key].push_back(static_cast<int>(i));
            }
        }

        std::vector<bool> visited(tris.size(), false);
        std::vector<bool> keep(points.size(), false);
        constexpr float normalTolerance = 1e-5f;
        const float areaTolerance = epsilon * epsilon;

        for (size_t seed = 0; seed < tris.size(); ++seed) {
            if (visited[seed]) continue;

            std::set<int> vertexSet;
            std::vector<int> pending{static_cast<int>(seed)};
            visited[seed] = true;

            while (!pending.empty()) {
                const int faceIndex = pending.back();
                pending.pop_back();

                const Tri &tri = tris[faceIndex];
                vertexSet.insert(tri.a);
                vertexSet.insert(tri.b);
                vertexSet.insert(tri.c);

                const int indices[] = {tri.a, tri.b, tri.c};
                for (int edge = 0; edge < 3; ++edge) {
                    const uint32_t low = static_cast<uint32_t>(std::min(indices[edge], indices[(edge + 1) % 3]));
                    const uint32_t high = static_cast<uint32_t>(std::max(indices[edge], indices[(edge + 1) % 3]));
                    const uint64_t key = (static_cast<uint64_t>(low) << 32) | high;
                    for (const int neighbor : edgeFaces[key]) {
                        if (visited[neighbor]) continue;

                        const Face &currentFace = faces[faceIndex];
                        const Face &neighborFace = faces[neighbor];
                        if (glm::dot(currentFace.normal, neighborFace.normal) < 1.0f - normalTolerance ||
                            std::abs(currentFace.distance - neighborFace.distance) > epsilon * 4.0f)
                            continue;

                        visited[neighbor] = true;
                        pending.push_back(neighbor);
                    }
                }
            }

            if (vertexSet.size() < 3) continue;

            const glm::vec3 normal = faces[seed].normal;
            int droppedAxis = 0;
            if (std::abs(normal.y) > std::abs(normal.x)) droppedAxis = 1;
            if (std::abs(normal.z) > std::abs(normal[droppedAxis])) droppedAxis = 2;

            std::vector<ProjectedPoint> projected;
            projected.reserve(vertexSet.size());
            for (const int index : vertexSet) {
                const glm::vec3 &point = points[index];
                if (droppedAxis == 0) projected.push_back({{point.y, point.z}, index});
                else if (droppedAxis == 1) projected.push_back({{point.x, point.z}, index});
                else projected.push_back({{point.x, point.y}, index});
            }
            std::sort(projected.begin(), projected.end(), [](const ProjectedPoint &a, const ProjectedPoint &b) {
                if (a.point.x != b.point.x) return a.point.x < b.point.x;
                if (a.point.y != b.point.y) return a.point.y < b.point.y;
                return a.index < b.index;
            });

            const auto cross = [](const glm::vec2 &origin, const glm::vec2 &a, const glm::vec2 &b) {
                const glm::vec2 oa = a - origin;
                const glm::vec2 ob = b - origin;
                return oa.x * ob.y - oa.y * ob.x;
            };
            std::vector<ProjectedPoint> polygon;
            polygon.reserve(projected.size() * 2);
            for (const ProjectedPoint &point : projected) {
                while (polygon.size() >= 2 &&
                       cross(polygon[polygon.size() - 2].point, polygon.back().point, point.point) <= areaTolerance)
                    polygon.pop_back();
                polygon.push_back(point);
            }
            const size_t lowerSize = polygon.size();
            for (auto it = projected.rbegin() + 1; it != projected.rend(); ++it) {
                while (polygon.size() > lowerSize &&
                       cross(polygon[polygon.size() - 2].point, polygon.back().point, it->point) <= areaTolerance)
                    polygon.pop_back();
                polygon.push_back(*it);
            }
            if (polygon.size() > 1) polygon.pop_back();

            for (const ProjectedPoint &point : polygon)
                keep[point.index] = true;
        }

        std::vector<int> extremes;
        extremes.reserve(points.size());
        for (size_t i = 0; i < keep.size(); ++i)
            if (keep[i]) extremes.push_back(static_cast<int>(i));
        return extremes;
    }
}

int ConvexFunc::FindPointFurthestInDir(const std::vector<glm::vec3> &points, const glm::vec3 &dir, int num) {
    int maxIdx = 0;
    float maxDist = glm::dot(points[0], dir);
    for (int i = 1; i < num; i++) {
        float dist = glm::dot(points[i], dir);
        if (dist > maxDist) {
            maxDist = dist;
            maxIdx = i;
        }
    }
    return maxIdx;
}

float ConvexFunc::DistanceFromLine(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &pt) {
    glm::vec3 ab = a - b;
    ab = glm::normalize(ab);

    const glm::vec3 ray = pt - a;
    const glm::vec3 projection = ab * glm::dot(ab, ray);
    const glm::vec3 perpendicular = ray - projection;
    return glm::length(perpendicular);
}

glm::vec3 ConvexFunc::FindPointFurthestFromLine(const std::vector<glm::vec3> &points, const glm::vec3 &ptA,
                                                const glm::vec3 &ptB, int num) {
    int maxIdx = 0;
    float maxDist = DistanceFromLine(ptA, ptB, points[0]);
    for (int i = 1; i < num; i++) {
        float dist = DistanceFromLine(ptA, ptB, points[i]);
        if (dist > maxDist) {
            maxDist = dist;
            maxIdx = i;
        }
    }
    return points[maxIdx];
}

float ConvexFunc::DistanceFromTriangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &pt) {
    const glm::vec3 ab = a - b;
    const glm::vec3 ac = c - b;
    glm::vec3 normal = glm::cross(ac, ab);
    normal = glm::normalize(normal);

    const glm::vec3 ray = pt - a;
    const float dist = glm::dot(normal, ray);
    return dist;
}

glm::vec3 ConvexFunc::FindPointFurthestFromTriangle(const std::vector<glm::vec3> &points, const glm::vec3 &ptA,
    const glm::vec3 &ptB, const glm::vec3 &ptC, int num) {
    int maxIdx = 0;
    float maxDist = DistanceFromTriangle(ptA, ptB, ptC, points[0]);
    for (int i = 1; i < num; i++) {
        float dist = DistanceFromTriangle(ptA, ptB, ptC, points[i]);
        if (dist * dist > maxDist * maxDist) {
            maxDist = dist;
            maxIdx = i;
        }
    }
    return points[maxIdx];
}

bool ConvexFunc::BuildTetrahedron(const std::vector<glm::vec3> &verts, std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris, const int num, const float epsilon) {
    hullPts.clear();
    hullTris.clear();

    glm::vec3 mins = verts[0];
    glm::vec3 maxs = verts[0];
    for (int i = 1; i < num; ++i) {
        mins = glm::min(mins, verts[i]);
        maxs = glm::max(maxs, verts[i]);
    }

    const glm::vec3 extent = maxs - mins;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;

    int minIdx = 0;
    int maxIdx = 0;
    for (int i = 1; i < num; ++i) {
        if (verts[i][axis] < verts[minIdx][axis]) minIdx = i;
        if (verts[i][axis] > verts[maxIdx][axis]) maxIdx = i;
    }

    std::array<glm::vec3, 4> points{};
    points[0] = verts[minIdx];
    points[1] = verts[maxIdx];
    points[2] = FindPointFurthestFromLine(verts, points[0], points[1], num);
    points[3] = FindPointFurthestFromTriangle(verts, points[0], points[1], points[2], num);

    // degenerate input: all points coincide, are collinear or coplanar
    if (glm::length(points[1] - points[0]) < epsilon) return false;
    if (DistanceFromLine(points[0], points[1], points[2]) < epsilon) return false;
    float dist = DistanceFromTriangle(points[0], points[1], points[2], points[3]);
    if (std::abs(dist) < epsilon) return false;

    if (dist > 0.0f)
        std::swap(points[0], points[1]);

    // build the tetrahedron
    for (const auto &p : points)
        hullPts.push_back(p);

    hullTris.push_back({0, 1, 2});
    hullTris.push_back({0, 2, 3});
    hullTris.push_back({2, 1, 3});
    hullTris.push_back({1, 0, 3});

    const glm::vec3 interior = (points[0] + points[1] + points[2] + points[3]) * 0.25f;
    for (Tri &tri : hullTris) {
        if (DistanceFromTriangle(hullPts[tri.a], hullPts[tri.b], hullPts[tri.c], interior) > 0.0f)
            std::swap(tri.b, tri.c);
    }

    return true;
}

void ConvexFunc::BuildConvexHull(const std::vector<glm::vec3> &verts, std::vector<glm::vec3> & hullPts, std::vector<Tri> &hullTris) {
    hullPts.clear();
    hullTris.clear();

    if (verts.size() < 4) return;

    glm::vec3 mins = verts[0];
    glm::vec3 maxs = verts[0];
    for (const glm::vec3 &vertex : verts) {
        mins = glm::min(mins, vertex);
        maxs = glm::max(maxs, vertex);
    }
    const float epsilon = std::max(glm::length(maxs - mins) * 1e-6f, 1e-8f);

    if (!BuildTetrahedron(verts, hullPts, hullTris, static_cast<int>(verts.size()), epsilon)) return;

    ExpandConvexHull(hullPts, hullTris, verts, epsilon);

    const std::vector<int> extremes = FindCoplanarExtremeVertices(hullPts, hullTris, epsilon);
    if (extremes.size() < hullPts.size() && extremes.size() >= 4) {
        std::vector<glm::vec3> reduced;
        reduced.reserve(extremes.size());
        for (const int index : extremes) reduced.push_back(hullPts[index]);

        std::vector<glm::vec3> reducedHullPoints;
        std::vector<Tri> reducedHullTris;
        if (BuildTetrahedron(reduced, reducedHullPoints, reducedHullTris, static_cast<int>(reduced.size()), epsilon)) {
            ExpandConvexHull(reducedHullPoints, reducedHullTris, reduced, epsilon);

            bool preservesHull = true;
            for (const glm::vec3 &point : hullPts) {
                for (const Tri &tri : reducedHullTris) {
                    if (DistanceFromTriangle(reducedHullPoints[tri.a], reducedHullPoints[tri.b],
                                             reducedHullPoints[tri.c], point) > epsilon * 4.0f) {
                        preservesHull = false;
                        break;
                    }
                }
                if (!preservesHull) break;
            }

            if (preservesHull) {
                hullPts = std::move(reducedHullPoints);
                hullTris = std::move(reducedHullTris);
            }
        }
    }
}

std::vector<glm::vec3> ConvexFunc::WeldVertices(const std::vector<glm::vec3> &verts) {
    if (verts.empty()) return {};

    glm::vec3 mn = verts[0], mx = verts[0];
    for (const auto &v : verts) {
        mn = glm::min(mn, v);
        mx = glm::max(mx, v);
    }

    // the grid cell is relative to the model size
    const float cell = std::max(glm::length(mx - mn) * 1e-5f, 1e-8f);

    std::set<std::tuple<int, int, int>> seen;
    std::vector<glm::vec3> out;
    out.reserve(verts.size());

    for (const auto &v : verts) {
        const auto key = std::make_tuple(static_cast<int>(std::floor((v.x - mn.x) / cell)),
                                         static_cast<int>(std::floor((v.y - mn.y) / cell)),
                                         static_cast<int>(std::floor((v.z - mn.z) / cell)));

        if (seen.insert(key).second)
            out.push_back(v);
    }
    return out;
}

void ConvexFunc::SimplifyHull(std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris, int maxVerts) {
    maxVerts = std::max(maxVerts, 4);

    while (static_cast<int>(hullPts.size()) > maxVerts) {
        const size_t n = hullPts.size();

        // centroid of the neighbor ring of every vertex
        std::vector ringSum(n, glm::vec3(0.0f));
        std::vector ringCount(n, 0);
        for (const auto &tri : hullTris) {
            const int idx[3] = {tri.a, tri.b, tri.c};
            for (int k = 0; k < 3; k++) {
                const int v = idx[k];
                ringSum[v] += hullPts[idx[(k + 1) % 3]] + hullPts[idx[(k + 2) % 3]];
                ringCount[v] += 2;;
            }
        }

        // vertex cost = volume of the cap that is lost when the vertex is removed: a fan of tetrahedron
        std::vector cost(n, 0.0f);
        for (const auto &tri : hullTris) {
            const int idx[3] = {tri.a, tri.b, tri.c};
            for (int k = 0; k < 3; k++) {
                const int v = idx[k];
                const glm::vec3 &pv = hullPts[v];
                const glm::vec3 centroid = ringSum[v] / static_cast<float>(ringCount[v]);
                const glm::vec3 e0 = hullPts[idx[(k + 1) % 3]] - pv;
                const glm::vec3 e1 = hullPts[idx[(k + 2) % 3]] - pv;
                cost[v] += std::abs(glm::dot(e0, glm::cross(e1, centroid - pv))) / 6.0f;
            }
        }

        std::vector<size_t> order(n);
        for (size_t i = 0; i < n; ++i)
            order[i] = i;
        std::sort(order.begin(), order.end(), [&](const size_t a, const size_t b) {
            return cost[a] < cost[b];
        });

        std::vector<bool> removed(n, false);
        for (int i = 0; i < static_cast<int>(n) - maxVerts; ++i)
            removed[order[i]] = true;

        std::vector<glm::vec3> rest;
        rest.reserve(maxVerts);
        for (size_t i = 0; i < n; ++i)
            if (!removed[i]) rest.push_back(hullPts[i]);

        // Rebuild once after selecting the least costly vertices to remove.
        std::vector<glm::vec3> newPts;
        std::vector<Tri> newTris;
        BuildConvexHull(rest, newPts, newTris);
        if (newPts.empty()) return;

        hullPts = std::move(newPts);
        hullTris = std::move(newTris);
    }
}

void ConvexFunc::RemoveInertialPoints(const std::vector<glm::vec3> &hullPoints, const std::vector<Tri> &hullTris,
                                      std::vector<glm::vec3> &checkPts) {
    for (int i = 0; i < checkPts.size(); i++) {
        const glm::vec3 &pt = checkPts[i];

        bool isExternal = false;
        for (auto tri : hullTris) {
            const glm::vec3 &a = hullPoints[tri.a];
            const glm::vec3 &b = hullPoints[tri.b];
            const glm::vec3 &c = hullPoints[tri.c];

            // if the point is in front of any triangle then it's external
            float dist = DistanceFromTriangle(hullPoints[tri.a], hullPoints[tri.b], hullPoints[tri.c], pt);
            if (dist > kEps) {
                isExternal = true;
                break;
            }
        }

        if (!isExternal) {
            checkPts.erase(checkPts.begin() + i);
            i--;
        }
    }

    // also remove any point that are just a little too close to the hull
    for (int i = 0; i < checkPts.size(); i++) {
        const glm::vec3 &pt = checkPts[i];

        bool isTooClose = false;
        for (auto hullPt : hullPoints) {
            glm::vec3 ray = hullPt - pt;
            if (glm::dot(ray, ray) < 0.01f * 0.0f) {
                isTooClose = true;
                break;
            }
        }

        if (isTooClose) {
            checkPts.erase(checkPts.begin() + i);
            i--;
        }
    }
}

void ConvexFunc::RemoveUnreferencedPoints(std::vector<glm::vec3> &hullPoints, std::vector<Tri> &hullTris) {
    std::vector<bool> referenced(hullPoints.size(), false);
    for (const Tri &tri : hullTris) {
        referenced[tri.a] = true;
        referenced[tri.b] = true;
        referenced[tri.c] = true;
    }

    std::vector<int> remap(hullPoints.size(), -1);
    std::vector<glm::vec3> compacted;
    compacted.reserve(hullPoints.size());
    for (size_t i = 0; i < hullPoints.size(); ++i) {
        if (!referenced[i]) continue;
        remap[i] = static_cast<int>(compacted.size());
        compacted.push_back(hullPoints[i]);
    }

    for (Tri &tri : hullTris) {
        tri.a = remap[tri.a];
        tri.b = remap[tri.b];
        tri.c = remap[tri.c];
    }
    hullPoints = std::move(compacted);
}

void ConvexFunc::ExpandConvexHull(std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris,
                                  const std::vector<glm::vec3> &verts, const float epsilon) {
    for (const glm::vec3 &point : verts) {
        bool isExternal = false;
        for (const Tri &tri : hullTris) {
            if (DistanceFromTriangle(hullPts[tri.a], hullPts[tri.b], hullPts[tri.c], point) > epsilon) {
                isExternal = true;
                break;
            }
        }

        if (isExternal)
            AddPoint(hullPts, hullTris, point, epsilon);
    }

    RemoveUnreferencedPoints(hullPts, hullTris);
}

void ConvexFunc::AddPoint(std::vector<glm::vec3> &hullPts, std::vector<Tri> &hullTris, const glm::vec3 &pt, const float epsilon) {
    // this point is outside
    // now we need to remove old triangles and build new ones

    // find all the triangles that face this point
    std::vector<bool> facing(hullTris.size(), false);
    struct HorizonEdge {
        Edge edge;
        int count;
    };
    std::unordered_map<uint64_t, HorizonEdge> edgeCounts;
    edgeCounts.reserve(hullTris.size());
    for (size_t i = 0; i < hullTris.size(); ++i) {
        const Tri &tri = hullTris[i];
        const glm::vec3 &a = hullPts[tri.a];
        const glm::vec3 &b = hullPts[tri.b];
        const glm::vec3 &c = hullPts[tri.c];

        const float dist = DistanceFromTriangle(a, b, c, pt);
        if (dist <= epsilon) continue;
        facing[i] = true;
        const Edge edges[3] = {
            {tri.a, tri.b},
            {tri.b, tri.c},
            {tri.c, tri.a}
        };

        for (const Edge &edge : edges) {
            const uint32_t low = static_cast<uint32_t>(std::min(edge.a, edge.b));
            const uint32_t high = static_cast<uint32_t>(std::max(edge.a, edge.b));
            const uint64_t key = (static_cast<uint64_t>(low) << 32) | high;
            auto entry = edgeCounts.try_emplace(key, HorizonEdge{edge, 0}).first;
            ++entry->second.count;
        }
    }

    std::vector<Edge> horizon;
    horizon.reserve(edgeCounts.size());
    for (const auto &entry : edgeCounts)
        if (entry.second.count == 1) horizon.push_back(entry.second.edge);

    std::vector<Tri> remaining;
    remaining.reserve(hullTris.size() + horizon.size());
    for (size_t i = 0; i < hullTris.size(); ++i)
        if (!facing[i]) remaining.push_back(hullTris[i]);

    hullPts.push_back(pt);
    const int newPtIdx = static_cast<int>(hullPts.size() - 1);
    for (const Edge &edge : horizon)
        remaining.push_back({edge.a, edge.b, newPtIdx});
    hullTris = std::move(remaining);
}

bool ConvexFunc::IsEdgeUnique(const std::vector<Tri> &tris, const std::vector<int> &facingTris, const Edge &edge,
                              const int ignoreTri) {
    for (int triIdx : facingTris) {
        if (ignoreTri == triIdx) {
            continue;
        }

        const Tri &tri = tris[triIdx];

        const Edge edges[3] = {
            {tri.a, tri.b},
            {tri.b, tri.c},
            {tri.c, tri.a}
        };

        for (Edge e : edges)
            if (edge == e)
                return false;
    }
    return true;
}