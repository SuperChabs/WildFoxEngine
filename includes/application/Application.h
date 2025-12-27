#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/Window.h"
#include "core/Input.h"
#include "core/Time.h"
#include "core/Camera.h"
#include "core/ImGuiManager.h" 
#include "rendering/Renderer.h"
#include "rendering/TextureManager.h"
#include "scene/SceneManager.h"

#include <memory>

class Application 
{
private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Input> input;
    std::unique_ptr<Time> time;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<ImGuiManager> imGuiManager;
    
    bool isRunning;
    bool showDebugUI;
    bool cameraControlEnabled;
    
    void ProcessInput();
    void Update();
    void Render();
    void RenderDebugUI();

    void SetCameraControlMode(bool enabled);
    
    //static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

protected:
    Application(int width, int height, const std::string& title);

    virtual void OnInitialize() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnShutdown() {}

public:
    virtual ~Application();

    Application(const Application&) = default;
    Application& operator=(const Application&) = default;
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;    
    
    bool Initialize();
    void Run();
    void Shutdown();
    void Stop() { isRunning = false; }
    
    Window* GetWindow() const { return window.get(); }
    Input* GetInput() const { return input.get(); }
    Time* GetTime() const { return time.get(); }
    Camera* GetCamera() const { return camera.get(); }
    Renderer* GetRenderer() const { return renderer.get(); }
    SceneManager* GetSceneManager() const { return sceneManager.get(); }
    TextureManager* GetTextureManager() const { return textureManager.get(); }
};

#endif