#include "ConvexInertialFunc.h"

#include "../ConvexFunc.h"
#include "../../../Bounds.h"

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
    constexpr int numSamples = 100;

    Bounds bounds;
    bounds.Expand(pts.data(), pts.size());

    glm::vec3 cm(0.0f);
    const float dx = bounds.WidthX() / static_cast<float>(numSamples);
    const float dy = bounds.WidthY() / static_cast<float>(numSamples);
    const float dz = bounds.WidthZ() / static_cast<float>(numSamples);

    int samplesCount = 0;
    for (float x = bounds.GetMins().x; x < bounds.GetMaxs().x; x += dx)
        for (float y = bounds.GetMins().y; y < bounds.GetMaxs().y; y += dy)
            for (float z = bounds.GetMins().z; z < bounds.GetMaxs().z; z += dz) {
                glm::vec3 pt(x, y, z);

                if (IsExternal(pts, tris, pt))
                    continue;

                cm += pt;
                samplesCount++;
            }

    cm /= static_cast<float>(samplesCount);
    return cm;
}

glm::mat3 ConvexInertialFunc::CalculateInertiaTensor(const std::vector<glm::vec3> &pts, const std::vector<Tri> &tris,
    const glm::vec3 &cm) {
    constexpr int numSamples = 100;

    Bounds bounds;
    bounds.Expand(pts.data(), static_cast<int>(pts.size()));

    glm::mat3 tensor = glm::mat3();

    const float dx = bounds.WidthX() / static_cast<float>(numSamples);
    const float dy = bounds.WidthY() / static_cast<float>(numSamples);
    const float dz = bounds.WidthZ() / static_cast<float>(numSamples);

    int samplesCount = 0;
    for (float x = bounds.GetMins().x; x < bounds.GetMaxs().x; x += dx)
        for (float y = bounds.GetMins().y; y < bounds.GetMaxs().y; y += dy)
            for (float z = bounds.GetMins().z; z < bounds.GetMaxs().z; z += dz) {
                glm::vec3 pt(x, y, z);

                if (IsExternal(pts, tris, pt))
                    continue;

                // get the point relative to the mass
                pt -= cm;

                tensor[0][0] += pt.y * pt.y + pt.z * pt.z;
                tensor[1][1] += pt.x * pt.x + pt.z * pt.z;
                tensor[2][2] += pt.x * pt.x + pt.y * pt.y;

                tensor[0][1] += -1.0f * pt.x * pt.y;
                tensor[0][2] += -1.0f * pt.x * pt.z;
                tensor[1][2] += -1.0f * pt.y * pt.z;

                tensor[1][0] += -1.0f * pt.x * pt.y;
                tensor[2][0] += -1.0f * pt.x * pt.z;
                tensor[2][1] += -1.0f * pt.y * pt.z;

                samplesCount++;
            }

    tensor += 1.0f / static_cast<float>(samplesCount);
    return tensor;
}
/** Inertia tensor:
             Row 0              Row 1              Row 2
            ┌─────────────────┬─────────────────┬─────────────────┐
 Column 0   │ tensor[0][0]    │ tensor[0][1]    │ tensor[0][2]    │
            │ Ixx = y² + z²   │ Ixy = -xy       │ Ixz = -xz       │
            ├─────────────────┼─────────────────┼─────────────────┤
 Column 1   │ tensor[1][0]    │ tensor[1][1]    │ tensor[1][2]    │
            │ Iyx = -xy       │ Iyy = x² + z²   │ Iyz = -yz       │
            ├─────────────────┼─────────────────┼─────────────────┤
 Column 2   │ tensor[2][0]    │ tensor[2][1]    │ tensor[2][2]    │
            │ Izx = -xz       │ Izy = -yz       │ Izz = x² + y²   │
            └─────────────────┴─────────────────┴─────────────────┘

 In matrix notation:

              ┌ y² + z²    -xy       -xz  ┐
      I = Σ   │ -xy        x² + z²   -yz  │
              │ -xz        -yz       x²+y²│
              └                           ┘


 Note: GLM uses column-major indexing:
       tensor[column][row]

 The point is first translated relative to the center of mass:
      pt -= cm;
*/