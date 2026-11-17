#pragma once

#include <vector>

#include "physics/core/PhysicsContext.h"

namespace BroadPhase {
    void SortBodiesBounds(std::vector<BodyInfo> &bodies, int num, std::vector<PseudoBody> &sortedArray, float dt);
    void BuildPairs(std::vector<CollisionPair> &collisionPairs, const std::vector<PseudoBody> &sortedArray, int num);
    void SweepAndPrune1D(std::vector<BodyInfo> &bodies, std::vector<CollisionPair> &finalPairs, int num, float dt);
    void BroadPhase(std::vector<BodyInfo> &bodies, std::vector<CollisionPair> &finalPairs, int num, float dt);
}
