#pragma once

#include "ConstraintConstantVelocity.h"
#include "ConstraintConstantVelocityLimited.h"
#include "ConstraintOrientation.h"
#include "Constraint.h"
#include "ConstraintDistance.h"
#include "ConstraintHingeQuat.h"
#include "ConstraintHingeQuatLimited.h"

inline std::unique_ptr<Constraint> CreateConstraint(const c_Joint &j, BodyInfo &a, BodyInfo &b) {
    std::unique_ptr<Constraint> joint;

    switch (j.type) {
        case JointType::DISTANCE: joint = std::make_unique<ConstraintDistance>();
            break;
        case JointType::ORIENTATION: joint = std::make_unique<ConstraintOrientation>();
            break;
        case JointType::CONSTANT_VELOCITY: joint = std::make_unique<ConstraintConstantVelocity>();
            break;
        case JointType::CONSTANT_VELOCITY_LIMITED: joint = std::make_unique<ConstraintConstantVelocityLimited>();
            break;
        case JointType::HINGE: joint = std::make_unique<ConstraintHingeQuat>();
            break;
        case JointType::HINGE_LIMITED: joint = std::make_unique<ConstraintHingeQuatLimited>();
            break;
        case JointType::NONE: return nullptr;
    }

    joint->m_bodyA = &a;
    joint->m_bodyB = &b;
    joint->m_anchorA = j.localAnchorA;
    joint->m_anchorB = j.localAnchorB;
    joint->m_q0 = j.q0;

    // the axis gets normalized by GetOrtho, so it must not be zero
    joint->m_axisA = glm::dot(j.axisA, j.axisA) > 1e-8f ? j.axisA : glm::vec3(1.0f, 0.0f, 0.0f);
    joint->m_axisB = glm::dot(j.axisB, j.axisB) > 1e-8f ? j.axisB : glm::vec3(1.0f, 0.0f, 0.0f);

    return joint;
}
