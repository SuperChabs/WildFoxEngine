#pragma once

#include <string>
#include <memory>

#include "core/IModule.h"
#include "core/logging/Logger.h"
#include "ECS/World.h"
#include "scene/serializer/SceneSerializer.h"
#include "scene/SceneManager.h"

class SceneModule : public IModule {
    ECSWorld *ecs = nullptr;
    std::unique_ptr<SceneSerializer> m_Serializer;
    std::unique_ptr<SceneManager> m_SceneManager;

public:
    SceneModule(ECSWorld *world)
        : ecs(world) {
    }

    bool Initialize() override {
        try {
            m_Serializer = std::make_unique<SceneSerializer>(ecs);
            m_SceneManager = std::make_unique<SceneManager>(ecs);

            Logger::Log(LogLevel::INFO, "SceneModule initialized");

            isInitialized = true;

            return true;
        } catch (const std::exception &e) {
            Logger::Log(LogLevel::ERROR, "SceneModule initialization failed: " + std::string(e.what()));

            isInitialized = false;

            return false;
        }
    }

    void Update(float deltaTime) override {
    }

    void Shutdown() override {
        m_Serializer.reset();
        m_SceneManager.reset();
    }

    /// @name IModule interface
    /// {@
    const char *GetName() const override { return "Scene"; }
    int GetPriority() const override { return 90; }
    bool IsRequired() const override { return true; }
    /// @}

    SceneSerializer *GetSceneSerializer() { return m_Serializer.get(); }
    SceneManager *GetSceneManager() { return m_SceneManager.get(); }
};