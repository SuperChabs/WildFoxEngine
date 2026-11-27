#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"

class UIPass : public RenderPass {
public:
    UIPass( ShaderManager *sm);

    void Setup() override;
    void Execute(FrameContext &ctx) override;
    void Cleanup() override;
};