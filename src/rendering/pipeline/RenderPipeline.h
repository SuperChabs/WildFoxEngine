#pragma once

#include <string>
#include <memory>
#include <vector>

#include <entt/entt.hpp>

#include "rendering/passes/RenderPass.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

class RenderPipeline {
protected:
    std::string name;
    std::vector<std::unique_ptr<RenderPass> > passes;
    GLContext *context;
    ShaderManager *shaderManager;

public:
    RenderPipeline(const std::string &n, GLContext *ctx, ShaderManager *sm);

    virtual ~RenderPipeline() = default;

    virtual void Initialize() = 0;

    virtual void Execute(ECSWorld &ecs, entt::entity cameraEntity, int width, int height);

    void AddPass(std::unique_ptr<RenderPass> pass);

    RenderPass *GetPass(const std::string &passName);

    const std::string &GetName() const;

    size_t GetPassCount() const;
};