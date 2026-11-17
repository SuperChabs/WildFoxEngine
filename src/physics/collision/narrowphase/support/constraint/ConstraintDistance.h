#pragma once

#include "Constraint.h"

class ConstraintDistance : public Constraint {
    math::MatMN m_Jacobian;
    math::VecN m_cachedLambda;
    float m_baumgarte;

public:
    ConstraintDistance() : Constraint(), m_Jacobian(1, 12), m_cachedLambda(1) {
        m_cachedLambda.Zero();
        m_baumgarte = 0.0f;
    }

    void PreSolve(float dt) override;
    void Solve() override;
    void PostSolve() override;
};
