#include "ConstraintConstantVelocityLimited.h"

#include "math/LCP.h"
#include "math/Math.h"
#include "physics/dynamics/BodyTransform.h"

void ConstraintConstantVelocityLimited::PreSolve(float dt) {
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
    glm::vec3 cv = m_axisA;
    math::GetOrtho(cv, u, v);

    glm::mat4 P;
    P[0] = {0, 0, 0, 0};
    P[1] = {0, 1, 0, 0};
    P[2] = {0, 0, 1, 0};
    P[3] = {0, 0, 0, 1};
    glm::mat4 P_T = glm::transpose(P);

    const glm::mat4 matA = P * Left(q1_inv) * Right(q2 * q0_inv) * P_T * -0.5f;
    const glm::mat4 matB = P * Left(q1_inv) * Right(q2 * q0_inv) * P_T * 0.5f;

    const float pi = acosf(-1.0f);
    const glm::quat qr = q1_inv * q2;
    const glm::quat qrr = qr * q0_inv;
    m_angleU = 2.0f * asinf(glm::clamp(glm::dot({qrr.x, qrr.y, qrr.z}, u), -1.0f, 1.0f)) * 180.0f / pi;
    m_angleV = 2.0f * asinf(glm::clamp(glm::dot({qrr.x, qrr.y, qrr.z}, v), -1.0f, 1.0f)) * 180.0f / pi;

    m_isAngleViolatedU = false;
    if (m_angleU > 45.0f || m_angleU < -45.0f)
        m_isAngleViolatedU = true;

    m_isAngleViolatedV = false;
    if (m_angleV > 45.0f || m_angleV < -45.0f)
        m_isAngleViolatedV = true;

    //
    // First row is the primary distance constraint that holds the anchor points together
    //
    m_Jacobian.Zero();

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

    {
        const int idx = 1;

        glm::vec4 tmp;

        J1 = {0, 0, 0};
        m_Jacobian.rows[1][0] = J1.x;
        m_Jacobian.rows[1][1] = J1.y;
        m_Jacobian.rows[1][2] = J1.z;

        tmp = matA * glm::vec4{0, cv.x, cv.y, cv.z};
        J2 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[1][3] = J2.x;
        m_Jacobian.rows[1][4] = J2.y;
        m_Jacobian.rows[1][5] = J2.z;

        J3 = {0, 0, 0};
        m_Jacobian.rows[1][6] = J3.x;
        m_Jacobian.rows[1][7] = J3.y;
        m_Jacobian.rows[1][8] = J3.z;

        tmp = matB * glm::vec4{0, cv.x, cv.y, cv.z};
        J4 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[1][ 9] = J4.x;
        m_Jacobian.rows[1][10] = J4.y;
        m_Jacobian.rows[1][11] = J4.z;
    }

    if (m_isAngleViolatedU) {
        const int idx = 1;
        glm::vec4 tmp;

        J1 = {0, 0, 0};
        m_Jacobian.rows[2][0] = J1.x;
        m_Jacobian.rows[2][1] = J1.y;
        m_Jacobian.rows[2][2] = J1.z;

        tmp = matA * glm::vec4{0, u.x, u.y, u.z};
        J2 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[2][3] = J2.x;
        m_Jacobian.rows[2][4] = J2.y;
        m_Jacobian.rows[2][5] = J2.z;

        J3 = {0, 0, 0};
        m_Jacobian.rows[2][6] = J3.x;
        m_Jacobian.rows[2][7] = J3.y;
        m_Jacobian.rows[2][8] = J3.z;

        tmp = matB * glm::vec4{0, u.x, u.y, u.z};
        J4 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[2][ 9] = J4.x;
        m_Jacobian.rows[2][10] = J4.y;
        m_Jacobian.rows[2][11] = J4.z;
    }

    if (m_isAngleViolatedV) {
        const int idx = 1;
        glm::vec4 tmp;

        J1 = {0, 0, 0};
        m_Jacobian.rows[3][0] = J1.x;
        m_Jacobian.rows[3][1] = J1.y;
        m_Jacobian.rows[3][2] = J1.z;

        tmp = matA * glm::vec4{0, v.x, v.y, v.z};
        J2 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[3][3] = J2.x;
        m_Jacobian.rows[3][4] = J2.y;
        m_Jacobian.rows[3][5] = J2.z;

        J3 = {0, 0, 0};
        m_Jacobian.rows[3][6] = J3.x;
        m_Jacobian.rows[3][7] = J3.y;
        m_Jacobian.rows[3][8] = J3.z;

        tmp = matB * glm::vec4{0, v.x, v.y, v.z};
        J4 = {tmp[idx], tmp[idx + 1], tmp[idx + 2]};
        m_Jacobian.rows[3][ 9] = J4.x;
        m_Jacobian.rows[3][10] = J4.y;
        m_Jacobian.rows[3][11] = J4.z;
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

void ConstraintConstantVelocityLimited::Solve() {
    const math::MatMN JacobianTranspose = m_Jacobian.Transpose();

    // build the system of equations
    const math::VecN q_dt = GetVelocitíes();
    const math::MatMN invMassMatrix = GetInverseMassMatrix();
    const math::MatMN J_W_Jt = m_Jacobian * invMassMatrix * JacobianTranspose;
    math::VecN rhs = m_Jacobian * q_dt * -1.0f;
    rhs[0] -= m_baumgarte;

    // solve for the Lagrange multipliers
    math::VecN lambdaN = math::GaussSeidel(J_W_Jt, rhs);

    // clamp the torque from the angle constraint
    // we need to make sure it's restorative angle
    if (m_isAngleViolatedU) {
        if (m_angleU > 0.0f)
            lambdaN[2] = std::min(0.0f, lambdaN[2]);
        if (m_angleU < 0.0f)
            lambdaN[2] = std::max(0.0f, lambdaN[2]);
    }
    if (m_isAngleViolatedV) {
        if (m_angleV > 0.0f)
            lambdaN[3] = std::min(0.0f, lambdaN[3]);
        if (m_angleV < 0.0f)
            lambdaN[3] = std::max(0.0f, lambdaN[3]);
    }

    // apply the impulses
    const math::VecN impulses = JacobianTranspose * lambdaN;
    ApplyImpulse(impulses);

    // accumulate the impulses for warm starting
    m_cachedLambda += lambdaN;
}

void ConstraintConstantVelocityLimited::PostSolve() {
    // limit the warm starting to the reasonable limits
    for (int i = 0; i < m_cachedLambda.m_num; i++) {
        if (i > 0)
            m_cachedLambda[i] = 0.0f;

        if (m_cachedLambda[i] * 0.0f != m_cachedLambda[i] * 0.0f)
            m_cachedLambda[i] = 0.0f;

        constexpr float limit = 20.0f;
        if (m_cachedLambda[i] > limit)
            m_cachedLambda[i] = limit;
        if (m_cachedLambda[i] < -limit)
            m_cachedLambda[i] = -limit;
    }
}
