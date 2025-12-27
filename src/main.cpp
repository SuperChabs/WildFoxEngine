#include "application/Application.h"
#include "core/Shader.h"
#include "rendering/Skybox.h"
#include "utils/Logger.h"

#include <memory>

class MyGame : public Application 
{
private:
    std::unique_ptr<Shader> mainShader;
    std::unique_ptr<Shader> skyboxShader;
    Skybox* skybox;
    
    unsigned int diffuseMap;
    unsigned int normalMap;
    unsigned int depthMap;
    
    glm::vec3 lightPos;
    float heightScale;

protected:
    void OnInitialize() override 
    {
        Logger::Info("Initializing game...");
        
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
        skybox = new Skybox(cubemapTexture, skyboxShader.get());
        
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
        
        Logger::Info("Game initialized successfully");
    }
    
    void OnUpdate(float deltaTime) override 
    {
        if (GetInput()->IsKeyPressed(GLFW_KEY_Q))
        {
            if (heightScale > 0.0f)
                heightScale -= 0.5f * deltaTime;
            else
                heightScale = 0.0f;
        }
        else if (GetInput()->IsKeyPressed(GLFW_KEY_E))
        {
            if (heightScale < 1.0f)
                heightScale += 0.5f * deltaTime;
            else
                heightScale = 1.0f;
        }
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
        Logger::Info("Shutting down game...");
        
        delete skybox;
        
        Logger::Info("Game shutdown complete!");
    }

public:
    MyGame() : Application(1024, 720, "Parallax Mapping Demo") {}
};

int main() 
{
    MyGame game;
    
    if (!game.Initialize()) 
    {
        Logger::Error("Failed to initialize game");
        return -1;
    }
    
    game.Run();
    game.Shutdown();
    
    return 0;
}