#pragma once

#include <angelscript.h>
#include <entt/entt.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"


class ScriptSystem {
public:
    static void Start(ECSWorld &ecs);
    static void Update(ECSWorld &ecs, float deltaTime);
    static void FixedUpdate(ECSWorld &ecs, float deltaTime);
    static void Stop(ECSWorld &ecs);
    static void Destroy(ECSWorld &ecs);

private:
    static void LoadScript(entt::entity e, c_Script &script);
    static void CallFunction(c_Script &script, asIScriptFunction *fn);
};