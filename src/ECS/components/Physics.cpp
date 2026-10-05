#include "Physics.h"

#include "physics/shapeFunc/ShapeFunctions.h"

ConvexHull::ConvexHull(const std::vector<glm::vec3> &points, const int num) {
    Shape::Build(this, points, num);
}

glm::mat3 ColliderComponent::GetInertiaTensor() const {
    return std::visit([](const auto& s){ return Shape::InertiaTensor(s); }, shape);
}

Bounds ColliderComponent::GetBounds(const glm::vec3 &pos, const glm::quat &rot) {
    return std::visit([&](const auto& s) { return Shape::GetBounds(s, pos, rot); }, shape);
}

Bounds ColliderComponent::GetBounds() {
    return std::visit([](const auto& s) { return Shape::GetBounds(s); }, shape);
}

glm::vec3 ColliderComponent::Support(const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, float bias) {
    return std::visit([&](const auto& s){ return Shape::Support(s, dir, pos, rot, bias); }, shape);
}