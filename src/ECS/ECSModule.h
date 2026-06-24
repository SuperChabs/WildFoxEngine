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
    const char *GetName() const override;

    int GetPriority() const override;

    bool IsRequired() const override;

    /// @}

    ECSWorld *GetECS();
};