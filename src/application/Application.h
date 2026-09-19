#pragma once

#include <string>
#include <memory>

#include "core/CoreModule.h"
#include "core/logging/ConsoleLogger.h"
#include "core/logging/FileLogger.h"
#include "core/ModuleManager.h"

/// @file Application.cpp
/// @brief Main application class for the WildFoxEngine
/// @author SuperChabs
/// @date 2026-01-28

constexpr float FIXED_DT = 1.0f / 60.0f;
static float accumulator = 0.0f;

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
class Application {
    std::unique_ptr<ModuleManager> moduleManager;
    CoreModule *m_coreModule;

    ConsoleLogger console;
    FileLogger file;

    bool isRunning;

protected:
    /**
     * @brief Constructor
     * @param title Window title
     * @param width Window width in pixels
     * @param height Window height in pixels
     */
    Application(int width, int height, const std::string &title);

    virtual void OnInitialize() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender() {}
    virtual void OnSimulation(float deltaTime) {}
    virtual void OnShutdown() {}

    virtual bool ShouldAllowCameraControl() const { return true; }

public:
    /**
     * @brief Destructor
     * Releases all resources used by the application
     */
    virtual ~Application();

    Application(const Application &) = delete;

    Application &operator=(const Application &) = delete;

    Application(Application &&) = delete;

    Application &operator=(Application &&) = delete;

    /**
     * @brief main initializer
     * Initialize all systems required for base working of engine
     */
    bool Initialize();

    /**
     * @brief Starts the main application loop
     * @note Calls OnUpdate() and OnRender() every frame
     */
    void Run();

    /**
     * @brief well, it does what in method name stands for
     */
    void Shutdown();

    /**
     * @brief Closes the application
     * Stops the main loop
     */
    void Stop();

    /// \name Getters
    /// @{

    ModuleManager *GetModuleManager() const;

    /// @}
};
