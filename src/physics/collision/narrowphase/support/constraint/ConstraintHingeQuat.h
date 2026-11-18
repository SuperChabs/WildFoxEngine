#pragma once

#include "Constraint.h"

class ConstraintHingeQuat : public Constraint {
public:
    math::VecN m_cachedLambda;
    math::MatMN m_Jacobian;

    float m_baumgarte;

    ConstraintHingeQuat() : Constraint(), m_cachedLambda(3), m_Jacobian(3, 12) {
        m_cachedLambda.Zero();
        m_baumgarte = 0.0f;
    }

    void PreSolve(float dt) override;
    void Solve() override;
    void PostSolve() override;
};
