#include "ResourceModule.h"
#include <string>
#include "core/logging/Logger.h"

bool ResourceModule::Initialize() {
    Logger::Log(LogLevel::INFO, "Creating resource managers...");

    try {
        textureManager = std::make_unique<TextureManager>();
        Logger::Log(LogLevel::DEBUG, "  TextureManager created");

        materialManager = std::make_unique<MaterialManager>(textureManager.get());
        Logger::Log(LogLevel::DEBUG, "  MaterialManager created");

        shaderManager = std::make_unique<ShaderManager>();
        Logger::Log(LogLevel::DEBUG, "  ShaderManager created");
        Logger::Log(LogLevel::DEBUG, "  ShaderManager address: " +
                                     std::to_string(reinterpret_cast<uintptr_t>(shaderManager.get())));

        modelManager = std::make_unique<ModelManager>();
        modelManager->SetMaterialManager(materialManager.get());
        Logger::Log(LogLevel::DEBUG, "  ModelManager created");

        isInitialized = true;

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception creating resource managers: " + std::string(e.what()));

        isInitialized = false;

        return false;
    }
}

void ResourceModule::Update(float deltaTime) {
}

void ResourceModule::Shutdown() {
    Logger::Log(LogLevel::INFO, "Shutting down ResourceModule...");

    if (shaderManager) {
        shaderManager->ClearAll();
        shaderManager.reset();
    }

    modelManager.reset();
    materialManager.reset();
    textureManager.reset();

    isInitialized = false;
    Logger::Log(LogLevel::INFO, "RenderingModule shutdown complete");
}

const char *ResourceModule::GetName() const {
    return "Resource";
}

int ResourceModule::GetPriority() const {
    return 20;
}

bool ResourceModule::IsRequired() const {
    return true;
}

ModelManager *ResourceModule::GetModelManager() {
    return modelManager.get();
}

ShaderManager *ResourceModule::GetShaderManager() {
    return shaderManager.get();
}

TextureManager *ResourceModule::GetTextureManager() {
    return textureManager.get();
}

MaterialManager *ResourceModule::GetMaterialManager() {
    return materialManager.get();
}