#include "Bounds.h"

const Bounds& Bounds::operator=(const Bounds &rhs) {
    m_mins = rhs.m_mins;
    m_maxs = rhs.m_maxs;
    return *this;
}

void Bounds::Expand(const glm::vec3 *pts, const int num) {
    for (int i = 0; i < num; i++)
        Expand(pts[i]);
}

void Bounds::Expand(const glm::vec3 &rhs) {
    if (rhs.x < m_mins.x) m_mins.x = rhs.x;
    if (rhs.y < m_mins.y) m_mins.y = rhs.y;
    if (rhs.z < m_mins.z) m_mins.z = rhs.z;

    if (rhs.x > m_maxs.x) m_maxs.x = rhs.x;
    if (rhs.y > m_maxs.y) m_maxs.y = rhs.y;
    if (rhs.z > m_maxs.z) m_maxs.z = rhs.z;
}

void Bounds::Expand(const Bounds &rhs) {
    Expand(rhs.m_mins);
    Expand(rhs.m_maxs);
}

bool Bounds::DoesInteract(const Bounds &rhs) const {
    if (m_maxs.x < rhs.m_mins.x || m_maxs.y < rhs.m_maxs.y || m_maxs.z < rhs.m_maxs.z) return false;
    if (m_maxs.x > rhs.m_mins.x || m_maxs.y > rhs.m_maxs.y || m_maxs.z > rhs.m_maxs.z) return false;
    return true;
}
