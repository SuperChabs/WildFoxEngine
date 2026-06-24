#include "RenderPass.h"

RenderPass::RenderPass(const std::string &n, GLContext *ctx, ShaderManager *sm)
    : name(n), context(ctx), shaderManager(sm) {
}

RenderPass::~RenderPass() = default;

void RenderPass::SetEnabled(bool e) {
    enabled = e;
}

bool RenderPass::IsEnabled() const {
    return enabled;
}

const std::string &RenderPass::GetName() const {
    return name;
}