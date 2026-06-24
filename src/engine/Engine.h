#pragma once

#include <string>
#include <memory>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <entt/entt.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "application/Application.h"
#include "rendering/core/Framebuffer.h"
#include "scripting/ASBindings.h"
#include "ECS/systems/Systems.h"
#include "ECS/components/Components.h"
#include "EngineCommandHandler.h"
#include "core/ModuleManager.h"
#include "rendering/RenderingModule.h"
#include "resource/ResourceModule.h"
#include "UI/UIModule.h"
#include "ECS/ECSModule.h"
#include "scene/SceneModule.h"
#include "core/EventBus.h"
#include "UI/DebugOverlay.h"
#include "physics/PhysicsModule.h"

/// @file Engine.cppm
/// @brief Engine class
/// @author SuperChabs

/**
 * 
 */
class Engine : public Application {
private:
    std::unique_ptr<ScriptSystem> scriptSystem;
    std::unique_ptr<InputControllerSystem> inputControllerSystem;
    std::unique_ptr<PhysicsDebugRenderSystem> physicsDebugSystem;
    std::unique_ptr<AudioSystem> audioSystem;

    std::unique_ptr<EditorCommandHandler> ech;

    ModuleManager *mm;
    RenderingModule *renderingModule;
    ResourceModule *resourceModule;
    UIModule *uiModule;
    ECSModule *ecsModule;
    SceneModule *sceneModule;
    PhysicsModule *m_physicsModule;

    DebugOverlay m_overlay;

    bool cameraControlEnabled;
    bool showUI;

    entt::entity mainCameraEntity = entt::null;

    static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

    static void MouseCallback(GLFWwindow *window, double xpos, double ypos);

    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    void SetCameraControlMode(bool enabled);

protected:
    /**
     * @brief Initialize game engine
     * Initialize Modules and other stuff that wasn't initialized
     * in Application class
     */
    void OnInitialize() override;

    void OnUpdate(float deltaTime) override;

    void UpdateMainCamera();

    /**
         * @brief Render scene into framebuffer
         */
    void OnRender() override;

    /**
         * @brief Shutdown classes needed to shutdown by hand
         */
    void OnShutdown() override;

    bool ShouldAllowCameraControl() const override;

    void SetMainCameraEntity(entt::entity newMainCameraEntity);

public:
    Engine(int w, int h, const std::string &title);

private:
    void InitializeAS();

    void ProcessInput();

    void RegistraterCoreCommands();
};