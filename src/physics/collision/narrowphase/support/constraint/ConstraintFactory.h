#pragma once

#include "physics/collision/narrowphase/support/constraint/Constraint.h"
#include "physics/collision/narrowphase/support/constraint/ConstraintDistance.h"
#include "physics/dynamics/BodyTransform.h"

inline std::unique_ptr<Constraint> CreateConstraint(const c_Joint &j, BodyInfo &a, BodyInfo &b) {
    auto joint = std::make_unique<ConstraintDistance>();

    joint->m_bodyA = &a;
    joint->m_bodyB = &b;
    joint->m_anchorA = j.localAnchorA;
    joint->m_anchorB = j.localAnchorB;

    return joint;
}