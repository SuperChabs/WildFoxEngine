#pragma once

#include <string>
#include <memory>
#include <vector>

#include "core/IModule.h"
#include "ECS/systems/PhysicsSystem.h"
#include "ECS/World.h"

class PhysicsModule : public IModule {
    std::unique_ptr<PhysicsSystem> m_physics;
    ECSWorld *m_ecs = nullptr;

public:
    PhysicsModule(ECSWorld *ecs)
        : m_ecs(ecs) {
    }

    bool Initialize() override {
        try {
            m_physics = std::make_unique<PhysicsSystem>();
            isInitialized = true;
            return true;
        } catch (...) {
            isInitialized = false;
            return false;
        }
    }

    void Update(float deltaTime) override {
        m_physics->Update(*m_ecs, deltaTime);
    }

    void Shutdown() override {
        m_physics.reset();
    }

    /// @name IModule interface
    /// @{
    const char *GetName() const override { return "Physics"; }
    int GetPriority() const override { return 80; }
    bool IsRequired() const override { return true; }
    /// @}

    PhysicsSystem *GetPhysics() { return m_physics.get(); }
};
