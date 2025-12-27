#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/Window.h"
#include "core/Input.h"
#include "core/Time.h"
#include "core/Camera.h"
#include "rendering/Renderer.h"
#include "rendering/TextureManager.h"
#include "scene/SceneManager.h"

class Application 
{
private:
    Window* window;
    Input* input;
    Time* time;
    Camera* camera;
    Renderer* renderer;
    TextureManager* textureManager;
    SceneManager* sceneManager;
    
    bool isRunning;
    
    static Application* instance;
    
    void ProcessInput();
    void Update();
    void Render();
    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

protected:
    virtual void OnInitialize() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnShutdown() {}

public:
    Application(int width, int height, const std::string& title);
    virtual ~Application();
    
    bool Initialize();
    void Run();
    void Shutdown();
    void Stop() { isRunning = false; }
    
    Window* GetWindow() { return window; }
    Input* GetInput() { return input; }
    Time* GetTime() { return time; }
    Camera* GetCamera() { return camera; }
    Renderer* GetRenderer() { return renderer; }
    SceneManager* GetSceneManager() { return sceneManager; }
    TextureManager* GetTextureManager() { return textureManager; }
    
    static Application* GetInstance() { return instance; }
};

#endif