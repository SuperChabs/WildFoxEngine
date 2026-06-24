#pragma once

#include <memory>

#include "core/IModule.h"
#include "resource/shader/ShaderManager.h"
#include "resource/texture/TextureManager.h"
#include "resource/material/MaterialManager.h"
#include "resource/model/ModelManager.h"

class ResourceModule : public IModule {
    std::unique_ptr<ModelManager> modelManager;
    std::unique_ptr<ShaderManager> shaderManager;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<MaterialManager> materialManager;

public:
    ResourceModule() = default;

    bool Initialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    /// @name IModule interface
    /// @{
    const char *GetName() const override;

    int GetPriority() const override;

    bool IsRequired() const override;

    /// @}

    ModelManager *GetModelManager();

    ShaderManager *GetShaderManager();

    TextureManager *GetTextureManager();

    MaterialManager *GetMaterialManager();
};