#pragma once

#include <memory>

#include "core/IModule.h"
#include "core/Physics.h"
#include "ECS/World.h"

class PhysicsModule : public IModule {
    std::unique_ptr<Physics> m_physics;
    ECSWorld *m_ecs = nullptr;

    bool m_paused = false;
    bool m_stepOnce = false;
    int m_substeps = 1;

    float m_accumulatedTime = 0.0f;
    static constexpr float FIXED_DT = 1.0f / 60.0f;

public:
    explicit PhysicsModule(ECSWorld *ecs);

    bool OnInitialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    /// @name IModule interface
    /// @{
    [[nodiscard]] const char *GetName() const override { return "Physics"; }
    [[nodiscard]] bool IsRequired() const override { return true; }
    /// @}

    [[nodiscard]] Physics *GetPhysics() const { return m_physics.get(); }

    void SetPaused(bool paused) { m_paused = paused; }
    [[nodiscard]] bool IsPaused() const { return m_paused; }
    void RequestStep() { m_stepOnce = !m_stepOnce; }

    void SetSubsteps(int n) { m_substeps = std::max(1, n); }
    [[nodiscard]] int GetSubsteps() const { return m_substeps; }
};