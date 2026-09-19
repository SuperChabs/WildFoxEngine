#pragma once

#include <glm/vec3.hpp>

class Bounds {
    glm::vec3 m_mins;
    glm::vec3 m_maxs;

public:
    Bounds() { Clear(); }
    Bounds(const Bounds &rhs) : m_mins(rhs.m_mins), m_maxs(rhs.m_maxs) {}
    ~Bounds() = default;

    const Bounds &operator=(const Bounds &rhs);

    void Expand(const glm::vec3 *pts, int num);
    void Expand(const glm::vec3 &rhs);
    void Expand(const Bounds &rhs);

    [[nodiscard]] float WidthX() const { return m_maxs.x - m_mins.x; }
    [[nodiscard]] float WidthY() const { return m_maxs.y - m_mins.y; }
    [[nodiscard]] float WidthZ() const { return m_maxs.z - m_mins.z; }

    void Clear() { m_mins = glm::vec3(1e6); m_maxs = glm::vec3(-1e6); }

    [[nodiscard]] bool DoesInteract(const Bounds &rhs) const;

    void SetMins(const glm::vec3 &mins) { m_mins = mins; }
    void SetMaxs(const glm::vec3 &maxs) { m_maxs = maxs; }

    [[nodiscard]] glm::vec3 GetMins() const { return m_mins; }
    [[nodiscard]] glm::vec3 GetMaxs() const { return m_maxs; }
};