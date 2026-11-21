#include "GatherStage.h"

#include "physics/collision/narrowphase/support/constraint/ConstraintFactory.h"

void GatherStage::Execute(StepContext &ctx) {
    std::unordered_map<entt::entity, BodyInfo*> lookup;

    std::vector<BodyInfo> current;
    m_world.Each<c_RigidBody, c_Collider, c_Transform>(
    [&](const entt::entity e, c_RigidBody &r, c_Collider &c, c_Transform &t) {
        current.push_back(BodyInfo{&r, &t, &c, e});
    });

    const bool changed = current.size() != ctx.bodies.size() ||
        !std::equal(current.begin(), current.end(), ctx.bodies.begin(),
            [](const BodyInfo &a, const BodyInfo &b) {
            return a.rigidBody == b.rigidBody && a.transform == b.transform && a.collider == b.collider;
        });

    if (changed) {
        ctx.manifolds.Clear();
        ctx.bodies = current;
    }

    size_t i = 0;
    m_world.Each<c_RigidBody, c_Collider, c_Transform>(
    [&](const entt::entity e, c_RigidBody &, c_Collider &, c_Transform &) {
        lookup[e] = &ctx.bodies[i++];
    });

    m_world.Each<c_Joint>([&](const entt::entity, c_Joint &j) {
        auto a = lookup.find(j.bodyA);
        auto b = lookup.find(j.bodyB);
        if (a == lookup.end() || b == lookup.end()) return;

        // remember the relative orientation the joint was created in
        if (!j.q0Initialized) {
            j.q0 = glm::inverse(a->second->transform->rotation) * b->second->transform->rotation;
            j.q0Initialized = true;
        }

        if (auto constraint = CreateConstraint(j, *a->second, *b->second))
            ctx.constraints.emplace_back(std::move(constraint));
    });
}
