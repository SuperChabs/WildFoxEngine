#include "ConstraintDistance.h"

#include "math/LCP.h"
#include "physics/dynamics/BodyTransform.h"

void ConstraintDistance::PreSolve(float dt) {
    // get the world space position of the hinge from A's orientation
    const glm::vec3 worldAnchorA = BodyTransform::BodySpaceToWorldSpace(*m_bodyA, m_anchorA);

    // get the world space position of the hinge from A's orientation
    const glm::vec3 worldAnchorB = BodyTransform::BodySpaceToWorldSpace(*m_bodyB, m_anchorB);

    const glm::vec3 r = worldAnchorB - worldAnchorA;
    const glm::vec3 ra = worldAnchorA - BodyTransform::GetCenterOfMassWorldSpace(*m_bodyA);
    const glm::vec3 rb = worldAnchorB - BodyTransform::GetCenterOfMassWorldSpace(*m_bodyB);
    const glm::vec3 a = worldAnchorA;
    const glm::vec3 b = worldAnchorB;

    m_Jacobian.Zero();

    const glm::vec3 J1 = (a - b) * 2.0f;
    m_Jacobian.rows[0][0] = J1.x;
    m_Jacobian.rows[0][1] = J1.y;
    m_Jacobian.rows[0][2] = J1.z;

    const glm::vec3 J2 = glm::cross(ra, J1);
    m_Jacobian.rows[0][3] = J2.x;
    m_Jacobian.rows[0][4] = J2.y;
    m_Jacobian.rows[0][5] = J2.z;

    const glm::vec3 J3 = r * 2.0f;
    m_Jacobian.rows[0][6] = J3.x;
    m_Jacobian.rows[0][7] = J3.y;
    m_Jacobian.rows[0][8] = J3.z;

    const glm::vec3 J4 = glm::cross(rb, J3);
    m_Jacobian.rows[0][ 9] = J4.x;
    m_Jacobian.rows[0][10] = J4.y;
    m_Jacobian.rows[0][11] = J4.z;

    // apply warm starting from last frame
    const math::VecN impulses = m_Jacobian.Transpose() * m_cachedLambda;
    ApplyImpulse(impulses);

    // calculate the baumgarte stabilization
    float C = glm::dot(r, r);
    C = std::max(0.0f, C - 0.01f);
    constexpr float Beta = 0.05f;
    m_baumgarte = (Beta / dt) * C;
}

void ConstraintDistance::Solve() {
    const math::MatMN JacobianTranspose = m_Jacobian.Transpose();

    // build the system of equation
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

void ConstraintDistance::PostSolve() {
    // limit the warm starting to reasonable limits
    if (m_cachedLambda[0] * 0.0f != m_cachedLambda[0] * 0.0f)
        m_cachedLambda[0] = 0.0f;

    constexpr float limit = 1e5f;
    if (m_cachedLambda[0] > limit)
        m_cachedLambda[0] = limit;
    if (m_cachedLambda[0] < -limit)
        m_cachedLambda[0] = -limit;
}
