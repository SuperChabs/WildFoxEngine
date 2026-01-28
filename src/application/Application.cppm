module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <memory>
#include <utility>

export module WFE.Application.Application;

import WFE.Core.Window; 
import WFE.Core.Input;
import WFE.Core.Time;
import WFE.Core.Camera;
import WFE.Core.Logger;
import WFE.Core.Logging.ConsoleLogger;
import WFE.Core.Logging.FileLogger;
import WFE.Core.CommandManager;
import WFE.ECS.ECSWorld;
import WFE.Rendering.Renderer; 
import WFE.Resource.Texture.TextureManager;
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Shader.ShaderManager;
import WFE.Resource.Model.ModelManager;

import WFE.UI.ImGuiManager;

/// @file Application.hpp
/// @brief Main application class for the WildFoxEngine
/// @author 0x00
/// @date 2026-01-28

/**
 * @class Application
 * @brief Core engine class that manages the application lifecycle
 * 
 * Responsibilities:
 * - Engine initialization
 * - Main render loop
 * - Event handling
 * - Resource loading
 */
export class Application 
{
private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Input> input;
    std::unique_ptr<Time> time;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<ImGuiManager> imGuiManager;
    std::unique_ptr<MaterialManager> materialManager;
    std::unique_ptr<ECSWorld> ecsWorld;
    std::unique_ptr<ShaderManager> shaderManager;
    std::unique_ptr<ModelManager> modelManager;
    
    ConsoleLogger console;
    FileLogger file;

    bool isRunning; ///< Whether the application is currently running
    bool showUI;
    
    void ProcessInput()
    {
        if (input->IsKeyPressed(Key::KEY_ESCAPE)) 
            Stop();
        
        if (input->IsKeyJustPressed(Key::KEY_F1)) 
            showUI = !showUI;

        if (input->IsKeyJustPressed(Key::KEY_F5))
        {
            shaderManager->ReloadAll();
            Logger::Log(LogLevel::INFO, "Reloaded all shaders");
        }

        // Ctrl+S - Quick Save
        if (input->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            input->IsKeyJustPressed(Key::KEY_S))
        {
            if (CommandManager::HasCommand("onQuickSave"))
                CommandManager::ExecuteCommand("onQuickSave", {});
        }
        
        // Ctrl+L - Quick Load
        if (input->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            input->IsKeyJustPressed(Key::KEY_L))
        {
            if (CommandManager::HasCommand("onQuickLoad"))
                CommandManager::ExecuteCommand("onQuickLoad", {});
        }
        
        // Ctrl+N - New Scene
        if (input->IsKeyPressed(Key::KEY_LEFT_CONTROL) && 
            input->IsKeyJustPressed(Key::KEY_N))
        {
            if (CommandManager::HasCommand("onNewScene"))
                CommandManager::ExecuteCommand("onNewScene", {});
        }
        
        if (cameraControlEnabled) 
        {
            if (input->IsKeyPressed(Key::KEY_W)) 
                camera->ProcessKeyboard(FORWARD, time->GetDeltaTime());
            if (input->IsKeyPressed(Key::KEY_S)) 
                camera->ProcessKeyboard(BACKWARD, time->GetDeltaTime()); 
            if (input->IsKeyPressed(Key::KEY_A)) 
                camera->ProcessKeyboard(LEFT, time->GetDeltaTime());
            if (input->IsKeyPressed(Key::KEY_D)) 
                camera->ProcessKeyboard(RIGHT, time->GetDeltaTime());

            if (input->IsKeyPressed(Key::KEY_SPACE)) 
                camera->ProcessKeyboard(UP, time->GetDeltaTime());
            if (input->IsKeyPressed(Key::KEY_LEFT_SHIFT)) 
                camera->ProcessKeyboard(DOWN, time->GetDeltaTime());
        }
    }

    void Update()
    {
        float deltaTime = time->GetDeltaTime();
        
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

    /**
     * @brief Constructor
     * @param title Window title
     * @param width Window width in pixels
     * @param height Window height in pixels
     */
    Application(int width, int height, const std::string& title)
        : window(nullptr), input(nullptr), time(nullptr), camera(nullptr),
            renderer(nullptr), textureManager(nullptr), isRunning(false), 
            showUI(true), cameraControlEnabled(false)
    {
        window = std::make_unique<Window>(width, height, title);

        Logger::Log(LogLevel::INFO, "Application created");
    }

    virtual void OnInitialize() {}

    /**
     * @brief Updates the application state
     * @param deltaTime Time elapsed since the last frame in seconds
     */
    virtual void OnUpdate(float deltaTime) {}

    /**
     * @brief Renders the scene
     * @note Called every frame after OnUpdate()
     */
    virtual void OnRender() {}
    virtual void OnShutdown() {}

    virtual void RenderUI() = 0;

    void SetRenderer(std::unique_ptr<Renderer> r)
    {
        renderer = std::move(r);
        Logger::Log(LogLevel::INFO, "Renderer set in Application");
    }

    void SetCameraControlMode(bool enabled)
    {
        cameraControlEnabled = enabled;
        
        if (enabled) 
        {
            window->SetCursorMode(GLFW_CURSOR_DISABLED);
            Logger::Log(LogLevel::INFO, "Camera control: ON");
        } 
        else 
        {
            window->SetCursorMode(GLFW_CURSOR_NORMAL);
            Logger::Log(LogLevel::INFO, "Camera control: OFF (UI mode)");
        }
    }

public:
    /**
     * @brief Destructor
     * Releases all resources used by the application
     */
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
        camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 0.0f));
        renderer = nullptr; // std::make_unique<Renderer>(shaderManager.get(), ecsWorld.get());
        textureManager = std::make_unique<TextureManager>();
        imGuiManager = std::make_unique<ImGuiManager>();
        materialManager = std::make_unique<MaterialManager>(textureManager.get());
        ecsWorld = std::make_unique<ECSWorld>();
        shaderManager = std::make_unique<ShaderManager>();
        modelManager = std::make_unique<ModelManager>();
        
        Logger::Log(LogLevel::DEBUG, "ShaderManager address: " + 
            std::to_string(reinterpret_cast<uintptr_t>(shaderManager.get())));
        Logger::Log(LogLevel::DEBUG, "ECSWorld address: " + 
            std::to_string(reinterpret_cast<uintptr_t>(ecsWorld.get())));

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        imGuiManager = std::make_unique<ImGuiManager>();
        if (!imGuiManager->Initialize(window->GetGLFWWindow())) 
            return false;

        SetCameraControlMode(false);  
        
        Logger::AddSink(&console);
        Logger::AddSink(&file);
        
        OnInitialize();

        Logger::Log(LogLevel::INFO, "Application initialized successfully");
        
        return true;
    }

    /**
     * @brief Starts the main application loop
     * @note Calls OnUpdate() and OnRender() every frame
     */
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

        if (imGuiManager)
        {
            imGuiManager->Shutdown();
            imGuiManager.reset();
        }        

        Logger::RemoveSink(&console);
        
        Logger::Log(LogLevel::INFO, "Application shutdown complete");
    }

    /**
     * @brief Closes the application
     * Stops the main loop
     */
    void Stop() 
    { 
        isRunning = false; 
    }
    
    Window* GetWindow() const { return window.get(); }
    Input* GetInput() const { return input.get(); }
    Time* GetTime() const { return time.get(); }
    Camera* GetCamera() const { return camera.get(); }
    Renderer* GetRenderer() const { return renderer.get(); }
    TextureManager* GetTextureManager() const { return textureManager.get(); }
    ImGuiManager* GetImGuiManager() const { return imGuiManager.get(); }
    MaterialManager* GetMaterialManager() const { return materialManager.get(); }
    ECSWorld* GetECSWorld() const { return ecsWorld.get(); }
    ShaderManager* GetShaderManager() const { return shaderManager.get(); }
    ModelManager* GetModelManager() const { return modelManager.get(); }
};