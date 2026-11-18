#pragma once

#include "Constraint.h"

class ConstraintHingeQuatLimited : public Constraint {
public:
    math::VecN m_cachedLambda;
    math::MatMN m_Jacobian;

    float m_baumgarte;

    bool m_isAngleViolated;
    float m_relativeAngle;

    ConstraintHingeQuatLimited() : Constraint(), m_cachedLambda(4), m_Jacobian(4, 12) {
        m_cachedLambda.Zero();
        m_baumgarte = 0.0f;
        m_isAngleViolated = false;
        m_relativeAngle = 0.0f;
    }

    void PreSolve(float dt) override;
    void Solve() override;
    void PostSolve() override;
};
