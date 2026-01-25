module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <entt.hpp>
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
import WFE.Scene.Model;
import WFE.Scene.SceneSurializer;
import WFE.UI.EditorLayout;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.ECS.Systems;
import WFE.Rendering.Renderer;

export class Engine : public Application 
{
private:
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<EditorLayout> editorLayout;
    
    std::unique_ptr<RotationSystem> rotationSystem;

    std::unique_ptr<SceneSerializer> sceneSerializer;

protected:
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

        sceneSerializer = std::make_unique<SceneSerializer>(
            GetECSWorld(), 
            GetCamera()
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
        
        Logger::Log(LogLevel::INFO, "Engine initialized successfully");
    }

    void OnUpdate(float deltaTime) override
    {
        rotationSystem->Update(*GetECSWorld(), deltaTime);
    }

void OnRender() override
{
    if (editorLayout && editorLayout->GetFramebuffer())
    {
        Framebuffer* fb = editorLayout->GetFramebuffer();
        ImVec2 viewportSize = editorLayout->GetViewportSize();
        
        fb->Bind();
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        
        GetRenderer()->BeginFrame();
        
        if (viewportSize.x > 0 && viewportSize.y > 0)
        {
            GetRenderer()->Render(
                *GetCamera(), 
                static_cast<int>(viewportSize.x), 
                static_cast<int>(viewportSize.y)
            );
        }
        
        GetRenderer()->EndFrame();
        
        fb->Unbind();
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, GetWindow()->GetWidth(), GetWindow()->GetHeight());
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); 
}
    void OnShutdown() override
    {
        Logger::Log(LogLevel::INFO, "Shutting down engine...");
        
        GetRenderer()->Shutdown();
        GetShaderManager()->ClearAll();
        
        Logger::Log(LogLevel::INFO, "Engine shutdown complete!");
    }

    void RenderUI() override
    {    
        if (!editorLayout)
        {
            Logger::Log(LogLevel::ERROR, "EditorLayout is null!");
            return;
        }

        editorLayout->RenderEditor(
            GetECSWorld(), 
            GetCamera(), 
            GetRenderer(), 
            GetShaderManager(), 
            GetMaterialManager()
        );
    }

public:
    Engine(int w, int h, const std::string& title)
        : Application(w, h, title)
    {}

private:
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
        
        CommandManager::RegisterCommand("onNewScene",
        [this](const CommandArgs&) 
        {
            GetECSWorld()->Clear();
            
            GetCamera()->SetPosition(glm::vec3(0, 0, 0));
            GetCamera()->SetYaw(-95.0f);
            GetCamera()->SetPitch(0.0f);
            
            Logger::Log(LogLevel::INFO, "New scene created");
        });
        
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
    }
};