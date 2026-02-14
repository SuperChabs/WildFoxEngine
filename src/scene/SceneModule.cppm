module;

#include <memory>

export module WFE.Scene.SceneModule;

import WFE.Core.IModule;
import WFE.Core.Logger;
import WFE.ECS.ECSWorld;
import WFE.Scene.SceneSurializer;

export class SceneModule : public IModule
{
    ECSWorld* ecs = nullptr;
    std::unique_ptr<SceneSerializer> serializer;

public:
    SceneModule(ECSWorld* world)
        : ecs(world)
    {}

    bool Initialize() override
    {
        try
        {
            serializer = std::make_unique<SceneSerializer>(ecs);
            Logger::Log(LogLevel::INFO, "SceneModule initialized");
            isInitialized = true;
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, "SceneModule initialization failed: " + std::string(e.what()));
            isInitialized = false;
            return false;
        }
    }

    void Update(float deltaTime) override {}

    void Shutdown() override
    {
        serializer.reset();
    }

    /// @name IModule interface
    /// {@
    const char* GetName() const override { return "Scene"; }
    int GetPriority() const override { return 90; }
    bool IsRequired() const override { return true; }
    /// @}

    SceneSerializer* GetSerializer() { return serializer.get(); }
};