module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

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

import WFE.Rendering.Skybox;
import WFE.Rendering.Core.Framebuffer;
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.Rendering.Light;

import WFE.Scene.Mesh;
import WFE.Scene.Model;

import WFE.UI.EditorLayout;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.ECS.Systems;

export class Engine : public Application 
{
private:
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<EditorLayout> editorLayout;

    std::unique_ptr<RenderSystem> renderSystem;
    std::unique_ptr<RotationSystem> rotationSystem;
    std::unique_ptr<LightSystem> lightSystem;
    std::unique_ptr<IconRenderSystem> iconRenderSystem;

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

        renderSystem = std::make_unique<RenderSystem>();
        rotationSystem = std::make_unique<RotationSystem>();
        lightSystem = std::make_unique<LightSystem>();
        iconRenderSystem = std::make_unique<IconRenderSystem>(GetTextureManager());

        InitCommandRegistration();

        Logger::Log(LogLevel::INFO, "Creating EditorLayout...");
        editorLayout = std::make_unique<EditorLayout>();
        
        if (!editorLayout)
            Logger::Log(LogLevel::ERROR, "Failed to create EditorLayout!");
        else
            Logger::Log(LogLevel::INFO, "EditorLayout created successfully!");

        GetShaderManager()->Bind("skybox");
        GetShaderManager()->SetInt("skybox", "skybox", 0);
        
        Logger::Log(LogLevel::INFO, "Game initialized successfully");
    }

    void OnUpdate(float deltaTime) override
    {
        rotationSystem->Update(*GetECSWorld(), deltaTime);
        lightSystem->Update(*GetECSWorld(), *GetShaderManager(), "basic");
    }

    void OnRender() override
    {
        if (editorLayout && editorLayout->GetFramebuffer())
        {
            Framebuffer* fb = editorLayout->GetFramebuffer();
            ImVec2 viewportSize = editorLayout->GetViewportSize();
            
            fb->Bind();
            
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            
            if (viewportSize.x > 0 && viewportSize.y > 0)
            {
                glm::mat4 projection = glm::perspective(
                    glm::radians(GetCamera()->GetZoom()), 
                    viewportSize.x / viewportSize.y, 
                    0.1f, 100.0f
                );
                glm::mat4 view = GetCamera()->GetViewMatrix();
                
                GetShaderManager()->Bind("basic");

                GLint currentProgram;
                glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
                if (currentProgram != GetShaderManager()->GetShader("basic")->ID) 
                    Logger::Log(LogLevel::ERROR, "Shader not active!");
                
                GetShaderManager()->SetMat4("basic", "projection", projection);
                GetShaderManager()->SetMat4("basic", "view", view);
                GetShaderManager()->SetVec3("basic", "viewPos", GetCamera()->GetPosition());
                
                static int frameCount = 0;
                if (frameCount % 60 == 0) 
                    Logger::Log(LogLevel::DEBUG, "Rendering " + 
                        std::to_string(GetECSWorld()->GetEntityCount()) + " entities");
                frameCount++; 
                
                renderSystem->Update(*GetECSWorld(), *GetShaderManager(), "basic", *GetCamera());
                skybox->Render(*GetShaderManager(), *GetCamera(), projection);

                glDisable(GL_DEPTH_TEST);
                GetShaderManager()->Bind("icon");
                GetShaderManager()->SetMat4("icon", "projection", projection);
                GetShaderManager()->SetMat4("icon", "view", view);
                iconRenderSystem->Update(*GetECSWorld(), *GetShaderManager(), "icon", *GetCamera());
                glEnable(GL_DEPTH_TEST);
            }
            
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
        Logger::Log(LogLevel::INFO, "Shutting down game...");
        
        GetShaderManager()->ClearAll();
        
        Logger::Log(LogLevel::INFO, "Game shutdown complete!");
    }

    void RenderUI() override
    {    
        if (!editorLayout)
        {
            Logger::Log(LogLevel::ERROR, "EditorLayout is null!");
            return;
        }

        editorLayout->RenderEditor(GetECSWorld(), GetCamera(), GetRenderer(), GetShaderManager(), GetMaterialManager());
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

            GetECSWorld()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(1));

            auto cubeMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
            GetECSWorld()->AddComponent<MeshComponent>(entity, cubeMesh);

            auto material = GetMaterialManager()->GetMaterial("gray");
            GetECSWorld()->AddComponent<MaterialComponent>(entity, material);

            GetECSWorld()->AddComponent<ColorComponent>(entity, glm::vec3(0.5f, 0.5f, 0.5f));
            GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            GetECSWorld()->AddComponent<RotationComponent >(entity, 50.0f);
            
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
                glm::vec3(0, 10, 0), 
                glm::vec3(45, 0, 0), 
                glm::vec3(1));
            
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
                glm::vec3(0, 5, 0), 
                glm::vec3(0), 
                glm::vec3(1));
            
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
                glm::vec3(0, 5, 0), 
                glm::vec3(45, 0, 0), 
                glm::vec3(1));
            
            GetECSWorld()->AddComponent<LightComponent>(entity, LightType::SPOT);
            GetECSWorld()->AddComponent<VisibilityComponent>(entity, true);
            
            GetECSWorld()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_spot.png", 0.35f);
            
            Logger::Log(LogLevel::INFO, "Spot light created with icon");
        });
    }
};