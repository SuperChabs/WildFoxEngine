#pragma once

#include "IPhysicsStage.h"

class PhysicsPipeline {
    std::vector<std::unique_ptr<IPhysicsStage>> stages;

public:
    template <typename T, typename... Args>
    T &AddStage(Args &&... args) {
        auto stage = std::make_unique<T>(std::forward<Args>(args)...);
        T &ref = *stage;
        stages.push_back(std::move(stage));
        return ref;
    }

    void Step(StepContext &ctx) const;

    [[nodiscard]] const auto &GetStages() { return stages; }
};
