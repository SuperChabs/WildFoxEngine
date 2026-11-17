#include "SolveStage.h"

#include "physics/dynamics/Integrator.h"
#include "physics/solver/ContactSolver.h"

void SolveStage::Execute(StepContext &ctx) {
    if (ctx.numContacts > 1)
        std::ranges::sort(ctx.contacts,
            [](const ContactInfo &a, const ContactInfo &b) {
              return a.timeOfImpact < b.timeOfImpact;
            });

    // solve constraints
    for (const auto &c : ctx.constraints)
        c->PreSolve(ctx.dt);

    ctx.manifolds.PreSolve(ctx.dt);

    constexpr int maxIters = 5;
    for (int iters = 0; iters < maxIters; iters++) {
        for (const auto &c : ctx.constraints)
            c->Solve();

        ctx.manifolds.Solve();
    }

    for (const auto &c : ctx.constraints)
        c->PostSolve();

    ctx.manifolds.PostSolve();

    // apply ballistic impulses
    float accumulatedTime = 0.0f;
    for (auto &contact : ctx.contacts) {
        const float dtStep = contact.timeOfImpact - accumulatedTime;

        if (contact.bodyA->rigidBody->m_invMass == 0.0f && contact.bodyB->rigidBody->m_invMass == 0.0f) continue;

        for (const auto &b : ctx.bodies)
            Integrator::IntegrateBody(b, dtStep);

        ContactSolver::ResolveContact(contact);
        accumulatedTime += dtStep;
    }

    // update the position for the rest of this frame's time
    const float timeRemaining = ctx.dt - accumulatedTime;
    if (timeRemaining > 0.0f) {
        for (const auto &b : ctx.bodies)
            Integrator::IntegrateBody(b, timeRemaining);
    }
}
