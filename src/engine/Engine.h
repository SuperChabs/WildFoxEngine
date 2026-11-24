#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <GLFW/glfw3.h>

#include "EngineCommandHandler.h"
#include "application/Application.h"
#include "ECS/systems/Systems.h"
#include "core/ModuleManager.h"
#include "rendering/RenderingModule.h"
#include "resource/ResourceModule.h"
#include "UI/UIModule.h"
#include "ECS/ECSModule.h"
#include "scene/SceneModule.h"
#include "physics/PhysicsModule.h"
#include "scripting/ScriptModule.h"

/// @file Engine.h
/// @brief Engine class
/// @author SuperChabs

/**
 * 
 */
class Engine : public Application {
    //std::unique_ptr<AudioSystem> audioSystem;

    std::unique_ptr<EditorCommandHandler> m_ech;

    ModuleManager   *mm{};

    CoreModule      *m_coreModule{};
    RenderingModule *m_renderingModule{};
    ResourceModule  *m_resourceModule{};
    UIModule        *m_uiModule{};
    ECSModule       *m_ecsModule{};
    ScriptModule    *m_scriptModule{};
    SceneModule     *m_sceneModule{};
    PhysicsModule   *m_physicsModule{};

    entt::entity gameCam = entt::null;

    static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
    static void MouseCallback(GLFWwindow *window, double xPos, double yPos);
    // static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

protected:
    /**
     * @brief Initialize game engine
     * Initialize Modules and other stuff that wasn't initialized
     * in Application class
     * @return false if a required module failed to initialize
     */
    bool OnInitialize() override;

    void OnSimulation(float deltaTime) override;

    void OnUpdate(float deltaTime) override;

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
    Engine(int w, int h, char* &title);

private:
    /// @brief Registers and initializes a module
    /// @return the module, or nullptr if a required module failed to initialize
    template<typename T, typename... Args>
    T *InitModule(Args &&... args);

    void ProcessInput();

    void RegistrateCoreCommands();
};