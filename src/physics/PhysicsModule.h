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
    PhysicsModule(ECSWorld *ecs);


    bool Initialize() override;


    void Update(float deltaTime) override;


    void Shutdown() override;


    /// @name IModule interface
    /// @{
    const char *GetName() const override;

    int GetPriority() const override;

    bool IsRequired() const override;

    /// @}

    PhysicsSystem *GetPhysics();
};