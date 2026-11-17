#include "ApplyForcesStage.h"

#include "physics/solver/ContactSolver.h"

void ApplyForcesStage::Execute(StepContext &ctx) {
    for (const auto &body : ctx.bodies) {
        auto &r = *body.rigidBody;
        if (r.m_invMass != 0.0f) {
            const float mass = 1.0f / r.m_invMass;
            ContactSolver::ApplyImpulseLinear(r, ctx.gravity * mass * ctx.dt);
        }
    }
}
