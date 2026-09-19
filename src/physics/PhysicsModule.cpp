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
    m_physics->Simulate(deltaTime);
}

void PhysicsModule::Shutdown() {
    m_physics.reset();
}