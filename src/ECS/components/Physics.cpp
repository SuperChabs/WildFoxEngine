#include "Physics.h"

#include "physics/collision/narrowphase/support/SupportFunc.h"
#include "physics/dynamics/MassProperties.h"
#include "physics/geometry/ShapeFunctions.h"


ConvexHull::ConvexHull(const std::vector<glm::vec3> &points, const int num) {
    ShapeFunc::Build(this, points, num);
}

glm::mat3 c_Collider::GetInertiaTensor() const {
    return std::visit([](const auto& s) {
        return MassProperties::InertiaTensor(s);
    }, shape);
}

Bounds c_Collider::GetBounds(const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale) {
    return std::visit([&](const auto& s) {
        return ShapeFunc::GetBounds(s, pos, rot, scale);
    }, shape);
}

Bounds c_Collider::GetBounds() {
    return std::visit([](const auto& s) {
        return ShapeFunc::GetBounds(s);
    }, shape);
}

glm::vec3 c_Collider::Support(const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot,
    const glm::vec3 &scale, float bias) {
    return std::visit([&](const auto& s) {
        return Support::Support(s, dir, pos, rot, scale, bias);
    }, shape);
}

float c_Collider::FastestLinearSpeed(const glm::vec3 &angularVelocity, const glm::vec3 &dir) const {
    return std::visit([&](const auto& s) {
        return ShapeFunc::FastestLinearSpeed(s, angularVelocity, dir);
    }, shape);
}

bool c_Collider::GenerateConvexHull(const std::vector<glm::vec3> &points, const int maxVerts) {
    ConvexHull hull;
    if (!ShapeFunc::Build(&hull, points, static_cast<int>(points.size()), maxVerts))
        return false;

    shape = std::move(hull);
    return true;
}

glm::vec3 c_Collider::GetCenterOfMass() {
    return std::visit([&](const auto& s){ return s.m_centerOfMass; }, shape);
}