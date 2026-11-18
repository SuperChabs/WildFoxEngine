#pragma once

#include "math/types/MatMN.h"
#include "physics/core/PhysicsContext.h"

class Constraint {
public:
    BodyInfo *m_bodyA = nullptr;
    BodyInfo *m_bodyB = nullptr;

    glm::vec3 m_anchorA{0.0f};           // the anchor location in bodyA's space
    glm::vec3 m_axisA{1.0f, 0.0f, 0.0f}; // the axis direction in bodyA's space

    glm::vec3 m_anchorB{0.0f};           // the anchor location in bodyB's space
    glm::vec3 m_axisB{1.0f, 0.0f, 0.0f}; // the axis direction in bodyB's space

    glm::quat m_q0 = glm::identity<glm::quat>(); // the initial relative quaternion q1^-1 * q2

    virtual ~Constraint() = default;

    virtual void PreSolve(float dt) = 0;
    virtual void Solve()            = 0;
    virtual void PostSolve() {}

    static glm::mat4 Left(const glm::quat &q);
    static glm::mat4 Right(const glm::quat &q);

protected:
    [[nodiscard]] math::MatMN GetInverseMassMatrix() const;
    [[nodiscard]] math::VecN  GetVelocitíes()        const;
    void ApplyImpulse(const math::VecN &impulses)    const;
};
