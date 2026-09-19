#include "Bounds.h"

const Bounds& Bounds::operator=(const Bounds &rhs) {
    this->mins = rhs.mins;
    this->maxs = rhs.maxs;
    return *this;
}

void Bounds::Expand(const glm::vec3 *pts, const int num) {
    for (int i = 0; i < num; i++)
        Expand(pts[i]);
}

void Bounds::Expand(const glm::vec3 &rhs) {
    if (rhs.x < mins.x) mins.x = rhs.x;
    if (rhs.y < mins.y) mins.y = rhs.y;
    if (rhs.z < mins.z) mins.z = rhs.z;

    if (rhs.x > maxs.x) maxs.x = rhs.x;
    if (rhs.y > maxs.y) maxs.y = rhs.y;
    if (rhs.z > maxs.z) maxs.z = rhs.z;
}

void Bounds::Expand(const Bounds &rhs) {
    Expand(rhs.mins);
    Expand(rhs.maxs);
}

bool Bounds::DoesInteract(const Bounds &rhs) const {
    if (this->maxs.x < rhs.mins.x || this->maxs.y < rhs.maxs.y || this->maxs.z < rhs.maxs.z)
        return false;
    if (this->maxs.x > rhs.mins.x || this->maxs.y > rhs.maxs.y || this->maxs.z > rhs.maxs.z)
        return false;
    return true;
}
