#include "MassProperties.h"

#include <glm/gtc/quaternion.hpp>

glm::mat3 MassProperties::InertiaTensor(const Sphere &s) {
    return {2.0f * s.m_radius * s.m_radius / 5.0f};
}

glm::mat3 MassProperties::InertiaTensor(const Box &s) {
    // inertia tensor for box centered around zero
    const float dx = s.m_bounds.GetMaxs().x - s.m_bounds.GetMins().x;
    const float dy = s.m_bounds.GetMaxs().y - s.m_bounds.GetMins().y;
    const float dz = s.m_bounds.GetMaxs().z - s.m_bounds.GetMins().z;

    glm::mat3 tensor(0.0f);
    tensor[0][0] = (dy * dy + dz * dz) / 12.0f;
    tensor[1][1] = (dx * dx + dz * dz) / 12.0f;
    tensor[2][2] = (dx * dx + dy * dy) / 12.0f;

    // now we need to use the parallel axis theorem to get the inertia tensor for a box
    // that is not centered around the origin

    glm::vec3 cm;
    cm.x = (s.m_bounds.GetMins().x + s.m_bounds.GetMaxs().x) * 0.5f;
    cm.y = (s.m_bounds.GetMins().y + s.m_bounds.GetMaxs().y) * 0.5f;
    cm.z = (s.m_bounds.GetMins().z + s.m_bounds.GetMaxs().z) * 0.5f;

    const glm::vec3 R = glm::vec3(0.0f) - cm; // the displacement from center of mass to the origin
    const float R2 = glm::dot(R, R);
    glm::mat3 patTensor(R2);
    // patTensor[0] = glm::vec3(R2 - R.x * R.x,      R.y * R.y,      R.z * R.z);
    // patTensor[1] = glm::vec3(     R.x * R.x, R2 - R.y * R.y,      R.z * R.z);
    // patTensor[2] = glm::vec3(     R.x * R.x,      R.y * R.y, R2 - R.z * R.z);
    patTensor -= glm::outerProduct(R, R);

    tensor += patTensor;
    return tensor;
}

glm::mat3 MassProperties::InertiaTensor(const ConvexHull &s) {
    return s.m_inertiaTensor;
}

glm::mat3 MassProperties::GetInverseInertiaTensorBodySpace(const glm::mat3 &inertia, const float &invMass) {
    return glm::inverse(inertia) * invMass;
}

glm::mat3 MassProperties::GetInverseInertiaTensorWorldSpace(const BodyInfo &info) {
    const auto &inertia = info.collider->GetInertiaTensor();
    const auto &invMass = info.rigidBody->m_invMass;
    const auto &rotation = info.transform->rotation;

    glm::mat3 invInertiaTensor = glm::inverse(inertia) * invMass;
    const glm::mat3 rot = glm::mat3_cast(rotation);
    invInertiaTensor = rot * invInertiaTensor * glm::transpose(rot);
    return invInertiaTensor;
}
