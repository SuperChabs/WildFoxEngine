#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rendering/passes/RenderPass.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"

class SkyboxPass : public RenderPass {
private:
    GLuint skyboxVAO = 0;
    GLuint cubemapTexture = 0;

public:
    SkyboxPass(GLContext *ctx, ShaderManager *sm, GLuint vao, GLuint cubemap);


    void Setup() override;


    void Execute(const glm::mat4 &view, const glm::mat4 &projection) override;


    void Cleanup() override;
};
