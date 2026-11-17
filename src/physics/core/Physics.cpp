#include "Physics.h"

#include "physics/pipeline/stages/Stages.h"

Physics::Physics(ECSWorld &world) : m_world(world) {
    m_pipeline.AddStage<GatherStage>(m_world);
    m_pipeline.AddStage<ApplyForcesStage>();
    m_pipeline.AddStage<CollisionStage>();
    m_pipeline.AddStage<SolveStage>();
}

void Physics::Simulate(const float dt) {
    stepContext.Clear();

    stepContext.dt = dt;
    stepContext.gravity = m_gravity;

    m_pipeline.Step(stepContext);
}
