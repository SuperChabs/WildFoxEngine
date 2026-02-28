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

        // If ImGui/ImGuizmo is using the mouse, do not drive camera orientation
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
        }
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
    [[deprecated("hallo")]]
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

        auto editorCam = ecsModule->GetECS()->CreateCamera("Editor Camera", true, false);
        SetMainCameraEntity(editorCam);
        SetCameraControlMode(false);
        
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
        uiModule->GetImGuiManager()->BeginFrame();

        entt::entity editorCamera = ecsModule->GetECS()->FindEditorCamera();
        entt::entity gameCamera = ecsModule->GetECS()->FindGameCamera();

        auto* renderer = renderingModule->GetRenderer();

        if (gameCamera == entt::null)
        {
            gameCamera = ecsModule->GetECS()->CreateCamera("Game Camera", false, true);
            auto& transform = ecsModule->GetECS()->GetComponent<TransformComponent>(gameCamera);
            transform.position = glm::vec3(0, 5, 10);
        }

        // Scene Framebuffer (use editor camera)
        Framebuffer* sceneFB = uiModule->GetEditorLayout()->GetSceneFramebuffer();
        ImVec2 sceneViewportSize = uiModule->GetEditorLayout()->GetSceneViewportSize();
        if (sceneViewportSize.x <= 0 || sceneViewportSize.y <= 0)
        {
            Logger::Log(LogLevel::WARNING, "Invalid scene viewport size, skipping render");
        }
        else
        {
            sceneFB->Bind();
            renderer->BeginFrame();

            if (editorCamera != entt::null)
                renderer->Render(
                    *ecsModule->GetECS(), 
                    editorCamera,             
                    static_cast<int>(sceneViewportSize.x), 
                    static_cast<int>(sceneViewportSize.y)
                );
            
            renderer->EndFrame();
            sceneFB->Unbind();
        }

        // Game Framebuffer (use game camera)
        Framebuffer* gameFB = uiModule->GetEditorLayout()->GetGameFramebuffer();
        ImVec2 gameViewportSize = uiModule->GetEditorLayout()->GetGameViewportSize();

        if (gameViewportSize.x <= 0 || gameViewportSize.y <= 0)
        {
            Logger::Log(LogLevel::WARNING, "Invalid game viewport size, skipping render");
        }
        else
        {
            gameFB->Bind();
            renderer->BeginFrame();

            if (gameCamera != entt::null)
                renderer->Render(
                    *ecsModule->GetECS(), 
                    gameCamera,             
                    static_cast<int>(gameViewportSize.x), 
                    static_cast<int>(gameViewportSize.y)
                );
            
            renderer->EndFrame();
            gameFB->Unbind();
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        uiModule->RenderUI();
        uiModule->GetImGuiManager()->EndFrame();
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

        if (GetInput()->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            GetInput()->IsKeyJustPressed(Key::KEY_S))
        {
            if (CommandManager::HasCommand("onQuickSave"))
                CommandManager::ExecuteCommand("onQuickSave", {});
        }
        
        if (GetInput()->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            GetInput()->IsKeyJustPressed(Key::KEY_L))
        {
            if (CommandManager::HasCommand("onQuickLoad"))
                CommandManager::ExecuteCommand("onQuickLoad", {});
        }
        
        if (GetInput()->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            GetInput()->IsKeyJustPressed(Key::KEY_N))
        {
            if (CommandManager::HasCommand("onNewScene"))
                CommandManager::ExecuteCommand("onNewScene", {});
        }
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
