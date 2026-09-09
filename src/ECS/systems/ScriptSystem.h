#pragma once

#include <angelscript.h>
#include <entt/entt.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"


class ScriptSystem {
public:
    static void Update(ECSWorld &ecs, float deltaTime);

    static void Start(ECSWorld &ecs);

    static void Stop(ECSWorld &ecs);

private:
    static void LoadScript(entt::entity e, ScriptComponent &script);

    static void CallUpdate(ScriptComponent &script, float deltaTime);

    static void CallFunction(ScriptComponent &script, asIScriptFunction *fn);
};