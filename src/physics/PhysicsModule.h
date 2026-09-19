#pragma once

#include <memory>

#include "core/IModule.h"
#include "Physics.h"
#include "ECS/World.h"

class PhysicsModule : public IModule {
    std::unique_ptr<Physics> m_physics;
    ECSWorld *m_ecs = nullptr;

public:
    explicit PhysicsModule(ECSWorld *ecs);

    bool Initialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    /// @name IModule interface
    /// @{
    [[nodiscard]] const char *GetName() const override { return "Physics"; }
    [[nodiscard]] int GetPriority() const override { return 80; }
    [[nodiscard]] bool IsRequired() const override { return true; }
    /// @}

    [[nodiscard]] Physics *GetPhysics() const { return m_physics.get(); }
};