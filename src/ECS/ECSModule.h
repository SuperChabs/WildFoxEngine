#pragma once

#include <string>
#include <memory>

#include "core/IModule.h"
#include "core/logging/Logger.h"
#include "ECS/World.h"

class ECSModule : public IModule {
    std::unique_ptr<ECSWorld> ecsWorld;

public:
    bool Initialize() override {
        try {
            ecsWorld = std::make_unique<ECSWorld>();

            Logger::Log(LogLevel::INFO, "Successfully initialized ECS module");

            isInitialized = true;

            return true;
        } catch (std::exception &e) {
            Logger::Log(LogLevel::ERROR,
                        "Exception creating ecs module: " + std::string(e.what()));

            isInitialized = false;

            return false;
        }
    }

    void Update(float deltaTime) override {
    }

    void Shutdown() override {
        ecsWorld.reset();
    }

    /// @name IModule interface
    /// @{
    const char *GetName() const override { return "ECS"; }
    int GetPriority() const override { return 10; }
    bool IsRequired() const override { return true; }
    /// @}

    ECSWorld *GetECS() { return ecsWorld.get(); }
};