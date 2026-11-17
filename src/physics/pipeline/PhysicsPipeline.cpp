#include "PhysicsPipeline.h"

void PhysicsPipeline::Step(StepContext &ctx) const {
    for (auto &stage : stages)
        if (stage->GetActive())
            stage->Execute(ctx);
}
