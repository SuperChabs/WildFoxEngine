#pragma once

#include <memory>

#include <GLFW/glfw3.h>

#include "core/IModule.h"
#include "core/ModuleManager.h"
#include "UI/ImGuiManager.h"
#include "ECS/World.h"
#include "scene/SceneManager.h"
#include "DebugOverlay.h"
#include "UI/EditorCamera.h"

class InputControllerSystem;

class UIModule : public IModule {
    std::unique_ptr<ImGuiManager>             m_imGuiManager;
    std::unique_ptr<DebugOverlay>             m_debugOverlay;
    std::unique_ptr<PhysicsDebugRenderSystem> m_physicsDebugSystem;
    std::unique_ptr<EditorCamera>             m_editorCamera;
    std::unique_ptr<UICameraController>       m_cameraController;

    ECSWorld      *ecs            = nullptr;
    ModuleManager *mm             = nullptr;
    GLFWwindow    *window         = nullptr;
    SceneManager  *m_sceneManager = nullptr;

public:
    UIModule(ECSWorld *ecs, SceneManager *sceneManager, ModuleManager *mm, GLFWwindow *window);

    bool OnInitialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    void MouseCallback(GLFWwindow *window, double xPos, double yPos) const;

    /// @name IModule interface
    /// @{
    [[nodiscard]] const char *GetName() const override;
    [[nodiscard]] bool IsRequired() const override;
    /// }@

    [[nodiscard]] DebugOverlay *GetDebugOverlay() const { return m_debugOverlay.get(); }
};
