#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"

class SkyboxPass : public RenderPass {
    GLuint m_skyboxVAO = 0;
    GLuint m_cubeMapTexture = 0;

public:
    SkyboxPass(ShaderManager *sm, GLuint vao, GLuint cubeMap);

    void Setup() override;
    void Execute(FrameContext &ctx) override;
    void Cleanup() override;
};