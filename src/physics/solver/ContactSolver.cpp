#include "ContactSolver.h"

#include "physics/dynamics/MassProperties.h"
#include "physics/dynamics/BodyTransform.h"

void ContactSolver::ResolveContact(ContactInfo &contact) {
    auto &rb_a = contact.bodyA->rigidBody;
    auto &rb_b = contact.bodyB->rigidBody;
    auto &col_a  = contact.bodyA->collider;
    auto &col_b  = contact.bodyB->collider;
    auto &t_a  = contact.bodyA->transform;
    auto &t_b  = contact.bodyB->transform;

    const glm::vec3 comA = BodyTransform::GetCenterOfMassWorldSpace(*contact.bodyA);
    const glm::vec3 comB = BodyTransform::GetCenterOfMassWorldSpace(*contact.bodyB);

    const float elasticityA = rb_a->m_elasticity;
    const float elasticityB = rb_b->m_elasticity;
    const float elasticityTotal = elasticityA * elasticityB;

    const float invMassA = rb_a->m_invMass;
    const float invMassB = rb_b->m_invMass;

    // resolution: inelastic, linear-only impulse
    const float total = invMassA + invMassB;
    if (total == 0.0f) return;

    const glm::mat3 invWorldInertiaA = MassProperties::GetInverseInertiaTensorWorldSpace(*contact.bodyA);
    const glm::mat3 invWorldInertiaB = MassProperties::GetInverseInertiaTensorWorldSpace(*contact.bodyB);

    const glm::vec3 &n = contact.normal;

    const glm::vec3 ra = contact.ptOnA_WorldSpace - comA;
    const glm::vec3 rb = contact.ptOnB_WorldSpace - comB;

    const glm::vec3 angularJA = glm::cross(invWorldInertiaA * glm::cross(ra, n), ra);
    const glm::vec3 angularJB = glm::cross(invWorldInertiaB * glm::cross(rb, n), rb);
    const float angularFactor = glm::dot(n, angularJA + angularJB);

    // get the world space velocity of motion and rotation
    const glm::vec3 velA = rb_a->m_linearVelocity + glm::cross(rb_a->m_angularVelocity, ra);
    const glm::vec3 velB = rb_b->m_linearVelocity + glm::cross(rb_b->m_angularVelocity, rb);

    // calculate the collision impulse
    const glm::vec3 vab = velA - velB;
    const float impulseJ = (1.0f + elasticityTotal) * glm::dot(n, vab) / (total + angularFactor);
    const glm::vec3 vectorImpulseJ = n * impulseJ;

    ApplyImpulse(*contact.bodyA, contact.ptOnA_WorldSpace, vectorImpulseJ * -1.0f);
    ApplyImpulse(*contact.bodyB, contact.ptOnB_WorldSpace, vectorImpulseJ * 1.0f);

    //
    // calculate the impulse caused by friction
    //

    const float frictionA = rb_a->m_friction;
    const float frictionB = rb_b->m_friction;
    const float frictionTotal = frictionA * frictionB;

    // find the normal direction of the velocity with respect to the normal of the collision
    const glm::vec3 velNorm = n * glm::dot(vab, n);

    // find the normal direction of the velocity with respect to the normal of the collision
    const glm::vec3 velTang = vab - velNorm;

    const float tangentLenSq = glm::dot(velTang, velTang);
    if (tangentLenSq > 1e-8f) {
        // get the tangential velocities relative to the other body
        glm::vec3 relativeVelTang = glm::normalize(velTang);

        const glm::vec3 inertiaA = glm::cross(invWorldInertiaA * glm::cross(ra, relativeVelTang), ra);
        const glm::vec3 inertiaB = glm::cross(invWorldInertiaB * glm::cross(rb, relativeVelTang), rb);
        const float invInertia = glm::dot(relativeVelTang, inertiaA + inertiaB);

        // calculate the tangential impuls for friction
        const glm::vec3 tdir = glm::normalize(velTang);
        float jt = glm::length(velTang) / (total + invInertia);
        jt = std::min(jt, frictionTotal * std::abs(impulseJ));
        const glm::vec3 impulseFriction = tdir * jt;

        // apply kinetic friction
        ApplyImpulse(*contact.bodyA, contact.ptOnA_WorldSpace, impulseFriction * -1.0f);
        ApplyImpulse(*contact.bodyB, contact.ptOnB_WorldSpace, impulseFriction * 1.0f);
    }

    //
    // positional correction
    //

    constexpr float slop = 0.01f;
    constexpr float percent = 0.4f; // 0.2-0.8
    const float depth = -contact.separationDistance;
    const float correction = std::max(depth - slop, 0.0f) * percent;

    const float tA = invMassA / total;
    const float tB = invMassB / total;
    t_a->position += contact.normal * correction * tA;
    t_b->position -= contact.normal * correction * tB;
}

void ContactSolver::ApplyImpulse(const BodyInfo &body, const glm::vec3 &impulsePoint, const glm::vec3 & impulse) {
    if (body.rigidBody->m_invMass == 0.0f) return;

    // impulsePoint is the world space location of the application of the impulse
    // impulse is the world space direction and magnitude of the impulse
    ApplyImpulseLinear(*body.rigidBody, impulse);

    const glm::vec3 position = BodyTransform::GetCenterOfMassWorldSpace(body); // applying impulses must produce torques through the center of mass
    const glm::vec3 r = impulsePoint - position;
    const glm::vec3 dL = glm::cross(r, impulse); // this is in world space
    ApplyImpulseAngular(body, dL);
}

void ContactSolver::ApplyImpulseLinear(c_RigidBody &rb, const glm::vec3 &impulse) {
    rb.m_linearVelocity += impulse * rb.m_invMass;
}

void ContactSolver::ApplyImpulseAngular(const BodyInfo &body, const glm::vec3 &impulse) {
    if (body.rigidBody->m_invMass == 0.0f) return;

    body.rigidBody->m_angularVelocity +=
            MassProperties::GetInverseInertiaTensorWorldSpace(body) * impulse;

    constexpr float maxAngularSpeed = 30.0f;
    if (glm::length(body.rigidBody->m_angularVelocity) > maxAngularSpeed)
        body.rigidBody->m_angularVelocity = glm::normalize(body.rigidBody->m_angularVelocity) * maxAngularSpeed;
}