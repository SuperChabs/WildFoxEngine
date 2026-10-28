#include "ConvexInertialFunc.h"

#include "../ConvexFunc.h"
#include "../../../Bounds.h"

namespace {
    struct Integrals {
        float volume = 0.0f;
        glm::vec3 com{0.0f};
        glm::mat3 cov{0.0f}; // integral of x * xᵀ dV, around the origin
    };

    Integrals Integrate(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris) {
        Integrals r;
        for (const Tri &t : tris) {
            const glm::vec3 &a = pts[t.a];
            const glm::vec3 &b = pts[t.b];
            const glm::vec3 &c = pts[t.c];

            const float v = glm::dot(a, glm::cross(b, c)) / 6.0f;
            const glm::vec3 s = a + b + c;

            r.volume += v;
            r.com += v * s * 0.25f; // tetrahedron centroid (the origin vertex contributes 0)
            r.cov += (v / 20.0f) * (glm::outerProduct(a, a) + glm::outerProduct(b, b) +
                                    glm::outerProduct(c, c) + glm::outerProduct(s, s));
        }
        if (std::abs(r.volume) > 1e-12f)
            r.com /= r.volume;
        return r;
    }
}

bool ConvexInertialFunc::IsExternal(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris, const glm::vec3 &pt) {
    bool isExternal = false;
    for (const auto &tri : tris) {
        const glm::vec3 &a = pts[tri.a];
        const glm::vec3 &b = pts[tri.b];
        const glm::vec3 &c = pts[tri.c];

        // if the point is in front of any triangle then its external
        float dist = ConvexFunc::DistanceFromTriangle(a, b, c, pt);
        if (dist > 0.0f) {
            isExternal = true;
            break;
        }
    }

    return isExternal;
}

glm::vec3 ConvexInertialFunc::CalculateCenterOfMass(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris) {
    // constexpr int numSamples = 100;
    //
    // Bounds bounds;
    // bounds.Expand(pts.data(), pts.size());
    //
    // glm::vec3 cm(0.0f);
    // const float dx = bounds.WidthX() / static_cast<float>(numSamples);
    // const float dy = bounds.WidthY() / static_cast<float>(numSamples);
    // const float dz = bounds.WidthZ() / static_cast<float>(numSamples);
    //
    // int samplesCount = 0;
    // for (float x = bounds.GetMins().x; x < bounds.GetMaxs().x; x += dx)
    //     for (float y = bounds.GetMins().y; y < bounds.GetMaxs().y; y += dy)
    //         for (float z = bounds.GetMins().z; z < bounds.GetMaxs().z; z += dz) {
    //             glm::vec3 pt(x, y, z);
    //
    //             if (IsExternal(pts, tris, pt))
    //                 continue;
    //
    //             cm += pt;
    //             samplesCount++;
    //         }
    //
    // cm /= static_cast<float>(samplesCount);
    // return cm;

    return Integrate(pts, tris).com;
}

glm::mat3 ConvexInertialFunc::CalculateInertiaTensor(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris,
    const glm::vec3 &cm) {
    // constexpr int numSamples = 100;
    //
    // Bounds bounds;
    // bounds.Expand(pts.data(), static_cast<int>(pts.size()));
    //
    // glm::mat3 tensor(0.0f);
    //
    // const float dx = bounds.WidthX() / static_cast<float>(numSamples);
    // const float dy = bounds.WidthY() / static_cast<float>(numSamples);
    // const float dz = bounds.WidthZ() / static_cast<float>(numSamples);
    //
    // int samplesCount = 0;
    // for (float x = bounds.GetMins().x; x < bounds.GetMaxs().x; x += dx)
    //     for (float y = bounds.GetMins().y; y < bounds.GetMaxs().y; y += dy)
    //         for (float z = bounds.GetMins().z; z < bounds.GetMaxs().z; z += dz) {
    //             glm::vec3 pt(x, y, z);
    //
    //             if (IsExternal(pts, tris, pt))
    //                 continue;
    //
    //             // get the point relative to the mass
    //             pt -= cm;
    //
    //             tensor[0][0] += pt.y * pt.y + pt.z * pt.z;
    //             tensor[1][1] += pt.x * pt.x + pt.z * pt.z;
    //             tensor[2][2] += pt.x * pt.x + pt.y * pt.y;
    //
    //             tensor[0][1] += -1.0f * pt.x * pt.y;
    //             tensor[0][2] += -1.0f * pt.x * pt.z;
    //             tensor[1][2] += -1.0f * pt.y * pt.z;
    //
    //             tensor[1][0] += -1.0f * pt.x * pt.y;
    //             tensor[2][0] += -1.0f * pt.x * pt.z;
    //             tensor[2][1] += -1.0f * pt.y * pt.z;
    //
    //             samplesCount++;
    //         }
    //
    // tensor *= 1.0f / static_cast<float>(samplesCount);
    // return tensor;

    const Integrals in = Integrate(pts, tris);

    // degenerate mesh: identity keeps glm::inverse(inertia) valid
    if (std::abs(in.volume) < 1e-12f) return {1.0f};

    // move the second moment from the origin to cm:
    // C_cm = C_0 - V (c mᵀ + m cᵀ) + V m mᵀ, where c is the true center of mass
    glm::mat3 cov = in.cov
                  - in.volume * (glm::outerProduct(in.com, cm) + glm::outerProduct(cm, in.com))
                  + in.volume * glm::outerProduct(cm, cm);
    cov /= in.volume; // per unit mass (invMass is applied elsewhere)

    // I = trace(C) * Identity - C
    const float trace = cov[0][0] + cov[1][1] + cov[2][2];
    return trace * glm::mat3(1.0f) - cov;
}