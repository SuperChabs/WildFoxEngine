#pragma once

#include "physics/pipeline/IPhysicsStage.h"

class SolveStage : public IPhysicsStage {
public:
    void Execute(StepContext &ctx) override;

    std::string GetName() override { return "SolveStage"; }
};