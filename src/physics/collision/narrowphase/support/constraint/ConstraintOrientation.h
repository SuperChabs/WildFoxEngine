#pragma once

#include "Constraint.h"

class ConstraintOrientation : public Constraint {
public:
    glm::quat m_q0; // the initial relative quaternion q1^-1 * q2

    math::MatMN m_Jacobian;

    float m_baumgarte;

    ConstraintOrientation() : Constraint(), m_Jacobian(4, 12) {
        m_baumgarte = 0.0f;
    }

    void PreSolve(float dt) override;
    void Solve() override;
};
