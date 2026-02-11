module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include <memory>
#include <utility>

export module WFE.Application.Application;

import WFE.Core.Window; 
import WFE.Core.Input;
import WFE.Core.Time;

import WFE.Core.Logger;
import WFE.Core.Logging.ConsoleLogger;
import WFE.Core.Logging.FileLogger;

import WFE.Core.CommandManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Systems;
import WFE.ECS.Components;
import WFE.UI.ImGuiManager;
import WFE.Core.ModuleManager;

/// @file Application.cppm
/// @brief Main application class for the WildFoxEngine
/// @author SuperChabs
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

    std::unique_ptr<ModuleManager> moduleManager;
    
    ConsoleLogger console;
    FileLogger file;

    bool isRunning;

    void Update()
    {
        float deltaTime = time->GetDeltaTime();

        OnUpdate(deltaTime);
    }

protected:

    /**
     * @brief Constructor
     * @param title Window title
     * @param width Window width in pixels
     * @param height Window height in pixels
     */
    Application(int width, int height, const std::string& title)
        : window(nullptr), input(nullptr), time(nullptr), 
          isRunning(false)
    {
        window = std::make_unique<Window>(width, height, title);

        Logger::Log(LogLevel::INFO, "Application created");
    }

    virtual void OnInitialize() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnShutdown() {}

    virtual bool ShouldAllowCameraControl() const { return true; }

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
    
    /**
     * @brief main initializer
     * Initialize all systems required for base working of engine
     */
    bool Initialize()
    {
        if (!window->Initialize())
        {
            Logger::Log(LogLevel::ERROR, "Failed to initialize Window");
            return false;
        }

        glfwSetWindowUserPointer(window->GetGLFWWindow(), this);
        
        input = std::make_unique<Input>(window->GetGLFWWindow());
        time = std::make_unique<Time>();

        moduleManager = std::make_unique<ModuleManager>();  
        
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
        
            Update();
            OnRender();
            
            window->SwapBuffers();
            window->PollEvents();
        }
    }

    /**
     * @brief well, it do what in method name stands for
     */
    void Shutdown()
    {
        OnShutdown();     

        Logger::RemoveSink(&console);
        Logger::RemoveSink(&file);
        
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
    
    /// \name Getters
    /// @{
    Window* GetWindow() { return window.get(); }
    Input* GetInput() { return input.get(); }
    Time* GetTime() const { return time.get(); }

    ModuleManager* GetModuleManager() { return moduleManager.get(); }
    /// @}
};