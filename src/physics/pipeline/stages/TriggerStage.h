#pragma once
#include "physics/pipeline/IPhysicsStage.h"

class TriggerStage : public IPhysicsStage {
    std::vector<TriggerPair> m_activeTriggers;

public:
    void Execute(StepContext &ctx) override;

    std::string GetName() override { return "TriggerStage"; }
};
