#include "CollisionStage.h"

#include "physics/collision/broadphase/BroadPhase.h"
#include "physics/collision/narrowphase/NarrowPhase.h"

void CollisionStage::Execute(StepContext &ctx) {
    // BroadPhase -----------------------------------------------------------------------------------------

    BroadPhase::BroadPhase(ctx.bodies, ctx.pairs, static_cast<int>(ctx.bodies.size()), ctx.dt);

    // NarrowPhase ----------------------------------------------------------------------------------------

    ctx.manifolds.RemoveExpired();
    for (const auto [fst, snd] : ctx.pairs) {

        if (fst->rigidBody->m_invMass == 0.0f && snd->rigidBody->m_invMass == 0.0f) continue;

        ContactInfo contact{};
        if (NarrowPhase::Intersect(*fst, *snd, contact, ctx.dt)) {
            if (0.0f == contact.timeOfImpact)
                // static contact
                ctx.manifolds.AddContact(contact);
            else {
                // ballistic contact
                ctx.contacts.push_back(contact);
                ctx.numContacts++;
            }
        }
    }
}