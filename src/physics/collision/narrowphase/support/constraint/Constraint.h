#pragma once

#include "math/types/MatMN.h"
#include "physics/core/PhysicsContext.h"

class Constraint {
public:
    BodyInfo *m_bodyA;
    BodyInfo *m_bodyB;

    glm::vec3 m_anchorA; // the anchor location in bodyA's space
    glm::vec3 m_axisA;   // the axis direction in bodyA's space

    glm::vec3 m_anchorB; // the anchor location in bodyB's space
    glm::vec3 m_axisB;   // the axis direction in bodyB's space

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
