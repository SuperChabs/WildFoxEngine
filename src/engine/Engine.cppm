module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <entt/entt.hpp>

#include <memory>
#include <string>
#include <variant>

export module WFE.Engine;

import WFE.Application.Application;
import WFE.Core.Input;
import WFE.Core.CommandManager;
import WFE.Core.Logger;
import WFE.Rendering.Core.Framebuffer;
import WFE.Scripting.ASBindings;
import WFE.ECS.Systems;
import WFE.ECS.Components;
import WFE.Engine.EditorCommandHandler;
import WFE.Core.ModuleManager;
import WFE.Rendering.RenderingModule;
import WFE.Resource.ResourceModule;
import WFE.UI.UIModule;
import WFE.ECS.ECSModule;
import WFE.Scene.SceneModule;
import WFE.Core.EventBus;
import WFE.UI.DebugOverlay;

/// @file Engine.cppm
/// @brief Engine class
/// @author SuperChabs

/**
 * 
 */
export class Engine : public Application 
{
private:
    std::unique_ptr<ScriptSystem> scriptSystem;
    std::unique_ptr<InputControllerSystem> inputControllerSystem;

    std::unique_ptr<EditorCommandHandler> ech;

    ModuleManager* mm;
    RenderingModule* renderingModule;
    ResourceModule* resourceModule;
    UIModule* uiModule;
    ECSModule* ecsModule;
    SceneModule* sceneModule;

    DebugOverlay m_overlay;
    SceneBuilderCallbacks cb;

    bool cameraControlEnabled;
    bool showUI;

    entt::entity mainCameraEntity = entt::null;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app && app->GetWindow())
            app->GetWindow()->SetSize(width, height);
    }

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos)
    {   
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        if (!engine) return;

        engine->GetInput()->UpdateMousePosition(xpos, ypos);

        if (ImGuizmo::IsUsing() || ImGui::GetIO().WantCaptureMouse)
            return;

        if (engine->cameraControlEnabled && engine->mainCameraEntity != entt::null) 
        {
            glm::vec2 delta = engine->GetInput()->GetMouseDelta();
            auto& orientation = engine->ecsModule->GetECS()->GetComponent<CameraOrientationComponent>(engine->mainCameraEntity);
            auto& config = engine->ecsModule->GetECS()->GetComponent<CameraComponent>(engine->mainCameraEntity);

            orientation.yaw   += delta.x * config.mouseSensitivity;
            orientation.pitch += delta.y * config.mouseSensitivity;

            if (orientation.pitch > 89.0f)  orientation.pitch = 89.0f;
            if (orientation.pitch < -89.0f) orientation.pitch = -89.0f;
            return;
        }

        if (ImGuizmo::IsUsing() || ImGui::GetIO().WantCaptureMouse)
            return;
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
        if (!engine) return;

        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) 
            if (engine->ShouldAllowCameraControl())
                engine->SetCameraControlMode(!engine->cameraControlEnabled);
    }

    void SetCameraControlMode(bool enabled)
    {
        cameraControlEnabled = enabled;
        
        if (enabled) 
        {
            GetWindow()->SetCursorMode(GLFW_CURSOR_DISABLED);
            Logger::Log(LogLevel::INFO, "Camera control: ON");
        } 
        else 
        {
            GetWindow()->SetCursorMode(GLFW_CURSOR_NORMAL);
            Logger::Log(LogLevel::INFO, "Camera control: OFF (UI mode)");
        }
    }

protected:
    /**
     * @brief Initialize game engine
     * Initialize Modules and other stuff that wasn't initialized
     * in Application class
     */
    void OnInitialize() override
    {
        Logger::Log(LogLevel::INFO, "Initializing WFE...");
        Logger::Log(LogLevel::INFO, "==================================");

        GetWindow()->SetFramebufferSizeCallback(FramebufferSizeCallback);
        GetWindow()->SetCursorPosCallback(MouseCallback);
        GetWindow()->SetScrollCallback(Input::ScrollCallback);
        GetWindow()->SetMouseButtonCallback(MouseButtonCallback);

        mm = GetModuleManager();

        mm->RegisterModule<ECSModule>();
        ecsModule = mm->GetModule<ECSModule>("ECS");
        ecsModule->Initialize();
        if (!ecsModule->IsInitialized())
        {
            Logger::Log(LogLevel::WARNING, 
                "ECS failed to initialize");
        }

        mm->RegisterModule<ResourceModule>();
        resourceModule = mm->GetModule<ResourceModule>("Resource");
        resourceModule->Initialize();
        if (!resourceModule->IsInitialized())
        {
            Logger::Log(LogLevel::WARNING, 
                "RedsourceModule failed to initialize");
        }

        mm->RegisterModule<RenderingModule>(
            GetWindow()->GetGLFWWindow(), 
            ecsModule->GetECS(), 
            mm);
        renderingModule = mm->GetModule<RenderingModule>("Rendering");
        renderingModule->Initialize();
        if (!renderingModule->IsInitialized())
        {
            Logger::Log(LogLevel::WARNING, 
                "RenderingModule failed to initialize");
        }

        mm->RegisterModule<SceneModule>(ecsModule->GetECS());
        sceneModule = mm->GetModule<SceneModule>("Scene");
        sceneModule->Initialize();
        if (!sceneModule->IsInitialized())
        {
            Logger::Log(LogLevel::WARNING, 
                "SceneModule failed to initialize");
        }

        mm->RegisterModule<UIModule>(
            ecsModule->GetECS(), 
            &mainCameraEntity, 
            sceneModule->GetSceneManager(), 
            mm,
            GetWindow()->GetGLFWWindow()
        );
        uiModule = mm->GetModule<UIModule>("UI");
        uiModule->Initialize();
        if (!uiModule->IsInitialized())
        {
            Logger::Log(LogLevel::WARNING, 
                "UIModule failed to initialize");
        }

        scriptSystem = std::make_unique<ScriptSystem>();
        inputControllerSystem = std::make_unique<InputControllerSystem>();

        InitializeAS();

        ech = std::make_unique<EditorCommandHandler>(mm);
        ech->RegisterAllCommands();
        RegistraterCoreCommands();

        auto editorCam = ecsModule->GetECS()->CreateCamera("Main Camera", true, true);
        SetMainCameraEntity(editorCam);
        SetCameraControlMode(false);

        CommandManager::ExecuteCommand("onDebugPauseToggle", {});

        renderingModule->GetRenderer()->SetEnableShadows(true);
        
        Logger::Log(LogLevel::INFO, "==================================");
        Logger::Log(LogLevel::INFO, "Engine initialized successfully");
    }

    void OnUpdate(float deltaTime) override
    {
        mm->UpdateAll(deltaTime);

        ProcessInput();

        bool allowCameraControl = cameraControlEnabled && ShouldAllowCameraControl();
        inputControllerSystem->Update(
            *ecsModule->GetECS(), 
            *GetInput(), 
            deltaTime, 
            allowCameraControl
        );

        UpdateMainCamera();

        scriptSystem->Update(*ecsModule->GetECS(), GetInput(), deltaTime);
    }

    void UpdateMainCamera()
    {
        auto* ecs = ecsModule->GetECS();
        entt::entity current = mainCameraEntity;

        if (current == entt::null || !ecs->IsValid(current) || !ecs->HasComponent<CameraComponent>(current))
        {
            auto view = ecs->GetRegistry().view<CameraComponent>();
            if (!view.empty())
            {
                entt::entity found = entt::null;
                for (auto entity : view)
                {
                    if (view.get<CameraComponent>(entity).isMainCamera)
                    {
                        found = entity;
                        break;
                    }
                }
                
                if (found == entt::null) found = view.front();
                
                SetMainCameraEntity(found); 
            }
            else
            {
                SetMainCameraEntity(entt::null);
            }
        }
    }

    /**
     * @brief Render scene into framebuffer
     */
    void OnRender() override
    {
        auto* ecs      = ecsModule->GetECS();
        auto* renderer = renderingModule->GetRenderer();

        entt::entity camera = ecs->FindGameCamera();
        if (camera == entt::null)
        {
            Logger::Log(LogLevel::WARNING, "No game camera found!");
            return;
        }

        renderer->BeginFrame();
        renderer->Render(
            *ecs,
            camera,
            GetWindow()->GetWidth(),
            GetWindow()->GetHeight()
        );
        renderer->EndFrame();

        if (showUI)
        {
            renderingModule->GetRenderer()->GetIcon()->Update(
                *ecs,
                *resourceModule->GetShaderManager(),
                "icon",
                ecs->GetComponent<CameraOrientationComponent>(camera).GetViewMatrix(
                    ecs->GetComponent<TransformComponent>(camera).position),
                ecs->GetComponent<CameraComponent>(camera).GetProjectionMatrix(
                    (float)GetWindow()->GetWidth()/(float)GetWindow()->GetHeight())
            );

            uiModule->GetImGuiManager()->BeginFrame();
            m_overlay.Render(ecs, mainCameraEntity, cb, resourceModule->GetMaterialManager());
            uiModule->GetImGuiManager()->EndFrame();
        }
    }

    /**
     * @brief Shutdown classes needed to shutdown by hand
     */
    void OnShutdown() override
    {
        Logger::Log(LogLevel::INFO, "Shutting down engine...");
        Logger::Log(LogLevel::INFO, "==================================");

        mm->ShutdownAll();
        
        Logger::Log(LogLevel::INFO, "==================================");
        Logger::Log(LogLevel::INFO, "Engine shutdown complete!");
    }

    bool ShouldAllowCameraControl() const override 
    { 
        if (sceneModule && sceneModule->GetSceneManager())
            return !sceneModule->GetSceneManager()->IsInPlayMode();

        return true;
    }

    void SetMainCameraEntity(entt::entity newMainCameraEntity) 
    {
        mainCameraEntity = newMainCameraEntity;
    }

public:
    Engine(int w, int h, const std::string& title)
        : Application(w, h, title)
    {}

private:
    void InitializeAS()
    {
        Logger::Log(LogLevel::INFO, "Initializing AngelScript...");
        
        try 
        {
            InitAS(ecsModule->GetECS(), GetInput());
            Logger::Log(LogLevel::INFO, "AngelScript initialized successfully");
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, "Failed to initialize AngelScript: " + std::string(e.what()));
        }
    }

    void ProcessInput()
    {
        if (GetInput()->IsKeyJustPressed(Key::KEY_F5))
        {
            resourceModule->GetShaderManager()->ReloadAll();
            Logger::Log(LogLevel::INFO, "Reloaded all shaders");
        }

        if (GetInput()->IsKeyJustPressed(Key::KEY_F1))
            showUI = !showUI;
        
    }

    void RegistraterCoreCommands()
    {
        CommandManager::RegisterCommand("onExit",
        [this](const CommandArgs&) 
        {
            Logger::Log(LogLevel::INFO, "Exit requested from menu");
            Stop();
        });
    }
};
