#pragma once

#include <glm/glm.hpp>

#include "ECS/World.h"
#include "physics/pipeline/PhysicsPipeline.h"
#include "physics/pipeline/StageStruct.h"

class Physics {
    glm::vec3 m_gravity{0.0f, -9.81f, 0.0f};

    StepContext stepContext;

    ECSWorld& m_world;
    PhysicsPipeline m_pipeline;
public:
    explicit Physics(ECSWorld& world);

    void Simulate(float dt);

    [[nodiscard]] glm::vec3 GetGravity() const { return m_gravity; }
    void SetGravity(const glm::vec3& gravity) { m_gravity = gravity; }
};
