module;

#include <exception>
#include <memory>

export module WFE.ECS.ECSModule;

import WFE.Core.IModule;
import WFE.Core.Logger;
import WFE.ECS.ECSWorld;

export class ECSModule : public IModule
{
    std::unique_ptr<ECSWorld> ecsWorld;

public:
    bool Initialize() override
    {
        try 
        {
            ecsWorld = std::make_unique<ECSWorld>();

            Logger::Log(LogLevel::INFO, "Successfully initialized ECS module");

            isInitialized = true;

            return true;
        } 
        catch (std::exception& e) 
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception creating ecs module: " + std::string(e.what()));

            isInitialized = false;

            return false;
        }
    }

    void Update(float deltaTime) override {}

    void Shutdown() override
    {
        ecsWorld.reset();
    }

    /// @name IModule interface
    /// @{
    const char* GetName() const override { return "ECS"; }
    int GetPriority() const override { return 10; }
    bool IsRequired() const override { return true; }
    /// @}

    ECSWorld* GetECS() { return ecsWorld.get(); }
};