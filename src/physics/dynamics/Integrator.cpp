#include "Integrator.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "BodyTransform.h"

void Integrator::IntegrateBody(const BodyInfo &body, const float dt) {
    auto &t = body.transform;
    auto &r = body.rigidBody;
    const auto &c = body.collider;

    t->position += r->m_linearVelocity * dt;

    const glm::vec3 positionCM = BodyTransform::GetCenterOfMassWorldSpace(body);
    const glm::vec3 cmToPos = t->position - positionCM;

    const glm::mat3 orientation = glm::mat3_cast(t->rotation);
    const glm::mat3 inertiaTensor = orientation * c->GetInertiaTensor() * glm::transpose(orientation);
    const glm::vec3 alpha = glm::inverse(inertiaTensor) * (-glm::cross(r->m_angularVelocity, inertiaTensor * r->m_angularVelocity));
    r->m_angularVelocity += alpha * dt;

    const glm::vec3 dAngle = r->m_angularVelocity * dt;
    const float angle = glm::length(dAngle);

    glm::quat dq(1.0f, 0.0f, 0.0f, 0.0f);
    if (angle > 1e-8f) {
        dq = glm::angleAxis(angle, dAngle / angle);
    }

    t->rotation = glm::normalize(dq * t->rotation);
    t->position = positionCM + dq * cmToPos;;
}
