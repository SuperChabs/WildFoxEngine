#pragma once

#include <string>
#include <memory>

#include <entt/entt.hpp>
#include <GLFW/glfw3.h>

#include "EditorCamera.h"
#include "EngineCommandHandler.h"
#include "application/Application.h"
#include "ECS/systems/Systems.h"
#include "ECS/components/Components.h"
#include "core/ModuleManager.h"
#include "rendering/RenderingModule.h"
#include "resource/ResourceModule.h"
#include "UI/UIModule.h"
#include "ECS/ECSModule.h"
#include "scene/SceneModule.h"
#include "physics/PhysicsModule.h"
#include "scripting/ScriptModule.h"

/// @file Engine.cppm
/// @brief Engine class
/// @author SuperChabs

/**
 * 
 */
class Engine : public Application {
    std::unique_ptr<InputControllerSystem> inputControllerSystem;
    std::unique_ptr<PhysicsDebugRenderSystem> physicsDebugSystem;

    //std::unique_ptr<AudioSystem> audioSystem;

    std::unique_ptr<EditorCommandHandler> m_ech;

    ModuleManager *mm;
    RenderingModule *renderingModule;
    ResourceModule *resourceModule;
    UIModule *uiModule;
    ECSModule *ecsModule;
    ScriptModule *m_scriptModule;
    SceneModule *sceneModule;
    PhysicsModule *m_physicsModule;

    bool cameraControlEnabled;
    bool showUI;

    entt::entity gameCam = entt::null;
    EditorCamera editorCam;

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
         * @brief Shutdown classes needed to shut down by hand
         */
    void OnShutdown() override;

    bool ShouldAllowCameraControl() const override;

public:
    Engine(int w, int h, const std::string &title);

private:
    void InitializeAS();

    void ProcessInput();

    void RegistraterCoreCommands();
};