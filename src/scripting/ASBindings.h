#pragma once

#include "ECS/World.h"
#include "ECS/systems/AudioSystem.h"
#include "core/Input.h"
#include "scripting/ASState.h"
#include "scripting/ASRegistration/ASRegisterAPI.h"

void InitAS(ECSWorld *ecs, Input *input, AudioSystem *audioSystem) {
    ASState::Get();
    ASRegisterAPI::RegisterAll(ecs, input, audioSystem);
}

void ShutdownAS() {
    ASState::Shutdown();
}