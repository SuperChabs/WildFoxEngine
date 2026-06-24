#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"

class UIPass : public RenderPass {
public:
    UIPass(GLContext *ctx, ShaderManager *sm);

    void Setup() override;

    void Execute(const glm::mat4 &view, const glm::mat4 &projection) override;

    void Cleanup() override;
};