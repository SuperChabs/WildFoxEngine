#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "ECS/World.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"

class GeometryPass : public RenderPass {
    ECSWorld *world;

    static constexpr int SHADOW_MAP_TEXTURE_SLOT = 6;
    static constexpr int CUBE_SHADOW_MAP_TEXTURE_SLOT = 4;

public:
    GeometryPass(ShaderManager *sm, ECSWorld *w);

    void Setup() override;

    void Execute(FrameContext &ctx) override;

    void Cleanup() override;

private:
    static void CleanupShadowBinding();
};