//
// Created by tymur52 on 18.11.26.
//

#include "ConstraintHingeQuat.h"

#include "math/LCP.h"
#include "math/Math.h"
#include "physics/dynamics/BodyTransform.h"

void ConstraintHingeQuat::PreSolve(float dt) {
    // get the world space position of the hinge from A's orientation
    const glm::vec3 worldAnchorA = BodyTransform::BodySpaceToWorldSpace(*m_bodyA, m_anchorA);

    // get the world space position of the hinge from A's orientation
    const glm::vec3 worldAnchorB = BodyTransform::BodySpaceToWorldSpace(*m_bodyB, m_anchorB);

    const glm::vec3 r = worldAnchorB - worldAnchorA;
    const glm::vec3 ra = worldAnchorA - BodyTransform::GetCenterOfMassWorldSpace(*m_bodyA);
    const glm::vec3 rb = worldAnchorB - BodyTransform::GetCenterOfMassWorldSpace(*m_bodyB);
    const glm::vec3 a = worldAnchorA;
    const glm::vec3 b = worldAnchorB;

    // get the orientation information of the bodies
    const glm::quat q1 = m_bodyA->transform->rotation;
    const glm::quat q2 = m_bodyB->transform->rotation;
    const glm::quat q0_inv = glm::inverse(m_q0);
    const glm::quat q1_inv = glm::inverse(q1);

    // this axis is defined in the local space of bodyA
    glm::vec3 v;
    glm::vec3 u;
    glm::vec3 hingeAxis = m_axisA;
    math::GetOrtho(hingeAxis, u, v);

    glm::mat4 P;
    P[0] = {0, 0, 0, 0};
    P[1] = {0, 1, 0, 0};
    P[2] = {0, 0, 1, 0};
    P[3] = {0, 0, 0, 1};
    glm::mat4 P_T = glm::transpose(P);

    const glm::mat4 matA = P * Left(q1_inv) * Right(q2 * q0_inv) * P_T * -0.5f;
    const glm::mat4 matB = P * Left(q1_inv) * Right(q2 * q0_inv) * P_T * 0.5f;

    m_Jacobian.Zero();

    //
    // The distance constraint
    //

    glm::vec3 J1 = (a - b) * 2.0f;
    m_Jacobian.rows[0][0] = J1.x;
    m_Jacobian.rows[0][1] = J1.y;
    m_Jacobian.rows[0][2] = J1.z;

    glm::vec3 J2 = glm::cross(ra, J1);
    m_Jacobian.rows[0][3] = J2.x;
    m_Jacobian.rows[0][4] = J2.y;
    m_Jacobian.rows[0][5] = J2.z;

    glm::vec3 J3 = (b - a) * 2.0f;
    m_Jacobian.rows[0][6] = J3.x;
    m_Jacobian.rows[0][7] = J3.y;
    m_Jacobian.rows[0][8] = J3.z;

    glm::vec3 J4 = glm::cross(rb, J3);
    m_Jacobian.rows[0][ 9] = J4.x;
    m_Jacobian.rows[0][10] = J4.y;
    m_Jacobian.rows[0][11] = J4.z;

    //
    // The quaternion jacobians
    //

    const int idx = 1;
    glm::vec4 tmp;
    {
        J1 = {0, 0, 0};
        m_Jacobian.rows[1][0] = J1.x;
        m_Jacobian.rows[1][1] = J1.y;
        m_Jacobian.rows[1][2] = J1.z;

        tmp = matA * glm::vec4{0, u.x, u.y, u.z};
        J2 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[1][3] = J2.x;
        m_Jacobian.rows[1][4] = J2.y;
        m_Jacobian.rows[1][5] = J2.z;

        J3 = {0, 0, 0};
        m_Jacobian.rows[1][6] = J3.x;
        m_Jacobian.rows[1][7] = J3.y;
        m_Jacobian.rows[1][8] = J3.z;

        tmp = matB * glm::vec4{0, u.x, u.y, u.z};
        J4 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[1][ 9] = J4.x;
        m_Jacobian.rows[1][10] = J4.y;
        m_Jacobian.rows[1][11] = J4.z;
    }
    {
        J1 = {0, 0, 0};
        m_Jacobian.rows[2][0] = J1.x;
        m_Jacobian.rows[2][1] = J1.y;
        m_Jacobian.rows[2][2] = J1.z;

        tmp = matA * glm::vec4{0, v.x, v.y, v.z};
        J2 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[2][3] = J2.x;
        m_Jacobian.rows[2][4] = J2.y;
        m_Jacobian.rows[2][5] = J2.z;

        J3 = {0, 0, 0};
        m_Jacobian.rows[2][6] = J3.x;
        m_Jacobian.rows[2][7] = J3.y;
        m_Jacobian.rows[2][8] = J3.z;

        tmp = matB * glm::vec4{0, v.x, v.y, v.z};
        J4 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[2][ 9] = J4.x;
        m_Jacobian.rows[2][10] = J4.y;
        m_Jacobian.rows[2][11] = J4.z;
    }

    // apply warm starting from last frame
    const math::VecN impulses = m_Jacobian.Transpose() * m_cachedLambda;
    ApplyImpulse(impulses);

    // calculate the baumgarte stabilization
    float C = glm::dot(r, r);
    C = std::max(0.0f, C - 0.01f);
    constexpr float Beta = 0.05f;
    m_baumgarte = (Beta / dt) * C;
}

void ConstraintHingeQuat::Solve() {
    const math::MatMN JacobianTranspose = m_Jacobian.Transpose();

    // build the system of equations
    const math::VecN q_dt = GetVelocitíes();
    const math::MatMN invMassMatrix = GetInverseMassMatrix();
    const math::MatMN J_W_Jt = m_Jacobian * invMassMatrix * JacobianTranspose;
    math::VecN rhs = m_Jacobian * q_dt * -1.0f;
    rhs[0] -= m_baumgarte;

    // solve for the Lagrange multipliers
    const math::VecN lambdaN = math::GaussSeidel(J_W_Jt, rhs);

    // apply the impulses
    const math::VecN impulses = JacobianTranspose * lambdaN;
    ApplyImpulse(impulses);

    // accumulate the impulses for warm starting
    m_cachedLambda += lambdaN;
}

void ConstraintHingeQuat::PostSolve() {
    // limit the warm starting to the reasonable limits
    for (int i = 0; i < m_cachedLambda.m_num; i++) {
        if (m_cachedLambda[i] * 0.0f != m_cachedLambda[i] * 0.0f)
            m_cachedLambda[i] = 0.0f;

        constexpr float limit = 20.0f;
        if (m_cachedLambda[i] > limit)
            m_cachedLambda[i] = limit;
        if (m_cachedLambda[i] < -limit)
            m_cachedLambda[i] = -limit;
    }
}
