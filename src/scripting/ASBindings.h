#pragma once

#include "ECS/World.h"
#include "ECS/systems/AudioSystem.h"
#include "core/Input.h"
#include "scripting/ASState.h"
#include "scripting/ASRegistration/ASRegisterAPI.h"

inline void InitAS(ECSWorld *ecs, Input *input, AudioSystem *audioSystem) {
    ASState::Get();
    ASRegisterAPI::RegisterAll(ecs, input, audioSystem);
}

inline void ShutdownAS() {
    ASState::Shutdown();
}