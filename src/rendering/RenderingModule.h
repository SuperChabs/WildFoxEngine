#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/IModule.h"
#include "core/ModuleManager.h"
#include "rendering/Renderer.h"
#include "scene/Skybox.h"
#include "ECS/World.h"
#include "ECS/systems/Systems.h"
#include "resource/ResourceModule.h"

/// @file RenderingModule.cppm
/// @brief Module responsible for all graphics and rendering
/// @author SuperChabs
/// @date 2026-02-06

/**
 * @class RenderingModule
 * @brief Encapsulates all rendering functionality
 * 
 * Responsibilities:
 * - Initialize OpenGL context
 * - Create and manage Renderer
 * - Create Skybox
 * - Setup rendering systems
 */
class RenderingModule : public IModule {
private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Skybox> skybox;

    ResourceModule *resourceModule;

    ECSWorld *ecsWorld = nullptr;
    GLFWwindow *window = nullptr;

    bool hasOpenGL = false;

public:
    /**
     * @brief Constructor
     * @param win GLFW window (for OpenGL context)
     * @param ecs ECS World (for render systems)
     */
    RenderingModule(GLFWwindow *win, ECSWorld *ecs, ModuleManager *mm);

    /**
     * @brief Initialize rendering module
     * @return true if successful, false if critical error occurred
     */
    bool Initialize() override;

    /**
     * @brief Update module (called every frame)
     * @param deltaTime Time since last frame
     */
    void Update(float deltaTime) override;

    /**
     * @brief Shutdown module
     */
    void Shutdown() override;

    /// @name IModule interface
    /// @{
    const char *GetName() const override;

    int GetPriority() const override;

    bool IsRequired() const override;

    /// @}

    /// @name Getters
    /// @{
    Renderer *GetRenderer();

    Skybox *GetSkybox();

    bool HasOpenGL() const;

    /// @}

private:
    /**
     * @brief Initialize OpenGL context
     * @return true if OpenGL is available
     */
    bool InitializeOpenGL();

    /**
     * @brief Load shaders and materials
     * @return true if successful
     */
    bool LoadResources();

    /**
     * @brief Create Skybox
     * @return true if successful
     */
    bool CreateSkybox();

    /**
     * @brief Create Renderer instance
     * @return true if successful
     */
    bool CreateRenderer();

    /**
     * @brief Initialize Renderer with skybox data
     * @return true if successful
     */
    bool InitializeRenderer();
};