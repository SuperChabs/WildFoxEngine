#include "PhysicsModule.h"

PhysicsModule::PhysicsModule(ECSWorld *ecs)
    : m_ecs(ecs) {
}

bool PhysicsModule::Initialize() {
    try {
        m_physics = std::make_unique<Physics>(*m_ecs);
        m_isInitialized = true;
        return true;
    } catch (...) {
        m_isInitialized = false;
        return false;
    }
}

void PhysicsModule::Update(const float deltaTime) {
    if (m_paused && !m_stepOnce) return;

    const float subDt = deltaTime / static_cast<float>(m_substeps);
    for (int i = 0; i < m_substeps; i++)
        m_physics->Simulate(subDt);

    m_stepOnce = false;
}

void PhysicsModule::Shutdown() {
    m_physics.reset();
}