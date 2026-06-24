#pragma once

#include <angelscript.h>

#include <scriptbuilder.h>
#include <entt/entt.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "core/Input.h"

class ScriptSystem {
public:
    void Update(ECSWorld &ecs, Input *input, float deltaTime);

    void Start(ECSWorld &ecs);

    void Stop(ECSWorld &ecs);

private:
    void LoadScript(entt::entity e, ScriptComponent &script);

    void CallUpdate(ScriptComponent &script, float deltaTime);

    void CallFunction(ScriptComponent &script, asIScriptFunction *fn);
};