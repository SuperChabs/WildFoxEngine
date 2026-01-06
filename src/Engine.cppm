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

export module XEngine.Engine;

import XEngine.Application.Application;
import XEngine.Core.Shader;
import XEngine.Core.Input;
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
    
    unsigned int diffuseMap;
    unsigned int normalMap;
    unsigned int depthMap;
    
    glm::vec3 lightPos;
    float heightScale;

    SceneObject* gameObject = nullptr;
    bool gameAutoRotate = false;
    float gameRotateSpeed = 50.0f; 

    std::unordered_map<uint64_t, ObjectParams> objectParams;

protected:
    void OnInitialize() override
    {

        Logger::Log(LogLevel::INFO, "Initializing XEngine...");
        
        // Завантаження шейдерів
        mainShader = std::make_unique<Shader>("basic");
        skyboxShader = std::make_unique<Shader>("skybox"); //
        
        // Завантаження текстур
        diffuseMap = GetTextureManager()->LoadTexture("assets/textures/bricks2.jpg");
        normalMap = GetTextureManager()->LoadTexture("assets/textures/bricks2_normal.jpg");
        depthMap = GetTextureManager()->LoadTexture("assets/textures/bricks2_disp.jpg");
        
        // Створення скайбоксу
        std::vector<std::string> faces = 
        {
            "assets/textures/skybox3/right.png",
            "assets/textures/skybox3/left.png",
            "assets/textures/skybox3/top.png",
            "assets/textures/skybox3/bottom.png",
            "assets/textures/skybox3/front.png",
            "assets/textures/skybox3/back.png"
        };
        
        unsigned int cubemapTexture = GetTextureManager()->LoadCubemap(faces);
        skybox = std::make_unique<Skybox>(cubemapTexture, skyboxShader.get());
        
        Logger::Log(LogLevel::INFO, "Creating EditorLayout...");
        editorLayout = std::make_unique<EditorLayout>();
        
        if (!editorLayout)
            Logger::Log(LogLevel::ERROR, "Failed to create EditorLayout!");
        else
            Logger::Log(LogLevel::INFO, "EditorLayout created successfully!");

        // Налаштування шейдерів
        mainShader->use();
        mainShader->setInt("diffuseMap", 0);
        mainShader->setInt("normalMap", 1);
        mainShader->setInt("depthMap", 2);
        
        skyboxShader->use();
        skyboxShader->setInt("skybox", 0);
        
        // Ініціалізація змінних
        lightPos = glm::vec3(0.5f, 1.0f, 0.3f);
        heightScale = 0.1f;
        
        Logger::Log(LogLevel::INFO, "Game initialized successfully");
    }

    void OnUpdate(float deltaTime) override
    {
        if (GetInput()->IsKeyPressed(XKey::KEY_Q))
        {
            if (heightScale > 0.0f)
                heightScale -= 0.5f * deltaTime;
            else
                heightScale = 0.0f;
        }
        else if (GetInput()->IsKeyPressed(XKey::KEY_E))
        {
            if (heightScale < 1.0f)
                heightScale += 0.5f * deltaTime;
            else
                heightScale = 1.0f;
        }

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
        // === РЕНДЕР 3D СЦЕНИ В FRAMEBUFFER ===
        if (editorLayout && editorLayout->GetFramebuffer())
        {
            Framebuffer* fb = editorLayout->GetFramebuffer();
            ImVec2 viewportSize = editorLayout->GetViewportSize();
            
            // Прив'язуємо framebuffer
            fb->Bind();
            
            // Очищаємо (тестовий червоний колір)
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            
            if (viewportSize.x > 0 && viewportSize.y > 0)
            {
                // Матриці для 3D
                glm::mat4 projection = glm::perspective(
                    glm::radians(GetCamera()->GetZoom()), 
                    viewportSize.x / viewportSize.y, 
                    0.1f, 100.0f
                );
                glm::mat4 view = GetCamera()->GetViewMatrix();
                
                // Рендеримо 3D сцену
                mainShader->use();
                mainShader->setMat4("projection", projection);
                mainShader->setMat4("view", view);
                mainShader->setVec3("viewPos", GetCamera()->GetPosition());
                mainShader->setVec3("lightPos", lightPos);
                mainShader->setFloat("height_scale", heightScale);
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, diffuseMap);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, normalMap);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, depthMap);
                
                GetSceneManager()->RenderAll(*mainShader);
                skybox->Render(*GetCamera(), projection);
            }
            
            // Відв'язуємо framebuffer
            fb->Unbind();
        }
        
        // === ПІДГОТОВКА ЕКРАНУ ДЛЯ IMGUI ===
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, GetWindow()->GetWidth(), GetWindow()->GetHeight());
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);  // Темно-сірий фон
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

        // Просто рендеримо UI панелі
        editorLayout->RenderEditor(
            GetSceneManager(),
            GetCamera(),
            GetRenderer(),
            [this]() {
                Mesh* gameMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
                if (gameMesh)
                {
                    std::unique_ptr<Model> gameModel = std::make_unique<Model>(gameMesh, "Cube");
                    SceneObject* newObj = GetSceneManager()->AddObject("Cube", std::move(gameModel));
                    objectParams[newObj->objectID] = ObjectParams();
                    Logger::Log(LogLevel::INFO, "Cube created with ID: " + std::to_string(newObj->objectID));
                }
            }
        );
    }
    // void Menu();
    // void SceneSettings();

public:
    Engine(int w, int h, const std::string& title)
        : Application(w, h, title)
    {}
};