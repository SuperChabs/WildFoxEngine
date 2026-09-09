#pragma once

#include <string>
#include <memory>
#include <vector>

#include <entt/entt.hpp>
#include <glad/glad.h>

#include "rendering/pipeline/RenderPipeline.h"
#include "rendering/passes/GeometryPass.h"
#include "rendering/passes/ShadowPass.h"
#include "ECS/World.h"
#include "rendering/core/GLContext.h"
#include "resource/shader/ShaderManager.h"
#include "ECS/components/Components.h"

class ForwardPipeline : public RenderPipeline {
    ECSWorld *world;
    GLuint skyboxVAO;
    GLuint cubemapTexture;

    ShadowPass *m_ShadowPassPtr = nullptr;
    GeometryPass *m_GeometryPassPtr = nullptr;

public:
    ForwardPipeline(GLContext *ctx, ShaderManager *sm, ECSWorld *w,
                    GLuint skyVAO, GLuint cubemap);

    void Initialize() override;

    void Execute(CameraComponent &camera, TransformComponent &transform,
                 CameraOrientationComponent &orientation, int width, int height) override;

    ShadowPass *GetShadowPass() const;
};