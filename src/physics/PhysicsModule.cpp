#include "PhysicsModule.h"

PhysicsModule::PhysicsModule(ECSWorld *ecs)
    : m_ecs(ecs) {
}

bool PhysicsModule::OnInitialize() {
    try {
        m_physics = std::make_unique<Physics>(*m_ecs);
        return true;
    } catch (...) {
        return false;
    }
}

void PhysicsModule::Update(const float deltaTime) {
    if (m_paused && !m_stepOnce) return;

    m_accumulatedTime += std::min(deltaTime, 0.25f);
    while (m_accumulatedTime >= FIXED_DT) {
        const float subDt = FIXED_DT / static_cast<float>(m_substeps);
        for (int i = 0; i < m_substeps; i++)
            m_physics->Simulate(subDt);
        m_accumulatedTime -= FIXED_DT;
    }
    m_stepOnce = false;
}

void PhysicsModule::Shutdown() {
    m_physics.reset();
}