#pragma once
#include "Constraint.h"

class ConstraintPenetration : public Constraint {
public:
    math::VecN m_cachedLambda;
    glm::vec3 m_normal;

    math::MatMN m_Jacobian;

    float m_baumgarte;
    float m_friction;

    ConstraintPenetration() : Constraint(), m_cachedLambda(3), m_Jacobian(3, 12) {
        m_cachedLambda.Zero();
        m_baumgarte = 0.0f;
        m_friction = 0.0f;
    }

    void PreSolve(float dt) override;
    void Solve() override;
};
