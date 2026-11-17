#include "Constraint.h"
#include "physics/dynamics/MassProperties.h"
#include "physics/solver/ContactSolver.h"

using namespace math;

MatMN Constraint::GetInverseMassMatrix() const {
    MatMN invMassMatrix(12, 12);
    invMassMatrix.Zero();

    invMassMatrix.rows[0][0] = m_bodyA->rigidBody->m_invMass;
    invMassMatrix.rows[1][1] = m_bodyA->rigidBody->m_invMass;
    invMassMatrix.rows[2][2] = m_bodyA->rigidBody->m_invMass;

    glm::mat3 invInertiaA = MassProperties::GetInverseInertiaTensorWorldSpace(*m_bodyA);
    for (int i = 0; i < 3; i++) {
        invMassMatrix.rows[3 + i][3] = invInertiaA[0][i]; // glm has [col][rows] so its correct
        invMassMatrix.rows[3 + i][4] = invInertiaA[1][i];
        invMassMatrix.rows[3 + i][5] = invInertiaA[2][i];
    }

    invMassMatrix.rows[6][6] = m_bodyB->rigidBody->m_invMass;
    invMassMatrix.rows[7][7] = m_bodyB->rigidBody->m_invMass;
    invMassMatrix.rows[8][8] = m_bodyB->rigidBody->m_invMass;

    glm::mat3 invInertiaB = MassProperties::GetInverseInertiaTensorWorldSpace(*m_bodyB);
    for (int i = 0; i < 3; i++) {
        invMassMatrix.rows[9 + i][9]  = invInertiaB[0][i]; // glm has [col][rows] so its correct
        invMassMatrix.rows[9 + i][10] = invInertiaB[1][i];
        invMassMatrix.rows[9 + i][11] = invInertiaB[2][i];
    }

    return invMassMatrix;
}

VecN Constraint::GetVelocitíes() const {
    VecN q_dt(12);

    q_dt[0] = m_bodyA->rigidBody->m_linearVelocity.x;
    q_dt[1] = m_bodyA->rigidBody->m_linearVelocity.y;
    q_dt[2] = m_bodyA->rigidBody->m_linearVelocity.z;

    q_dt[3] = m_bodyA->rigidBody->m_angularVelocity.x;
    q_dt[4] = m_bodyA->rigidBody->m_angularVelocity.y;
    q_dt[5] = m_bodyA->rigidBody->m_angularVelocity.z;

    q_dt[6] = m_bodyB->rigidBody->m_linearVelocity.x;
    q_dt[7] = m_bodyB->rigidBody->m_linearVelocity.y;
    q_dt[8] = m_bodyB->rigidBody->m_linearVelocity.z;

    q_dt[9]  = m_bodyB->rigidBody->m_angularVelocity.x;
    q_dt[10] = m_bodyB->rigidBody->m_angularVelocity.y;
    q_dt[11] = m_bodyB->rigidBody->m_angularVelocity.z;

    return q_dt;
}

void Constraint::ApplyImpulse(const VecN &impulses) const {
    glm::vec3 forceInternalA{0.0f};
    glm::vec3 torqueInternalA{0.0f};
    glm::vec3 forceInternalB{0.0f};
    glm::vec3 torqueInternalB{0.0f};

    forceInternalA[0] = impulses[0];
    forceInternalA[1] = impulses[1];
    forceInternalA[2] = impulses[2];

    torqueInternalA[0] = impulses[3];
    torqueInternalA[1] = impulses[4];
    torqueInternalA[2] = impulses[5];

    forceInternalB[0] = impulses[6];
    forceInternalB[1] = impulses[7];
    forceInternalB[2] = impulses[8];

    torqueInternalB[0] = impulses[9];
    torqueInternalB[1] = impulses[10];
    torqueInternalB[2] = impulses[11];

    const auto &rb_a = m_bodyA->rigidBody;
    const auto &rb_b = m_bodyB->rigidBody;

    ContactSolver::ApplyImpulseLinear(*rb_a, forceInternalA);
    ContactSolver::ApplyImpulseAngular(*m_bodyA, torqueInternalA);

    ContactSolver::ApplyImpulseLinear(*rb_b, forceInternalB);
    ContactSolver::ApplyImpulseAngular(*m_bodyB, torqueInternalB);
}

glm::mat4 Constraint::Left(const glm::quat &q) {
    glm::mat4 L;
    L[0] = {q.w, -q.x, -q.y,  -q.z};
    L[1] = {q.x,  q.w, -q.z,   q.y};
    L[2] = {q.y,  q.z,  q.w,  -q.x};
    L[3] = {q.z, -q.y,  q.x,   q.w};
    return glm::transpose(L);
}

glm::mat4 Constraint::Right(const glm::quat &q) {
    glm::mat4 R;
    R[0] = {q.w, -q.x, -q.y,  -q.z};
    R[1] = {q.x,  q.w,  q.z,  -q.y};
    R[2] = {q.y, -q.z,  q.w,   q.x};
    R[3] = {q.z,  q.y, -q.x,   q.w};
    return glm::transpose(R);
}
