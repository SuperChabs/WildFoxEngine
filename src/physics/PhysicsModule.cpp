#include "PhysicsModule.h"

PhysicsModule::PhysicsModule(ECSWorld *ecs)
    : m_ecs(ecs) {
}

bool PhysicsModule::Initialize() {
    try {
        m_physics = std::make_unique<PhysicsSystem>();
        isInitialized = true;
        return true;
    } catch (...) {
        isInitialized = false;
        return false;
    }
}

void PhysicsModule::Update(float deltaTime) {
    m_physics->Update(*m_ecs, deltaTime);
}

void PhysicsModule::Shutdown() {
    m_physics.reset();
}

const char *PhysicsModule::GetName() const {
    return "Physics";
}

int PhysicsModule::GetPriority() const {
    return 80;
}

bool PhysicsModule::IsRequired() const {
    return true;
}

PhysicsSystem *PhysicsModule::GetPhysics() {
    return m_physics.get();
}