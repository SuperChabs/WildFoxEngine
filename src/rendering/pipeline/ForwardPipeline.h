#pragma once

#include <glad/glad.h>

#include "rendering/pipeline/RenderPipeline.h"
#include "rendering/passes/GeometryPass.h"
#include "rendering/passes/ShadowPass.h"
#include "ECS/World.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"
#include "ECS/components/Components.h"

class ForwardPipeline : public RenderPipeline {
    ECSWorld *m_world;
    GLuint m_skyboxVAO;
    GLuint m_cubeMapTexture;

public:
    ForwardPipeline(GLContext *ctx, ShaderManager *sm, ECSWorld *w, GLuint skyVAO, GLuint cubeMap);
    void Initialize() override;
};