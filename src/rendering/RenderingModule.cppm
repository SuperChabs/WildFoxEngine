module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

export module WFE.Rendering.RenderingModule;

import WFE.Core.IModule;
import WFE.Core.Logger;

// Rendering
import WFE.Rendering.Renderer;
import WFE.Scene.Skybox;

// Resource Managers
import WFE.Resource.Shader.ShaderManager;
import WFE.Resource.Texture.TextureManager;
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Model.ModelManager;

// ECS
import WFE.ECS.ECSWorld;
import WFE.ECS.Systems;

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
 * - Manage resources (shaders, textures, materials, models)
 * - Create Skybox
 * - Setup rendering systems
 */
export class RenderingModule : public IModule
{
private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Skybox> skybox;
    
    std::unique_ptr<ShaderManager> shaderManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<MaterialManager> materialManager;
    std::unique_ptr<ModelManager> modelManager;
    
    std::unique_ptr<IconRenderSystem> iconRenderSystem;
    
    ECSWorld* ecsWorld = nullptr;
    GLFWwindow* window = nullptr;
    
    bool initialized = false;
    bool hasOpenGL = false;

public:
    /**
     * @brief Constructor
     * @param win GLFW window (for OpenGL context)
     * @param ecs ECS World (for render systems)
     */
    RenderingModule(GLFWwindow* win, ECSWorld* ecs)
        : window(win)
        , ecsWorld(ecs)
    {
        if (!window)
            Logger::Log(LogLevel::ERROR, "RenderingModule: window is null!");
        if (!ecsWorld)
            Logger::Log(LogLevel::ERROR, "RenderingModule: ecsWorld is null!");
    }
    
    /**
     * @brief Initialize rendering module
     * @return true if successful, false if critical error occurred
     */
    bool Initialize() override
    {
        Logger::Log(LogLevel::INFO, "=== Initializing RenderingModule ===");
        
        if (!InitializeOpenGL())
        {
            Logger::Log(LogLevel::WARNING, 
                "OpenGL not available - running in headless mode");
            hasOpenGL = false;
            return true; 
        }
        
        hasOpenGL = true;
        
        if (!CreateResourceManagers())
        {
            Logger::Log(LogLevel::ERROR, "Failed to create resource managers");
            return false;
        }
        
        if (!LoadResources())
        {
            Logger::Log(LogLevel::ERROR, "Failed to load resources");
            return false;
        }
        
        if (!CreateSkybox())
        {
            Logger::Log(LogLevel::WARNING, "Failed to create skybox");
        }
        
        if (!CreateRenderer())
        {
            Logger::Log(LogLevel::ERROR, "Failed to create renderer");
            return false;
        }
        
        if (!InitializeRenderer())
        {
            Logger::Log(LogLevel::ERROR, "Failed to initialize renderer");
            return false;
        }
        
        if (!CreateRenderSystems())
        {
            Logger::Log(LogLevel::WARNING, "Failed to create render systems");
        }
        
        initialized = true;
        Logger::Log(LogLevel::INFO, "RenderingModule initialized successfully");
        Logger::Log(LogLevel::INFO, "======================================");
        
        return true;
    }
    
    /**
     * @brief Update module (called every frame)
     * @param deltaTime Time since last frame
     */
    void Update(float deltaTime) override
    {
    }
    
    /**
     * @brief Shutdown module
     */
    void Shutdown() override
    {
        Logger::Log(LogLevel::INFO, "Shutting down RenderingModule...");
        
        if (renderer)
        {
            renderer->Shutdown();
            renderer.reset();
        }
        
        if (shaderManager)
        {
            shaderManager->ClearAll();
            shaderManager.reset();
        }
        
        iconRenderSystem.reset();
        skybox.reset();
        modelManager.reset();
        materialManager.reset();
        textureManager.reset();
        
        initialized = false;
        Logger::Log(LogLevel::INFO, "RenderingModule shutdown complete");
    }
    
    /// @name IModule interface
    /// @{
    const char* GetName() const override { return "Rendering"; }
    int GetPriority() const override { return 50; }
    bool IsRequired() const override { return false; } 
    /// @}
    
    /// @name Getters
    /// @{
    Renderer* GetRenderer() { return renderer.get(); }
    ShaderManager* GetShaderManager() { return shaderManager.get(); }
    TextureManager* GetTextureManager() { return textureManager.get(); }
    MaterialManager* GetMaterialManager() { return materialManager.get(); }
    ModelManager* GetModelManager() { return modelManager.get(); }
    Skybox* GetSkybox() { return skybox.get(); }
    
    bool IsInitialized() const { return initialized; }
    bool HasOpenGL() const { return hasOpenGL; }
    /// @}

private:
    /**
     * @brief Initialize OpenGL context
     * @return true if OpenGL is available
     */
    bool InitializeOpenGL()
    {
        Logger::Log(LogLevel::INFO, "Checking OpenGL availability...");
        
        if (!window)
        {
            Logger::Log(LogLevel::ERROR, "No window available for OpenGL context");
            return false;
        }
        
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Logger::Log(LogLevel::WARNING, "Failed to load OpenGL functions");
            return false;
        }
        
        const char* vendor = (const char*)glGetString(GL_VENDOR);
        const char* rendererName = (const char*)glGetString(GL_RENDERER);
        const char* version = (const char*)glGetString(GL_VERSION);
        
        Logger::Log(LogLevel::INFO, "OpenGL Vendor: " + std::string(vendor ? vendor : "Unknown"));
        Logger::Log(LogLevel::INFO, "OpenGL Renderer: " + std::string(rendererName ? rendererName : "Unknown"));
        Logger::Log(LogLevel::INFO, "OpenGL Version: " + std::string(version ? version : "Unknown"));
        
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        
        return true;
    }
    
    /**
     * @brief Create resource managers
     * @return true if successful
     */
    bool CreateResourceManagers()
    {
        Logger::Log(LogLevel::INFO, "Creating resource managers...");
        
        try
        {
            textureManager = std::make_unique<TextureManager>();
            Logger::Log(LogLevel::DEBUG, "TextureManager created");
            
            materialManager = std::make_unique<MaterialManager>(textureManager.get());
            Logger::Log(LogLevel::DEBUG, "MaterialManager created");
            
            shaderManager = std::make_unique<ShaderManager>();
            Logger::Log(LogLevel::DEBUG, "ShaderManager created");
            Logger::Log(LogLevel::DEBUG, "ShaderManager address: " + 
                std::to_string(reinterpret_cast<uintptr_t>(shaderManager.get())));
            
            modelManager = std::make_unique<ModelManager>();
            modelManager->SetMaterialManager(materialManager.get());
            Logger::Log(LogLevel::DEBUG, "ModelManager created");
            
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception creating resource managers: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Load shaders and materials
     * @return true if successful
     */
    bool LoadResources()
    {
        Logger::Log(LogLevel::INFO, "Loading resources...");
        
        try
        {
            shaderManager->Load();
            Logger::Log(LogLevel::INFO, "Shaders loaded successfully");
            
            materialManager->LoadMaterialConfigs();
            Logger::Log(LogLevel::INFO, "Materials loaded successfully");
            
            shaderManager->Bind("skybox");
            shaderManager->SetInt("skybox", "skybox", 0);
            shaderManager->Unbind();
            
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception loading resources: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Create Skybox
     * @return true if successful
     */
    bool CreateSkybox()
    {
        Logger::Log(LogLevel::INFO, "Creating skybox...");
        
        try
        {
            std::vector<std::string> faces = 
            {
                "assets/textures/skybox1/right.png",
                "assets/textures/skybox1/left.png",
                "assets/textures/skybox1/top.png",
                "assets/textures/skybox1/bottom.png",
                "assets/textures/skybox1/front.png",
                "assets/textures/skybox1/back.png"
            };
            
            unsigned int cubemapTexture = textureManager->LoadCubemap(faces);
            
            if (cubemapTexture == 0)
            {
                Logger::Log(LogLevel::ERROR, "Failed to load cubemap texture");
                return false;
            }
            
            skybox = std::make_unique<Skybox>(cubemapTexture, "skybox");
            
            Logger::Log(LogLevel::INFO, "Skybox created successfully");
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception creating skybox: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Create Renderer instance
     * @return true if successful
     */
    bool CreateRenderer()
    {
        Logger::Log(LogLevel::INFO, "Creating renderer...");
        
        if (!ecsWorld)
        {
            Logger::Log(LogLevel::ERROR, "ECSWorld is null, cannot create renderer");
            return false;
        }
        
        try
        {
            renderer = std::make_unique<Renderer>(shaderManager.get(), ecsWorld);
            
            Logger::Log(LogLevel::INFO, "Renderer created successfully");
            Logger::Log(LogLevel::DEBUG, "  - ShaderManager: " + 
                std::to_string(reinterpret_cast<uintptr_t>(shaderManager.get())));
            Logger::Log(LogLevel::DEBUG, "  - ECSWorld: " + 
                std::to_string(reinterpret_cast<uintptr_t>(ecsWorld)));
            
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception creating renderer: " + std::string(e.what()));
            return false;
        }
    }
    
    /**
     * @brief Initialize Renderer with skybox data
     * @return true if successful
     */
    bool InitializeRenderer()
    {
        Logger::Log(LogLevel::INFO, "Initializing renderer...");
        
        if (!renderer)
        {
            Logger::Log(LogLevel::ERROR, "Renderer is null");
            return false;
        }
        
        if (!skybox)
        {
            Logger::Log(LogLevel::WARNING, "Skybox is null, using fallback");
        }
        
        GLuint skyboxVAO = skybox ? skybox->GetVAO() : 0;
        GLuint skyboxTexture = skybox ? skybox->GetTexture() : 0;
        
        if (!renderer->Initialize(skyboxVAO, skyboxTexture))
        {
            Logger::Log(LogLevel::ERROR, "Renderer initialization failed");
            return false;
        }
        
        Logger::Log(LogLevel::INFO, "Renderer initialized successfully");
        return true;
    }
    
    /**
     * @brief Create rendering systems (IconRenderSystem, etc.)
     * @return true if successful
     */
    bool CreateRenderSystems()
    {
        Logger::Log(LogLevel::INFO, "Creating render systems...");
        
        try
        {
            iconRenderSystem = std::make_unique<IconRenderSystem>(textureManager.get());
            renderer->SetIconRenderSystem(std::move(iconRenderSystem));
            
            Logger::Log(LogLevel::INFO, "IconRenderSystem created");
            
            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception creating render systems: " + std::string(e.what()));
            return false;
        }
    }
};