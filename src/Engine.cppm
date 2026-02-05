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
import WFE.Scene.Skybox;
import WFE.Rendering.Core.Framebuffer;
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.Rendering.Light;
import WFE.Scene.Mesh;
import WFE.Scene.SceneSurializer;
import WFE.UI.EditorLayout;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.ECS.Systems;
import WFE.Rendering.Renderer;
import WFE.Scripting.LuaBindings;
import WFE.Scripting.LuaState;

/// @file Engine.cppm
/// @brief Engine class
/// @author SuperChabs
/// @date 2026-02-05

/**
 * 
 */
export class Engine : public Application 
{
private:
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<EditorLayout> editorLayout;
    
    std::unique_ptr<RotationSystem> rotationSystem;
    std::unique_ptr<ScriptSystem> scriptSystem;

    std::unique_ptr<SceneSerializer> sceneSerializer;

    bool isPlayMode = false;

    glm::vec3 savedEditorCameraPos;
    float savedEditorCameraYaw;
    float savedEditorCameraPitch;

protected:
    /**
     * @brief Initialize game engine
     * Initialize Skybox, Renderer and other stuff that wasn't initialized
     * in Application class
     */
    void OnInitialize() override
    {
        Logger::Log(LogLevel::INFO, "Initializing WFE...");

        GetShaderManager()->Load();
        GetMaterialManager()->LoadMaterialConfigs();

        std::vector<std::string> faces = 
        {
            "assets/textures/skybox1/right.png",
            "assets/textures/skybox1/left.png",
            "assets/textures/skybox1/top.png",
            "assets/textures/skybox1/bottom.png",
            "assets/textures/skybox1/front.png",
            "assets/textures/skybox1/back.png"
        };
        
        unsigned int cubemapTexture = GetTextureManager()->LoadCubemap(faces);
        skybox = std::make_unique<Skybox>(cubemapTexture, "skybox");

        auto renderer = std::make_unique<Renderer>(GetShaderManager(), GetECSWorld());

        SetRenderer(std::move(renderer));

        auto iconSystem = std::make_unique<IconRenderSystem>(GetTextureManager());
        GetRenderer()->SetIconRenderSystem(std::move(iconSystem));

        if (!GetRenderer()->Initialize(skybox->GetVAO(), skybox->GetTexture()))
        {
            Logger::Log(LogLevel::ERROR, "Failed to initialize Renderer!");
            return;
        }

        rotationSystem = std::make_unique<RotationSystem>();
        scriptSystem = std::make_unique<ScriptSystem>();

        InitializeLua();

        sceneSerializer = std::make_unique<SceneSerializer>(
            GetECSWorld(), 
            nullptr
        );

        InitCommandRegistration();

        Logger::Log(LogLevel::INFO, "Creating EditorLayout...");
        editorLayout = std::make_unique<EditorLayout>();
        
        if (!editorLayout)
            Logger::Log(LogLevel::ERROR, "Failed to create EditorLayout!");
        else
            Logger::Log(LogLevel::INFO, "EditorLayout created successfully!");

        GetShaderManager()->Bind("skybox");
        GetShaderManager()->SetInt("skybox", "skybox", 0);

        auto editorCam = GetECSWorld()->CreateCamera("Editor Camera", true, false);
        SetMainCameraEntity(editorCam);
        
        Logger::Log(LogLevel::INFO, "Engine initialized successfully");
    }

    void OnUpdate(float deltaTime) override
    {
        UpdateMainCamera();

        if (isPlayMode)
            rotationSystem->Update(*GetECSWorld(), deltaTime);

        scriptSystem->Update(*GetECSWorld(), deltaTime);
    }

    void UpdateMainCamera()
    {
        auto* ecs = GetECSWorld();
        entt::entity current = GetMainCameraEntity();

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
        if (!editorLayout)
            return;
        
        entt::entity editorCamera = GetECSWorld()->FindEditorCamera();
        entt::entity gameCamera = GetECSWorld()->FindGameCamera();

        if (gameCamera == entt::null)
        {
            gameCamera = GetECSWorld()->CreateCamera("Game Camera", false, true);
            auto& transform = GetECSWorld()->GetComponent<TransformComponent>(gameCamera);
            transform.position = glm::vec3(0, 5, 10);
        }

        // Scene Framebuffer
        Framebuffer* sceneFB = editorLayout->GetSceneFramebuffer();
        ImVec2 sceneViewportSize = editorLayout->GetSceneViewportSize();
        sceneFB->Bind();
        GetRenderer()->BeginFrame();

        GetRenderer()->Render(
            *GetECSWorld(),
            editorCamera, 
            static_cast<int>(sceneViewportSize.x), 
            static_cast<int>(sceneViewportSize.y)
        );

        GetRenderer()->EndFrame();
        sceneFB->Unbind();

        // Game Framebuffer
        Framebuffer* gameFB = editorLayout->GetGameFramebuffer();
        ImVec2 gameViewportSize = editorLayout->GetGameViewportSize();
        gameFB->Bind();
        GetRenderer()->BeginFrame();

        if (gameCamera != entt::null)
            GetRenderer()->Render(
                *GetECSWorld(), 
                gameCamera,             
                static_cast<int>(gameViewportSize.x), 
                static_cast<int>(gameViewportSize.y)
            );
        
        GetRenderer()->EndFrame();
        gameFB->Unbind();
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT); 
    }

    /**
     * @brief Shutdown classes needed to shutdown by hand
     */
    void OnShutdown() override
    {
        Logger::Log(LogLevel::INFO, "Shutting down engine...");
        
        GetRenderer()->Shutdown();
        GetShaderManager()->ClearAll();
        
        Logger::Log(LogLevel::INFO, "Engine shutdown complete!");
    }

    /**
     * @brief Render User Interface
     */
    void RenderUI() override
    {    
        if (!editorLayout)
        {
            Logger::Log(LogLevel::ERROR, "EditorLayout is null!");
            return;
        }

        editorLayout->RenderEditor(
            GetECSWorld(), 
            GetMainCameraEntity(),
            GetRenderer(), 
            GetShaderManager(), 
            GetMaterialManager(),
            isPlayMode
        );
    }

    bool ShouldAllowCameraControl() const override 
    { 
        return !isPlayMode; 
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
            InitLua(GetECSWorld());
            Logger::Log(LogLevel::INFO, "Lua scripting initialized successfully");
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, "Failed to initialize Lua: " + std::string(e.what()));
        }
    }

    void InitCommandRegistration()
    {
        CommandManager::RegisterCommand("onCreateCube",
        [this](const CommandArgs&) 
        {
            auto entity = GetECSWorld()->CreateEntity("Cube");
            GetECSWorld()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));
            
            auto cubeMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
            GetECSWorld()->AddComponent<MeshComponent>(entity, cubeMesh);
            
            auto material = GetMaterialManager()->GetMaterial("gray");
            GetECSWorld()->AddComponent<MaterialComponent>(entity, material);
            
            GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            GetECSWorld()->AddComponent<RotationComponent>(entity, 50.0f);
            
            Logger::Log(LogLevel::INFO, "Cube entity created");
        });

        CommandManager::RegisterCommand("onCreatePlane",
        [this](const CommandArgs&) 
        {
            auto entity = GetECSWorld()->CreateEntity("Plane");
            GetECSWorld()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));
            
            auto mesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::QUAD);
            GetECSWorld()->AddComponent<MeshComponent>(entity, mesh);
            
            auto material = GetMaterialManager()->GetMaterial("gray");
            GetECSWorld()->AddComponent<MaterialComponent>(entity, material);
            
            GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            GetECSWorld()->AddComponent<RotationComponent>(entity, 50.0f);
            
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
            entt::entity selected = editorLayout->GetSelectedEntity();
            
            if (selected == entt::null || !GetECSWorld()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            if (GetECSWorld()->HasComponent<ColorComponent>(selected))
            {
                auto& colorComp = GetECSWorld()->GetComponent<ColorComponent>(selected);
                colorComp.color = color;
            }
            else
            {
                GetECSWorld()->AddComponent<ColorComponent>(selected, color);
            }

            if (GetECSWorld()->HasComponent<MeshComponent>(selected))
            {
                auto& meshComp = GetECSWorld()->GetComponent<MeshComponent>(selected);
                if (meshComp.mesh)
                    meshComp.mesh->SetColor(color);
            }
        });

        CommandManager::RegisterCommand("onCreateDirectionalLight",
        [this](const CommandArgs&) 
        {
            auto entity = GetECSWorld()->CreateEntity("Directional Light");
            GetECSWorld()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 10, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            GetECSWorld()->AddComponent<LightComponent>(entity, LightType::DIRECTIONAL);
            GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            GetECSWorld()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_directional.png", 0.3f);
            Logger::Log(LogLevel::INFO, "Directional light created with icon");
        });

        CommandManager::RegisterCommand("onCreatePointLight",
        [this](const CommandArgs&) 
        {
            auto entity = GetECSWorld()->CreateEntity("Point Light");
            GetECSWorld()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 5, 0), glm::vec3(0), glm::vec3(1));
            GetECSWorld()->AddComponent<LightComponent>(entity, LightType::POINT);
            GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            GetECSWorld()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_point.png", 0.4f);
            Logger::Log(LogLevel::INFO, "Point light created with icon");
        });

        CommandManager::RegisterCommand("onCreateSpotLight",
        [this](const CommandArgs&) 
        {
            auto entity = GetECSWorld()->CreateEntity("Spot Light");
            GetECSWorld()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 5, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            GetECSWorld()->AddComponent<LightComponent>(entity, LightType::SPOT);
            GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            GetECSWorld()->AddComponent<IconComponent>(entity, 
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
                GetMaterialManager(),
                GetTextureManager()
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
        //     GetECSWorld()->Clear();
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
            
            if (!GetModelManager())
            {
                Logger::Log(LogLevel::ERROR, "ModelManager is NULL!");
                return;
            }
            
            if (!GetECSWorld())
            {
                Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
                return;
            }
            
            Logger::Log(LogLevel::INFO, "Calling LoadWithECS...");
            auto model = GetModelManager()->LoadWithECS(filepath, GetECSWorld());
            
            if (!model) 
            {
                Logger::Log(LogLevel::ERROR, "Failed to load model: " + filepath);
                return;
            }
            
            Logger::Log(LogLevel::INFO, 
                "Model loaded successfully with " + std::to_string(model->GetMeshCount()) + " meshes");
            
            Logger::Log(LogLevel::INFO, "Total entities in world: " + std::to_string(GetECSWorld()->GetEntityCount()));
            Logger::Log(LogLevel::INFO, "=== onLoadModel command complete ===\n");

            // for (size_t i = 0; i < model->GetMeshCount(); ++i) 
            // {
            //     auto entityName = model->GetName() + "_Mesh_" + std::to_string(i);
            //     auto entity = GetECSWorld()->CreateEntity(entityName);

            //     Logger::Log(LogLevel::INFO, "Creating entity: " + entityName);

            //     GetECSWorld()->AddComponent<TransformComponent>(entity, 
            //         glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));
            //     Logger::Log(LogLevel::DEBUG, "TransformComponent added");

            //     auto mesh = model->GetMesh(i);
            //     GetECSWorld()->AddComponent<MeshComponent>(entity, mesh);
            //     Logger::Log(LogLevel::DEBUG, "MeshComponent added with " + 
            //         std::to_string(mesh->GetMeshRenderer()->GetVertexCount()) + " vertices");

            //     auto material = mesh->GetMaterial();
            //     if (!material)
            //     {
            //         Logger::Log(LogLevel::WARNING, "Mesh " + std::to_string(i) + 
            //             " has no material, using default");
            //         material = GetMaterialManager()->GetMaterial("default");
            //     }
            //     else
            //     {
            //         Logger::Log(LogLevel::INFO, "Material assigned: " + material->GetName() +
            //             " (using color: " + (material->IsUsingColor() ? "YES" : "NO") + ")");
            //     }
            //     GetECSWorld()->AddComponent<MaterialComponent>(entity, material);

            //     GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            //     Logger::Log(LogLevel::DEBUG, "VisibilityComponent added");
            // }

            // Logger::Log(LogLevel::INFO, 
            //     "All model entities created: " + model->GetName() + 
            //     " (" + std::to_string(model->GetMeshCount()) + " meshes)");
        });

        CommandManager::RegisterCommand("onCreateCamera", [this](const CommandArgs&) 
        {
            auto* ecs = GetECSWorld();
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
            
            auto* ecs = GetECSWorld();
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
            
            auto* ecs = GetECSWorld();
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

            entt::entity selected = editorLayout->GetSelectedEntity();
            if (selected == entt::null || !GetECSWorld()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            std::string scriptPath = std::get<std::string>(args[0]);

            auto* ecs = GetECSWorld();

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
            entt::entity selected = editorLayout->GetSelectedEntity();
            if (selected == entt::null || !GetECSWorld()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            if (GetECSWorld()->HasComponent<ScriptComponent>(selected))
            {
                auto& script = GetECSWorld()->GetComponent<ScriptComponent>(selected);
                
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
                
                GetECSWorld()->RemoveComponent<ScriptComponent>(selected);
                Logger::Log(LogLevel::INFO, "Script removed from entity");
            }
            else
            {
                Logger::Log(LogLevel::WARNING, "Entity has no script component");
            }
        });
    }
};