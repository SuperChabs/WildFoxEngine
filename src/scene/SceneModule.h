#pragma once

#include <memory>

#include "core/IModule.h"
#include "ECS/World.h"
#include "scene/serializer/SceneSerializer.h"
#include "scene/SceneManager.h"

class SceneModule : public IModule {
    ECSWorld *ecs = nullptr;
    std::unique_ptr<SceneSerializer> m_Serializer;
    std::unique_ptr<SceneManager> m_SceneManager;

public:
    SceneModule(ECSWorld *world);

    bool Initialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    /// @name IModule interface
    /// {@
    const char *GetName() const override;

    int GetPriority() const override;

    bool IsRequired() const override;

    /// @}

    SceneSerializer *GetSceneSerializer();

    SceneManager *GetSceneManager();
};