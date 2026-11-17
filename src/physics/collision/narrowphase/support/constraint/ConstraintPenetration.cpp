#include "ConstraintPenetration.h"

#include "math/LCP.h"
#include "math/Math.h"
#include "physics/dynamics/BodyTransform.h"

void ConstraintPenetration::PreSolve(float dt) {
    // get the world space position of the hinge from A's orientation
    const glm::vec3 worldAnchorA = BodyTransform::BodySpaceToWorldSpace(*m_bodyA, m_anchorA);

    // get the world space position of the hinge from A's orientation
    const glm::vec3 worldAnchorB = BodyTransform::BodySpaceToWorldSpace(*m_bodyB, m_anchorB);

    const glm::vec3 ra = worldAnchorA - BodyTransform::GetCenterOfMassWorldSpace(*m_bodyA);
    const glm::vec3 rb = worldAnchorB - BodyTransform::GetCenterOfMassWorldSpace(*m_bodyB);
    const glm::vec3 a = worldAnchorA;
    const glm::vec3 b = worldAnchorB;

    const float frictionA = m_bodyA->rigidBody->m_friction;
    const float frictionB = m_bodyB->rigidBody->m_friction;
    m_friction = frictionA * frictionB;

    glm::vec3 u;
    glm::vec3 v;
    math::GetOrtho(m_normal, u, v);

    // convert tangent space from model space to world space
    glm::vec3 normal = m_bodyA->transform->rotation * m_normal;
    u = m_bodyA->transform->rotation * u;
    v = m_bodyA->transform->rotation * v;

    //
    // Penetration Constraint
    //
    m_Jacobian.Zero();

    // first how is the primary distance constraint that holds the anchor points together
    glm::vec3 J1 = normal * -1.0f;
    m_Jacobian.rows[0][0] = J1.x;
    m_Jacobian.rows[0][1] = J1.y;
    m_Jacobian.rows[0][2] = J1.z;

    glm::vec3 J2 = glm::cross(ra, J1);
    m_Jacobian.rows[0][3] = J2.x;
    m_Jacobian.rows[0][4] = J2.y;
    m_Jacobian.rows[0][5] = J2.z;

    glm::vec3 J3 = normal * 1.0f;
    m_Jacobian.rows[0][6] = J3.x;
    m_Jacobian.rows[0][7] = J3.y;
    m_Jacobian.rows[0][8] = J3.z;

    glm::vec3 J4 = glm::cross(rb, J3);
    m_Jacobian.rows[0][ 9] = J4.x;
    m_Jacobian.rows[0][10] = J4.y;
    m_Jacobian.rows[0][11] = J4.z;

    //
    // Friction Jacobian
    //
    if (m_friction > 0.0f) {
        J1 = u * -1.0f;
        m_Jacobian.rows[1][0] = J1.x;
        m_Jacobian.rows[1][1] = J1.y;
        m_Jacobian.rows[1][2] = J1.z;
        J2 = glm::cross(ra, J1);
        m_Jacobian.rows[1][3] = J2.x;
        m_Jacobian.rows[1][4] = J2.y;
        m_Jacobian.rows[1][5] = J2.z;
        J3 = u * 1.0f;
        m_Jacobian.rows[1][6] = J3.x;
        m_Jacobian.rows[1][7] = J3.y;
        m_Jacobian.rows[1][8] = J3.z;
        J4 = glm::cross(rb, J3);
        m_Jacobian.rows[1][ 9] = J4.x;
        m_Jacobian.rows[1][10] = J4.y;
        m_Jacobian.rows[1][11] = J4.z;
    }
    if (m_friction > 0.0f) {
        J1 = v * -1.0f;
        m_Jacobian.rows[2][0] = J1.x;
        m_Jacobian.rows[2][1] = J1.y;
        m_Jacobian.rows[2][2] = J1.z;
        J2 = glm::cross(ra, J1);
        m_Jacobian.rows[2][3] = J2.x;
        m_Jacobian.rows[2][4] = J2.y;
        m_Jacobian.rows[2][5] = J2.z;
        J3 = v * 1.0f;
        m_Jacobian.rows[2][6] = J3.x;
        m_Jacobian.rows[2][7] = J3.y;
        m_Jacobian.rows[2][8] = J3.z;
        J4 = glm::cross(rb, J3);
        m_Jacobian.rows[2][ 9] = J4.x;
        m_Jacobian.rows[2][10] = J4.y;
        m_Jacobian.rows[2][11] = J4.z;
    }

    // apply warm starting from last frame
    const math::VecN impulses = m_Jacobian.Transpose() * m_cachedLambda;
    ApplyImpulse(impulses);

    // calculate the baumgarte stabilization
    float C = glm::dot(b - a, normal);
    C = std::min(0.0f, C + 0.02f); // add slop
    constexpr float Beta = 0.25f;
    m_baumgarte = Beta * C / dt;
}

void ConstraintPenetration::Solve() {
    const math::MatMN JacobianTranspose = m_Jacobian.Transpose();

    // build the system of equation
    const math::VecN q_dt = GetVelocitíes();
    const math::MatMN invMassMatrix = GetInverseMassMatrix();
    const math::MatMN J_W_Jt = m_Jacobian * invMassMatrix * JacobianTranspose;
    math::VecN rhs = m_Jacobian * q_dt * -1.0f;
    rhs[0] -= m_baumgarte;

    // solve for the Lagrange multipliers
    math::VecN lambdaN = math::GaussSeidel(J_W_Jt, rhs);

    // accumulate the impulses and clamp to within the constraint limits
    const math::VecN oldLambda = m_cachedLambda;
    m_cachedLambda += lambdaN;
    constexpr float lambdaLimit = 0.0f;
    if (m_cachedLambda[0] < lambdaLimit)
        m_cachedLambda[0] = lambdaLimit;
    if (m_friction > 0.0f) {
        const float umg = m_friction * 10.0f * 1.0f/(m_bodyA->rigidBody->m_invMass + m_bodyB->rigidBody->m_invMass);
        const float normalForce = fabsf(lambdaN[0] * m_friction);
        const float maxForce = (umg > normalForce) ? umg : normalForce;

        if (m_cachedLambda[1] > maxForce)
            m_cachedLambda[1] = maxForce;

        if (m_cachedLambda[1] < -maxForce)
            m_cachedLambda[1] = -maxForce;

        if (m_cachedLambda[2] > maxForce)
            m_cachedLambda[2] = maxForce;

        if (m_cachedLambda[2] < -maxForce)
            m_cachedLambda[2] = -maxForce;
    }
    lambdaN = m_cachedLambda - oldLambda;

    // apply the impulses
    const math::VecN impulses = JacobianTranspose * lambdaN;
    ApplyImpulse(impulses);
}
