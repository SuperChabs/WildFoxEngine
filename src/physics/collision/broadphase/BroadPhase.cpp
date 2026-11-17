#include "BroadPhase.h"

#include "physics/geometry/Bounds.h"

void BroadPhase::SortBodiesBounds(std::vector<BodyInfo> &bodies, const int num, std::vector<PseudoBody> &sortedArray, const float dt) {
    auto axis = glm::vec3(1.0f);
    axis = glm::normalize(axis);

    for (int i = 0; i < num; i++) {
        const auto &body = bodies[i];
        auto &col = body.collider;
        const auto &trans = body.transform;
        const auto &rig = body.rigidBody;

        Bounds bounds = col->GetBounds(trans->position, trans->rotation, trans->scale);

        // expand the bounds by the linear velocity
        bounds.Expand(bounds.GetMins() + rig->m_linearVelocity * dt);
        bounds.Expand(bounds.GetMaxs() + rig->m_linearVelocity * dt);

        constexpr float epsilon = 0.1f;
        bounds.Expand(bounds.GetMins() + glm::vec3(-1.0f) * epsilon);
        bounds.Expand(bounds.GetMaxs() + glm::vec3(1.0f) * epsilon);

        sortedArray[i * 2].id = &bodies[i];
        sortedArray[i * 2].value = glm::dot(axis, bounds.GetMins());
        sortedArray[i * 2].isMin = true;

        sortedArray[i * 2 + 1].id = &bodies[i];
        sortedArray[i * 2 + 1].value = glm::dot(axis, bounds.GetMaxs());
        sortedArray[i * 2 + 1].isMin = false;
    }
    std::ranges::sort(sortedArray, {}, &PseudoBody::value);
}

void BroadPhase::BuildPairs(std::vector<CollisionPair> &collisionPairs, const std::vector<PseudoBody> &sortedArray, const int num) {
    collisionPairs.clear();

    for (int i = 0; i < num * 2; i++) {
        const auto &a = sortedArray[i];
        if (!a.isMin)
            continue;

        BodyInfo *fst = a.id;

        for (int j = i + 1; j < num * 2; j++) {
            const auto &b = sortedArray[j];
            if (b.id == fst) break;
            if (!b.isMin) continue;
            collisionPairs.emplace_back(fst, b.id);
        }
    }
}

void BroadPhase::SweepAndPrune1D(std::vector<BodyInfo> &bodies, std::vector<CollisionPair> &finalPairs, const int num,
        const float dt) {
    std::vector<PseudoBody> sortedBodies(num * 2);

    SortBodiesBounds(bodies, num, sortedBodies, dt);
    BuildPairs(finalPairs, sortedBodies, num);
}

void BroadPhase::BroadPhase(std::vector<BodyInfo> &bodies, std::vector<CollisionPair> &finalPairs, const int num,
        const float dt) {
    SweepAndPrune1D(bodies, finalPairs, num, dt);
}
