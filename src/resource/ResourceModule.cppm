module;

#include <memory>

export module WFE.Resource.ResourceModule;

import WFE.Core.IModule;
import WFE.Core.Logger;
import WFE.Resource.Shader.ShaderManager;
import WFE.Resource.Texture.TextureManager;
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Model.ModelManager;

export class ResourceModule : public IModule
{
    std::unique_ptr<ModelManager> modelManager;
    std::unique_ptr<ShaderManager> shaderManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<MaterialManager> materialManager;

public:
    ResourceModule() = default;

    bool Initialize() override
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
            
            isInitialized = true;

            return true;
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception creating resource managers: " + std::string(e.what()));

            isInitialized = false;

            return false;
        }
    }

    void Update(float deltaTime) override {}

    void Shutdown() override
    {
        Logger::Log(LogLevel::INFO, "Shutting down ResourceModule...");

        if (shaderManager)
        {
            shaderManager->ClearAll();
            shaderManager.reset();
        }

        modelManager.reset();
        materialManager.reset();
        textureManager.reset();

        isInitialized = false;
        Logger::Log(LogLevel::INFO, "RenderingModule shutdown complete");
    }

    /// @name IModule interface
    /// @{
    const char* GetName() const override { return "Resource"; }
    int GetPriority() const override { return 10; }
    bool IsRequired() const override { return true; } 
    /// @}

    ModelManager* GetModelManager() { return modelManager.get(); }
    ShaderManager* GetShaderManager() { return shaderManager.get(); }
    TextureManager* GetTextureManager() { return textureManager.get(); }
    MaterialManager* GetMaterialManager() { return materialManager.get(); }
};