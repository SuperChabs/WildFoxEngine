#pragma once

#include "ECS/World.h"
#include "physics/pipeline/IPhysicsStage.h"

class GatherStage : public IPhysicsStage {
    ECSWorld& m_world;

public:
    explicit GatherStage(ECSWorld &world) : m_world(world) {}

    void Execute(StepContext &ctx) override;

    std::string GetName() override { return "Gather"; }
};
