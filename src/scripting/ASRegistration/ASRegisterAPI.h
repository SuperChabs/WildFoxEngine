#pragma once

#include <angelscript.h>
#include <scriptbuilder.h>
#include <scriptstdstring.h>

#include "ECS/World.h"
#include "core/Input.h"


#include "ECS/systems/AudioSystem.h"

class ASRegisterAPI {
public:
    static void RegisterAll(ECSWorld *ecs, Input *input, AudioSystem *audioSystem);
};