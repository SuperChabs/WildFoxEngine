#pragma once

#include "physics/core/PhysicsContext.h"
#include "physics/pipeline/StageStruct.h"

class IPhysicsStage {
    bool m_active = true;

public:
    virtual ~IPhysicsStage() = default;

    virtual void Execute(StepContext &ctx) = 0;

    [[nodiscard]] virtual std::string GetName() = 0;

    [[nodiscard]] bool GetActive() const { return m_active; }
    void SetActive(const bool active) { m_active = active; }
};
