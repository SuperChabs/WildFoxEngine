module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <memory>

export module XEngine.Application.Application;

import XEngine.Core.Window; 
import XEngine.Core.Input;
import XEngine.Core.Time;
import XEngine.Core.Camera;
import XEngine.Core.ImGuiManager;
import XEngine.Rendering.Renderer;
import XEngine.Rendering.TextureManager;
import XEngine.Scene.SceneManager;
import XEngine.Core.Logger;
import XEngine.Core.Logging.ConsoleLogger;

export class Application 
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
    
    ConsoleLogger console;

    bool isRunning;
    bool showUI;
    
    void ProcessInput()
    {
        if (input->IsKeyPressed(XKey::KEY_ESCAPE)) 
            Stop();
        
        if (input->IsKeyJustPressed(XKey::KEY_F1)) 
            showUI = !showUI;
        
        if (cameraControlEnabled) 
        {
            if (input->IsKeyPressed(XKey::KEY_W)) 
                camera->ProcessKeyboard(FORWARD, time->GetDeltaTime());
            if (input->IsKeyPressed(XKey::KEY_S)) 
                camera->ProcessKeyboard(BACKWARD, time->GetDeltaTime()); 
            if (input->IsKeyPressed(XKey::KEY_A)) 
                camera->ProcessKeyboard(LEFT, time->GetDeltaTime());
            if (input->IsKeyPressed(XKey::KEY_D)) 
                camera->ProcessKeyboard(RIGHT, time->GetDeltaTime());

            if (input->IsKeyPressed(XKey::KEY_SPACE)) 
                camera->ProcessKeyboard(UP, time->GetDeltaTime());
            if (input->IsKeyPressed(XKey::KEY_LEFT_SHIFT)) 
                camera->ProcessKeyboard(DOWN, time->GetDeltaTime());
        }
    }

    void Update()
    {
        float deltaTime = time->GetDeltaTime();
        sceneManager->Update(deltaTime);
        
        OnUpdate(deltaTime);
    }

    void Render()
    {
        OnRender();  

        if (showUI) 
        {
            int width = window->GetWidth();
            int height = window->GetHeight();
            glViewport(0, 0, width, height);
            glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);
            
            imGuiManager->BeginFrame();
            RenderUI();
            imGuiManager->EndFrame();
        } 
    }
    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app && app->GetWindow())
            app->GetWindow()->SetSize(width, height);
    }

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        
        if (!app || !app->input || !app->camera)
            return;
        
        app->input->UpdateMousePosition(xpos, ypos);
        
        if (app->cameraControlEnabled) 
        {
            glm::vec2 delta = app->input->GetMouseDelta();
            app->camera->ProcessMouseMovement(delta.x, delta.y);
        }
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        
        if (!app || !app->input || !app->camera)
            return;
        
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) 
            app->SetCameraControlMode(!app->cameraControlEnabled);
    }

protected:
    bool cameraControlEnabled;

    Application(int width, int height, const std::string& title)
        : window(nullptr), input(nullptr), time(nullptr), camera(nullptr),
            renderer(nullptr), textureManager(nullptr), sceneManager(nullptr),
            isRunning(false), showUI(true), cameraControlEnabled(false)
    {
        window = std::make_unique<Window>(width, height, title);

        Logger::Log(LogLevel::INFO, "Application created");
    }

    virtual void OnInitialize() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnShutdown() {}

    virtual void RenderUI() = 0;

    void SetCameraControlMode(bool enabled)
    {
        cameraControlEnabled = enabled;
        
        if (enabled) 
        {
            window->SetCursorMode(GLFW_CURSOR_DISABLED);
            Logger::Log(LogLevel::INFO, "Camera control: ON");
        } else 
        {
            window->SetCursorMode(GLFW_CURSOR_NORMAL);
            Logger::Log(LogLevel::INFO, "Camera control: OFF (UI mode)");
        }
    }

public:
    virtual ~Application()
    {
        Shutdown();
    }

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;    
    
    bool Initialize()
    {
        if (!window->Initialize())
        {
            Logger::Log(LogLevel::ERROR, "Failed to initialize Window");
            return false;
        }

        glfwSetWindowUserPointer(window->GetGLFWWindow(), this);
        
        window->SetFramebufferSizeCallback(FramebufferSizeCallback);
        window->SetCursorPosCallback(MouseCallback);
        window->SetScrollCallback(Input::ScrollCallback);
        window->SetMouseButtonCallback(MouseButtonCallback);
        
        input = std::make_unique<Input>(window->GetGLFWWindow());
        time = std::make_unique<Time>();
        camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
        renderer = std::make_unique<Renderer>();
        textureManager = std::make_unique<TextureManager>();
        sceneManager = std::make_unique<SceneManager>();
        imGuiManager = std::make_unique<ImGuiManager>();
        
        renderer->Initialize();
        
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        imGuiManager = std::make_unique<ImGuiManager>();
        if (!imGuiManager->Initialize(window->GetGLFWWindow())) 
            return false;

        SetCameraControlMode(false);  
        
        Logger::AddSink(&console);
        
        OnInitialize();

        Logger::Log(LogLevel::INFO, "Application initialized successfully");
        
        return true;
    }

    void Run()
    {
        isRunning = true;
        
        while (isRunning && !window->ShouldClose())
        {
            time->Update();
            
            ProcessInput();
            Update();
            Render();
            
            window->SwapBuffers();
            window->PollEvents();
        }
    }

    void Shutdown()
    {
        OnShutdown();

        Logger::RemoveSink(&console);
        
        Logger::Log(LogLevel::INFO, "Application shutdown complete");
    }

    void Stop() 
    { 
        isRunning = false; 
    }
    
    Window* GetWindow() const { return window.get(); }
    Input* GetInput() const { return input.get(); }
    Time* GetTime() const { return time.get(); }
    Camera* GetCamera() const { return camera.get(); }
    Renderer* GetRenderer() const { return renderer.get(); }
    SceneManager* GetSceneManager() const { return sceneManager.get(); }
    TextureManager* GetTextureManager() const { return textureManager.get(); }
    ImGuiManager* GetImGuiManager() const { return imGuiManager.get(); }
};