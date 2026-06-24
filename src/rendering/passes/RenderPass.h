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
    RenderPass(const std::string &n, GLContext *ctx, ShaderManager *sm);

    virtual ~RenderPass();

    virtual void Setup() = 0;

    virtual void Execute(const glm::mat4 &view, const glm::mat4 &projection) = 0;

    virtual void Cleanup() = 0;

    void SetEnabled(bool e);

    bool IsEnabled() const;

    const std::string &GetName() const;
};