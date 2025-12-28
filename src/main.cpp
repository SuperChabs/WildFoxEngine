#include "application/Application.h"
#include "core/Shader.h"
#include "rendering/Skybox.h"
#include "utils/Logger.h"

#include <imgui.h>

#include <memory>

#define DEBUG
class MyGame : public Application 
{
private:
    std::unique_ptr<Shader> mainShader;
    std::unique_ptr<Shader> skyboxShader;
    std::unique_ptr<Skybox> skybox;
    
    unsigned int diffuseMap;
    unsigned int normalMap;
    unsigned int depthMap;
    
    glm::vec3 lightPos;
    float heightScale;

protected:
    void OnInitialize() override 
    {
        Logger::Log(LogLevel::INFO, "Initializing game...");
        
        // Завантаження шейдерів
        mainShader = std::make_unique<Shader>("parallax");
        skyboxShader = std::make_unique<Shader>("skybox");
        
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
    }

    void RenderUI() override
    {    
        Menu();
        SceneSettings();
    }
    
    void Menu() 
    {
        ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Ctrl+W"))  { Stop(); }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        float fps = 1.0f / GetTime()->GetDeltaTime();
        ImGui::Text("FPS: %", fps);
        ImGui::Text("Frame time: %.3f ms", GetTime()->GetDeltaTime() * 1000.0f);
        
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Camera")) 
        {
            glm::vec3 pos = GetCamera()->GetPosition();
            ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
            
            float speed = GetCamera()->GetMovementSpeed();
            if (ImGui::SliderFloat("Speed", &speed, 1.0f, 20.0f)) 
                GetCamera()->SetMovementSpeed(speed);

            float sensitivity = GetCamera()->GetMouseSensitivity();
            if (ImGui::SliderFloat("Sensitivity", &sensitivity, 0.01f, 1.0f)) 
                GetCamera()->SetMouseSensitivity(sensitivity);

            if (ImGui::SliderFloat3("Position", &pos[0], -10.0f, 10.0f)) 
                GetCamera()->SetPosition(pos);

            float yaw = GetCamera()->GetYaw();
            if(ImGui::SliderFloat("Yaw", &yaw, -360.0f, 360.0f)) 
            {
                GetCamera()->SetYaw(yaw);
            }

            float pitch = GetCamera()->GetPitch();
            if(ImGui::SliderFloat("Pitch", &pitch, -90.0f, 90.0f)) 
            {
                GetCamera()->SetPitch(pitch);
            }

            ImGui::Separator();

            if (ImGui::Button("Reset Position")) 
                GetCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

            if (ImGui::Button("Reset Orientation")) 
            {
                GetCamera()->SetYaw(-95.0f);
                GetCamera()->SetPitch(0.0f);
            }

            if (ImGui::Button("Reset All")) 
            {
                GetCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                GetCamera()->SetYaw(-95.0f);
                GetCamera()->SetPitch(0.0f);
            }

            if (ImGui::Button(cameraControlEnabled ? "Disable Camera Control" : "Enable Camera Control")) 
                SetCameraControlMode(!cameraControlEnabled);
        }

        if (ImGui::CollapsingHeader("Window")) 
        {
            ImGui::Text("Size: %dx%d", GetWindow()->GetWidth(), GetWindow()->GetHeight());
            ImGui::Text("Aspect: %.2f", GetWindow()->GetAspectRatio());
        }
        
        ImGui::Separator();
        ImGui::Text("Press F1 to toggle this window");
        
        ImGui::End();
    }

    void SceneSettings() 
    {
        ImGui::Begin("Scene Settings");

        ImGui::Text("Light Position");
        if (ImGui::SliderFloat3("Light Pos", &lightPos[0], -5.0f, 5.0f))
            mainShader->setVec3("lightPos", lightPos);

        ImGui::Text("Height Scale");
        if (ImGui::SliderFloat("Height Scale", &heightScale, 0.0f, 1.0f))
            mainShader->setFloat("height_scale", heightScale);

        ImGui::End();
    }

    void OnRender() override 
    {
        int width = GetWindow()->GetWidth();
        int height = GetWindow()->GetHeight();
        
        // Матриці
        glm::mat4 projection = glm::perspective(glm::radians(GetCamera()->GetZoom()), GetWindow()->GetAspectRatio(), 0.1f, 100.0f);
        glm::mat4 view = GetCamera()->GetViewMatrix();
        
        mainShader->use();
        mainShader->setMat4("projection", projection);
        mainShader->setMat4("view", view);
        mainShader->setMat4("model", glm::mat4(1.0f));
        mainShader->setVec3("viewPos", GetCamera()->GetPosition());
        mainShader->setVec3("lightPos", lightPos);
        mainShader->setFloat("height_scale", heightScale);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        
        GetRenderer()->GetPrimitives()->RenderQuad();
        
        // Рендеринг позиції світла
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f));
        mainShader->setMat4("model", lightModel);
        GetRenderer()->GetPrimitives()->RenderQuad();
        
        // Рендеринг скайбоксу
        skybox->Render(*GetCamera(), projection);
    }
    
    void OnShutdown() override 
    {
        Logger::Log(LogLevel::INFO, "Shutting down game...");
        
        mainShader.reset();
        skyboxShader.reset();
        skybox.reset();
        
        Logger::Log(LogLevel::INFO, "Game shutdown complete!");
    }

public:
    MyGame() : Application(1020, 800, "Game Demo") {}
};

int main() 
{
    MyGame game;
    
    if (!game.Initialize()) 
    {
        Logger::Log(LogLevel::ERROR, "Failed to initialize game");
        return -1;
    }
    
    game.Run();
    game.Shutdown();
    
    return 0;
}