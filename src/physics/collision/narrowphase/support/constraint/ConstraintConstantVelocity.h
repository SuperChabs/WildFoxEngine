#pragma once

#include "Constraint.h"

class ConstraintConstantVelocity: public Constraint {
public:
    math::VecN m_cachedLambda;
    math::MatMN m_Jacobian;

    float m_baumgarte;

    ConstraintConstantVelocity() : Constraint(), m_cachedLambda(2), m_Jacobian(2, 12) {
        m_cachedLambda.Zero();
        m_baumgarte = 0.0f;
    }

    void PreSolve(float dt) override;
    void Solve() override;
    void PostSolve() override;
};
