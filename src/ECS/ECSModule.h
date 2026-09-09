#pragma once

#include <memory>

#include "core/IModule.h"
#include "ECS/World.h"

class ECSModule : public IModule {
    std::unique_ptr<ECSWorld> ecsWorld;

public:
    bool Initialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    /// @name IModule interface
    /// @{
    [[nodiscard]] const char *GetName() const override;
    [[nodiscard]] int GetPriority() const override;
    [[nodiscard]] bool IsRequired() const override;

    /// @}

    ECSWorld *GetECS() const;
};