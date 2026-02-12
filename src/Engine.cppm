module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
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
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.Scene.SceneSurializer;
import WFE.Scene.Light;
import WFE.Scripting.LuaBindings;
import WFE.Scripting.LuaState;
import WFE.ECS.Systems;
import WFE.ECS.Components;

import WFE.Core.ModuleManager;
import WFE.Rendering.RenderingModule;
import WFE.Resource.ResourceModule;
import WFE.UI.UIModule;
import WFE.ECS.ECSModule;

/// @file Engine.cppm
/// @brief Engine class
/// @author SuperChabs

/**
 * 
 */
export class Engine : public Application 
{
private:  
    std::unique_ptr<RotationSystem> rotationSystem;
    std::unique_ptr<ScriptSystem> scriptSystem;
    std::unique_ptr<InputControllerSystem> inputControllerSystem;

    std::unique_ptr<SceneSerializer> sceneSerializer;

    ModuleManager* mm;
    RenderingModule* renderingModule;
    ResourceModule* resourceModule;
    UIModule* uiModule;
    ECSModule* ecsModule;

    bool isPlayMode = false;
    bool cameraControlEnabled;
    bool showUI;

    entt::entity mainCameraEntity = entt::null;

    glm::vec3 savedEditorCameraPos;
    float savedEditorCameraYaw;
    float savedEditorCameraPitch;

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
                "RedsourceModule failed to initialize");
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

        mm->RegisterModule<UIModule>(
            ecsModule->GetECS(), 
            &mainCameraEntity, 
            &isPlayMode, 
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

        InitializeLua();

        sceneSerializer = std::make_unique<SceneSerializer>(
            ecsModule->GetECS()
        );

        InitCommandRegistration();

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

        if (isPlayMode)
            rotationSystem->Update(*ecsModule->GetECS(), deltaTime);

        scriptSystem->Update(*ecsModule->GetECS(), deltaTime);
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

        // Scene Framebuffer
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

            if (gameCamera != entt::null)
                renderer->Render(
                    *ecsModule->GetECS(), 
                    gameCamera,             
                    static_cast<int>(sceneViewportSize.x), 
                    static_cast<int>(sceneViewportSize.y)
                );
            
            renderer->EndFrame();
            sceneFB->Unbind();
        }

        // Game Framebuffer
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
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, GetWindow()->GetWidth(), GetWindow()->GetHeight());
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
        if (showUI)
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
        return !isPlayMode; 
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
    void InitializeLua()
    {
        Logger::Log(LogLevel::INFO, "Initializing Lua scripting...");
        
        try 
        {
            InitLua(ecsModule->GetECS());
            Logger::Log(LogLevel::INFO, "Lua scripting initialized successfully");
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, "Failed to initialize Lua: " + std::string(e.what()));
        }
    }

    void ProcessInput()
    {
        if (GetInput()->IsKeyPressed(Key::KEY_ESCAPE)) 
            Stop();
        
        if (GetInput()->IsKeyJustPressed(Key::KEY_F1)) 
            showUI = !showUI;

        if (GetInput()->IsKeyJustPressed(Key::KEY_F5))
        {
            resourceModule->GetShaderManager()->ReloadAll();
            Logger::Log(LogLevel::INFO, "Reloaded all shaders");
        }

        // Ctrl+S - Quick Save
        if (GetInput()->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            GetInput()->IsKeyJustPressed(Key::KEY_S))
        {
            if (CommandManager::HasCommand("onQuickSave"))
                CommandManager::ExecuteCommand("onQuickSave", {});
        }
        
        // Ctrl+L - Quick Load
        if (GetInput()->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            GetInput()->IsKeyJustPressed(Key::KEY_L))
        {
            if (CommandManager::HasCommand("onQuickLoad"))
                CommandManager::ExecuteCommand("onQuickLoad", {});
        }
        
        // Ctrl+N - New Scene
        if (GetInput()->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            GetInput()->IsKeyJustPressed(Key::KEY_N))
        {
            if (CommandManager::HasCommand("onNewScene"))
                CommandManager::ExecuteCommand("onNewScene", {});
        }
    }

    void InitCommandRegistration()
    {
        CommandManager::RegisterCommand("onCreateCube",
        [this](const CommandArgs&) 
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Cube");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));
            
            auto cubeMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
            ecsModule->GetECS()->AddComponent<MeshComponent>(entity, cubeMesh);
            
            auto material = resourceModule->GetMaterialManager()->GetMaterial("gray");
            ecsModule->GetECS()->AddComponent<MaterialComponent>(entity, material);
            
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<RotationComponent>(entity, 50.0f);
            
            Logger::Log(LogLevel::INFO, "Cube entity created");
        });

        CommandManager::RegisterCommand("onCreatePlane",
        [this](const CommandArgs&) 
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Plane");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));
            
            auto mesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::QUAD);
            ecsModule->GetECS()->AddComponent<MeshComponent>(entity, mesh);
            
            auto material = resourceModule->GetMaterialManager()->GetMaterial("gray");
            ecsModule->GetECS()->AddComponent<MaterialComponent>(entity, material);
            
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<RotationComponent>(entity, 50.0f);
            
            Logger::Log(LogLevel::INFO, "Cube entity created");
        });

        CommandManager::RegisterCommand("onExit",
        [this](const CommandArgs&) 
        {
            Logger::Log(LogLevel::INFO, "Exit requested from menu");
            Stop();
        });

        CommandManager::RegisterCommand("onChangeMeshColor", 
        [this](const CommandArgs& args)
        {
            if (args.size() != 1)
            {
                Logger::Log(LogLevel::ERROR, "onChangeMeshColor requires 1 argument: color");
                return;
            }

            const auto& color = std::get<glm::vec3>(args[0]);
            entt::entity selected = uiModule->GetEditorLayout()->GetSelectedEntity();
            
            if (selected == entt::null || !ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            if (ecsModule->GetECS()->HasComponent<ColorComponent>(selected))
            {
                auto& colorComp = ecsModule->GetECS()->GetComponent<ColorComponent>(selected);
                colorComp.color = color;
            }
            else
            {
                ecsModule->GetECS()->AddComponent<ColorComponent>(selected, color);
            }

            if (ecsModule->GetECS()->HasComponent<MeshComponent>(selected))
            {
                auto& meshComp = ecsModule->GetECS()->GetComponent<MeshComponent>(selected);
                if (meshComp.mesh)
                    meshComp.mesh->SetColor(color);
            }
        });

        CommandManager::RegisterCommand("onCreateDirectionalLight",
        [this](const CommandArgs&) 
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Directional Light");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 10, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::DIRECTIONAL);
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_directional.png", 0.3f);
            Logger::Log(LogLevel::INFO, "Directional light created with icon");
        });

        CommandManager::RegisterCommand("onCreatePointLight",
        [this](const CommandArgs&) 
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Point Light");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 5, 0), glm::vec3(0), glm::vec3(1));
            ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::POINT);
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_point.png", 0.4f);
            Logger::Log(LogLevel::INFO, "Point light created with icon");
        });

        CommandManager::RegisterCommand("onCreateSpotLight",
        [this](const CommandArgs&) 
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Spot Light");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 5, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::SPOT);
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_spot.png", 0.35f);
            Logger::Log(LogLevel::INFO, "Spot light created with icon");
        });

        CommandManager::RegisterCommand("onSaveScene",
        [this](const CommandArgs& args) 
        {
            std::string filename = "scene.json";
            
            if (!args.empty())
            {
                try 
                {
                    filename = std::get<std::string>(args[0]);
                }
                catch (...) 
                {
                    Logger::Log(LogLevel::WARNING, "Invalid filename argument, using default");
                }
            }
            
            if (filename.find(".json") == std::string::npos)
                filename += ".json";
            
            bool success = sceneSerializer->SaveScene(filename);
            
            if (success)
                Logger::Log(LogLevel::INFO, "✓ Scene saved successfully: " + filename);
            else
                Logger::Log(LogLevel::ERROR, "✗ Failed to save scene: " + filename);
        });
        
        CommandManager::RegisterCommand("onLoadScene",
        [this](const CommandArgs& args) 
        {
            std::string filename = "scene.json";
            
            if (!args.empty())
            {
                try 
                {
                    filename = std::get<std::string>(args[0]);
                }
                catch (...) 
                {
                    Logger::Log(LogLevel::WARNING, "Invalid filename argument, using default");
                }
            }
            
            if (filename.find(".json") == std::string::npos)
                filename += ".json";
            
            bool success = sceneSerializer->LoadScene(
                filename,
                resourceModule->GetMaterialManager(),
                resourceModule->GetTextureManager()
            );
            
            if (success)
                Logger::Log(LogLevel::INFO, "✓ Scene loaded successfully: " + filename);
            else
                Logger::Log(LogLevel::ERROR, "✗ Failed to load scene: " + filename);
        });
        
        CommandManager::RegisterCommand("onQuickSave",
        [this](const CommandArgs&) 
        {
            CommandManager::ExecuteCommand("onSaveScene", {std::string("quicksave.json")});
        });
        
        CommandManager::RegisterCommand("onQuickLoad",
        [this](const CommandArgs&) 
        {
            CommandManager::ExecuteCommand("onLoadScene", {std::string("quicksave.json")});
        });
        
        // CommandManager::RegisterCommand("onNewScene",
        // [this](const CommandArgs&) 
        // {
        //     ecsModule->GetECS()->Clear();
        //    
        //     GetCamera()->SetPosition(glm::vec3(0, 0, 0));
        //     GetCamera()->SetYaw(-95.0f);
        //     GetCamera()->SetPitch(0.0f);
        //  
        //     Logger::Log(LogLevel::INFO, "New scene created");
        // });
        
        CommandManager::RegisterCommand("onListScenes",
        [this](const CommandArgs&) 
        {
            auto scenes = sceneSerializer->GetAvailableScenes();
            
            if (scenes.empty())
            {
                Logger::Log(LogLevel::INFO, "No saved scenes found");
            }
            else
            {
                Logger::Log(LogLevel::INFO, "Available scenes (" + 
                    std::to_string(scenes.size()) + "):");
                
                for (const auto& scene : scenes)
                    Logger::Log(LogLevel::INFO, "  - " + scene);
            }
        });

        CommandManager::RegisterCommand("onLoadModel",
        [this](const CommandArgs& args) 
        {
            Logger::Log(LogLevel::INFO, "=== onLoadModel command called ===");
            
            if (args.empty())
            {
                Logger::Log(LogLevel::ERROR, "onLoadModel requires filepath argument");
                return;
            }
            
            std::string filepath = std::get<std::string>(args[0]);
            Logger::Log(LogLevel::INFO, "Filepath: " + filepath);
            
            if (!resourceModule->GetModelManager())
            {
                Logger::Log(LogLevel::ERROR, "ModelManager is NULL!");
                return;
            }
            
            if (!ecsModule->GetECS())
            {
                Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
                return;
            }
            
            Logger::Log(LogLevel::INFO, "Calling LoadWithECS...");
            auto model = resourceModule->GetModelManager()->LoadWithECS(filepath, ecsModule->GetECS());
            
            if (!model) 
            {
                Logger::Log(LogLevel::ERROR, "Failed to load model: " + filepath);
                return;
            }
            
            Logger::Log(LogLevel::INFO, 
                "Model loaded successfully with " + std::to_string(model->GetMeshCount()) + " meshes");
            
            Logger::Log(LogLevel::INFO, "Total entities in world: " + std::to_string(ecsModule->GetECS()->GetEntityCount()));
            Logger::Log(LogLevel::INFO, "=== onLoadModel command complete ===\n");
        });

        CommandManager::RegisterCommand("onCreateCamera", [this](const CommandArgs&) 
        {
            auto* ecs = ecsModule->GetECS();
            auto newCam = ecs->CreateCamera("Camera", false);
            
            auto view = ecs->GetRegistry().view<CameraComponent>();
            if (view.size() == 1)
            {
                ecs->GetComponent<CameraComponent>(newCam).isMainCamera = true;
                SetMainCameraEntity(newCam);
            }
            
            Logger::Log(LogLevel::INFO, "New camera created and assigned as main if needed");
        });

        CommandManager::RegisterCommand("onPlayGame",
        [this](const CommandArgs&) 
        {
            if (isPlayMode)
            {
                Logger::Log(LogLevel::WARNING, "Already in play mode");
                return;
            }
            
            Logger::Log(LogLevel::INFO, "=== ENTERING PLAY MODE ===");
            
            auto* ecs = ecsModule->GetECS();
            entt::entity editorCam = ecs->FindEditorCamera();
            
            if (editorCam != entt::null && ecs->IsValid(editorCam))
            {
                auto& transform = ecs->GetComponent<TransformComponent>(editorCam);
                auto& orientation = ecs->GetComponent<CameraOrientationComponent>(editorCam);
                
                savedEditorCameraPos = transform.position;
                savedEditorCameraYaw = orientation.yaw;
                savedEditorCameraPitch = orientation.pitch;
                
                Logger::Log(LogLevel::INFO, "Editor camera state saved");
            }
            
            entt::entity gameCam = ecs->FindGameCamera();
            if (gameCam != entt::null)
            {
                SetMainCameraEntity(gameCam);
                auto& gameCamComp = ecs->GetComponent<CameraComponent>(gameCam);
                gameCamComp.isMainCamera = true;
                
                if (editorCam != entt::null)
                {
                    auto& editorCamComp = ecs->GetComponent<CameraComponent>(editorCam);
                    editorCamComp.isMainCamera = false;
                }
                
                Logger::Log(LogLevel::INFO, "Switched to Game Camera");
            }
            
            isPlayMode = true;
            Logger::Log(LogLevel::INFO, "Play mode started");
        });

        CommandManager::RegisterCommand("onStopGame",
        [this](const CommandArgs&) 
        {
            if (!isPlayMode)
            {
                Logger::Log(LogLevel::WARNING, "Not in play mode");
                return;
            }
            
            Logger::Log(LogLevel::INFO, "=== EXITING PLAY MODE ===");
            
            auto* ecs = ecsModule->GetECS();
            entt::entity editorCam = ecs->FindEditorCamera();
            
            if (editorCam != entt::null && ecs->IsValid(editorCam))
            {
                auto& transform = ecs->GetComponent<TransformComponent>(editorCam);
                auto& orientation = ecs->GetComponent<CameraOrientationComponent>(editorCam);
                
                transform.position = savedEditorCameraPos;
                orientation.yaw = savedEditorCameraYaw;
                orientation.pitch = savedEditorCameraPitch;
                
                auto& editorCamComp = ecs->GetComponent<CameraComponent>(editorCam);
                editorCamComp.isMainCamera = true;
                
                SetMainCameraEntity(editorCam);
                
                entt::entity gameCam = ecs->FindGameCamera();
                if (gameCam != entt::null)
                {
                    auto& gameCamComp = ecs->GetComponent<CameraComponent>(gameCam);
                    gameCamComp.isMainCamera = false;
                }
                
                Logger::Log(LogLevel::INFO, "Editor camera state restored");
            }
            
            isPlayMode = false;
            Logger::Log(LogLevel::INFO, "Play mode stopped");
        });

        CommandManager::RegisterCommand("onAttachScript",
        [this](const CommandArgs& args)
        {
            if (args.empty())
            {
                Logger::Log(LogLevel::ERROR,
                    "onAttachScript requires script path");
                return;
            }

            entt::entity selected = uiModule->GetEditorLayout()->GetSelectedEntity();
            if (selected == entt::null || !ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            std::string scriptPath = std::get<std::string>(args[0]);

            auto* ecs = ecsModule->GetECS();

            if (ecs->HasComponent<ScriptComponent>(selected))
            {
                auto& script = ecs->GetComponent<ScriptComponent>(selected);

                script.scriptPath = scriptPath;
                script.loaded = false;
                script.failed = false;
                script.env = {};

                Logger::Log(LogLevel::INFO,
                    "Script updated: " + scriptPath);
            }
            else
            {
                auto& script = ecs->AddComponent<ScriptComponent>(selected);

                script.scriptPath = scriptPath;
                script.loaded = false;
                script.failed = false;

                Logger::Log(LogLevel::INFO,
                    "Script attached: " + scriptPath);
            }
        });

        CommandManager::RegisterCommand("onRemoveScript",
        [this](const CommandArgs&) 
        {
            entt::entity selected = uiModule->GetEditorLayout()->GetSelectedEntity();
            if (selected == entt::null || !ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            if (ecsModule->GetECS()->HasComponent<ScriptComponent>(selected))
            {
                auto& script = ecsModule->GetECS()->GetComponent<ScriptComponent>(selected);
                
                if (script.loaded && script.env["onDestroy"].valid())
                {
                    try 
                    {
                        script.env["onDestroy"]();
                    } 
                    catch (...) 
                    {
                        Logger::Log(LogLevel::WARNING, "Error in script onDestroy");
                    }
                }
                
                ecsModule->GetECS()->RemoveComponent<ScriptComponent>(selected);
                Logger::Log(LogLevel::INFO, "Script removed from entity");
            }
            else
            {
                Logger::Log(LogLevel::WARNING, "Entity has no script component");
            }
        });
    }
};