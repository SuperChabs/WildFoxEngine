#include "Physics.h"

#include "physics/shapeFunc/ShapeFunctions.h"

ConvexHull::ConvexHull(const std::vector<glm::vec3> &points, const int num) {
    Shape::Build(this, points, num);
}

glm::mat3 ColliderComponent::GetInertiaTensor() const {
    return std::visit([](const auto& s){ return Shape::InertiaTensor(s); }, shape);
}

Bounds ColliderComponent::GetBounds(const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale) {
    return std::visit([&](const auto& s) {
        using T = std::decay_t<decltype(s)>;

        return Shape::GetBounds(s, pos, rot, scale);
    }, shape);
}

Bounds ColliderComponent::GetBounds() {
    return std::visit([](const auto& s) { return Shape::GetBounds(s); }, shape);
}

glm::vec3 ColliderComponent::Support(const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot,
    const glm::vec3 &scale, float bias) {
    return std::visit([&](const auto& s) {
        return Shape::Support(s, dir, pos, rot, scale, bias);
    }, shape);
}

float ColliderComponent::FastestLinearSpeed(const glm::vec3 &angularVelocity, const glm::vec3 &dir) const {
    return std::visit([&](const auto& s){ return Shape::FastestLinearSpeed(s, angularVelocity, dir); }, shape);
}

bool ColliderComponent::GenerateConvexHull(const std::vector<glm::vec3> &points, const int maxVerts) {
    ConvexHull hull;
    if (!Shape::Build(&hull, points, static_cast<int>(points.size()), maxVerts))
        return false;

    shape = std::move(hull);
    return true;
}
