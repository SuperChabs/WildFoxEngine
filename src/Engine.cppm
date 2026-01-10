module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <variant>

export module XEngine.Engine;

import XEngine.Application.Application;
import XEngine.Core.Shader;
import XEngine.Core.Input;
import XEngine.Core.CommandManager;
import XEngine.Rendering.Skybox;
import XEngine.Rendering.Framebuffer;
import XEngine.Rendering.Primitive.PrimitivesFactory;
import XEngine.Scene.SceneManager;
import XEngine.Scene.Mesh;
import XEngine.Scene.Model;
import XEngine.UI.EditorLayout;
import XEngine.Core.Logger;

export class Engine : public Application 
{
private:
    std::unique_ptr<Shader> mainShader;
    std::unique_ptr<Shader> skyboxShader;
    std::unique_ptr<Skybox> skybox;

    std::unique_ptr<EditorLayout> editorLayout;

    SceneObject* gameObject = nullptr;
    bool gameAutoRotate = false;
    float gameRotateSpeed = 50.0f; 

    std::unordered_map<uint64_t, ObjectParams> objectParams;

protected:
    void OnInitialize() override
    {

        Logger::Log(LogLevel::INFO, "Initializing XEngine...");
        
        mainShader = std::make_unique<Shader>("basic");
        skyboxShader = std::make_unique<Shader>("skybox"); 

        GetMaterialManager()->CreateColorMaterial("orange", glm::vec3(1.0f, 0.5f, 0.0f));

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
        skybox = std::make_unique<Skybox>(cubemapTexture, skyboxShader.get());
        
        CommandManager::RegisterCommand("onCreateCube",
        [this](const CommandArgs&) 
        {
            Mesh* gameMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
            if (gameMesh)
            {
                auto material = GetMaterialManager()->GetMaterial("gray");
                gameMesh->SetMaterial(material);

                std::unique_ptr<Model> gameModel = std::make_unique<Model>(gameMesh, "Cube");
                SceneObject* newObj = GetSceneManager()->AddObject("Cube", std::move(gameModel));
                objectParams[newObj->objectID] = ObjectParams();
                Logger::Log(LogLevel::INFO, "Cube created with ID: " + std::to_string(newObj->objectID));
            }
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
                return;

            const auto& color = std::get<glm::vec3>(args[0]);

            SceneObject* selected = editorLayout->GetSelectedObject();
            auto it = selected->model->GetMeshCount();

            if (selected)
                for (size_t i = 0; i < it; i++)
                {
                    Mesh* mesh = selected->model->GetMesh(i);
                    mesh->SetColor(glm::vec3(color));
                }
        });

        Logger::Log(LogLevel::INFO, "Creating EditorLayout...");
        editorLayout = std::make_unique<EditorLayout>();
        
        if (!editorLayout)
            Logger::Log(LogLevel::ERROR, "Failed to create EditorLayout!");
        else
            Logger::Log(LogLevel::INFO, "EditorLayout created successfully!");
        
        skyboxShader->use();
        skyboxShader->setInt("skybox", 0);
        
        Logger::Log(LogLevel::INFO, "Game initialized successfully");
    }

    void OnUpdate(float deltaTime) override
    {
        if (GetSceneManager()->GetObjectCount() > 0 && gameObject)
        {
            for (const auto& [id, obj] : GetSceneManager()->GetObjects()) 
            {
                auto& params = objectParams[obj->objectID];
                
                if (params.autoRotate)
                    obj->transform.Rotate(glm::vec3(0.0f, params.rotateSpeed * deltaTime, 0.0f));
                
            }
        }
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
                
                mainShader->use();

                GLint currentProgram;
                glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
                if (currentProgram != mainShader->ID) 
                    Logger::Log(LogLevel::ERROR, "Shader not active!");
                
                mainShader->setMat4("projection", projection);
                mainShader->setMat4("view", view);
                mainShader->setVec3("viewPos", GetCamera()->GetPosition());
                
                static int frameCount = 0;
                if (frameCount % 60 == 0) 
                    Logger::Log(LogLevel::DEBUG, "Rendering " + 
                        std::to_string(GetSceneManager()->GetObjectCount()) + " objects");
                frameCount++; 
                
                GetSceneManager()->RenderAll(*mainShader);
                skybox->Render(*GetCamera(), projection);
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
        
        mainShader.reset();
        skyboxShader.reset();
        skybox.reset();
        
        Logger::Log(LogLevel::INFO, "Game shutdown complete!");
    }

    void RenderUI() override
    {    
        if (!editorLayout)
        {
            Logger::Log(LogLevel::ERROR, "EditorLayout is null!");
            return;
        }

        editorLayout->RenderEditor( GetSceneManager(), GetCamera(), GetRenderer());
    }

public:
    Engine(int w, int h, const std::string& title)
        : Application(w, h, title)
    {}
};