#pragma once
#include "Constraint.h"

class ConstraintConstantVelocityLimited : public Constraint {
public:
    math::VecN m_cachedLambda;
    math::MatMN m_Jacobian;

    float m_baumgarte;

    bool m_isAngleViolatedU;
    bool m_isAngleViolatedV;

    float m_angleU;
    float m_angleV;

    ConstraintConstantVelocityLimited() : Constraint(), m_cachedLambda(4), m_Jacobian(4, 12) {
        m_cachedLambda.Zero();
        m_baumgarte = 0.0f;
        m_isAngleViolatedU = false;
        m_isAngleViolatedV = false;
        m_angleU = 0.0f;
        m_angleV = 0.0f;
    }

    void PreSolve(float dt) override;
    void Solve() override;
    void PostSolve() override;
};
