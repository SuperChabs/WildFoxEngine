#pragma once
#include <glm/glm.hpp>

namespace math {
    inline void GetOrtho(glm::vec3& n, glm::vec3& u, glm::vec3& v) {
        n = glm::normalize(n);

        const glm::vec3 w = (n.z * n.z > 0.9f * 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
        u = glm::cross(n, w);
        u = glm::normalize(u);

        v = glm::cross(n, w);
        v = glm::normalize(v);
        u = glm::cross(n, v);
        u = glm::normalize(u);
    }

    inline float GetMagnitude(const glm::vec3 &n) {
        float mag = n.x * n.x + n.y * n.y + n.z * n.z;
        mag = glm::sqrt(mag);
        return mag;
    }

    inline glm::vec3 ScreenToWorldPoint(const glm::vec2 &screenPos, const glm::mat4 &view, const glm::mat4 &projection,
                                        const glm::vec4 &viewport) {
        glm::vec3 win(screenPos.x, viewport.w - screenPos.y, 0.0);
        glm::vec3 point = glm::unProject(win, view, projection, viewport);
        return point;
    }
}
