#pragma once

#include <string>

#include <glm/glm.hpp>

#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"

class RenderPass {
protected:
    std::string name;
    GLContext *context;
    ShaderManager *shaderManager;

    bool enabled = true;

public:
    RenderPass(const std::string &n, GLContext *ctx, ShaderManager *sm)
        : name(n), context(ctx), shaderManager(sm) {
    }

    virtual ~RenderPass() = default;

    virtual void Setup() {
    }

    virtual void Execute(const glm::mat4 &view, const glm::mat4 &projection) = 0;

    virtual void Cleanup() {
    }

    void SetEnabled(bool e) { enabled = e; }
    bool IsEnabled() const { return enabled; }

    const std::string &GetName() const { return name; }
};