#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "core/CommandManager.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"

class UIPass : public RenderPass {
public:
    UIPass(GLContext *ctx, ShaderManager *sm)
        : RenderPass("UIPass", ctx, sm) {
    }

    void Setup() override {
        context->SetDepthTest(true);
        context->SetDepthFunc(GL_LESS);
        glDepthMask(GL_FALSE);
        context->SetBlend(true);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Execute(const glm::mat4 &view, const glm::mat4 &projection) override {
        if (!enabled) return;

        Setup();

        CommandManager::ExecuteCommand("Renderer_RenderUI",
                                       {
                                           view,
                                           projection,
                                           std::string("icon")
                                       });

        Cleanup();
    }

    void Cleanup() override {
        glDepthMask(GL_TRUE);
        context->SetDepthTest(true);
        context->SetBlend(false);
    }
};