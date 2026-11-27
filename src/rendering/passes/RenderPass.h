#pragma once

#include <string>

#include "resource/shader/ShaderManager.h"
#include "PassContext.h"

class RenderPass {
protected:
    std::string m_name;
    ShaderManager *m_shaderManager;

    bool m_enabled = true;

public:
    RenderPass(const std::string &n, ShaderManager *sm) : m_name(n), m_shaderManager(sm) {}

    virtual ~RenderPass() = default;

    virtual void Setup() = 0;
    virtual void Execute(FrameContext &ctx) = 0;
    virtual void Cleanup() = 0;

    static void BindTarget(GLuint fbo, int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
    }

    void SetEnabled(bool e) { m_enabled = e; }

    [[nodiscard]] bool IsEnabled() const { return m_enabled; }
    [[nodiscard]] const std::string &GetName() const { return m_name; }
};