#pragma once

#include <angelscript.h>
#include <entt/entt.hpp>

#include <string>
#include <variant>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

#include "core/CommandManager.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

void DispatchTrigger(ECSWorld *ecs, const std::string &fnDecl,
                     entt::entity a, entt::entity b) {
    auto tryCall = [&](entt::entity self, entt::entity other) {
        if (!ecs->HasComponent<ScriptComponent>(self)) return;
        auto &script = ecs->GetComponent<ScriptComponent>(self);
        if (!script.loaded || !script.ctx || !script.module) return;

        asIScriptFunction *fn = script.module->GetFunctionByDecl(fnDecl.c_str());
        if (!fn) return;

        script.ctx->Prepare(fn);
        script.ctx->SetArgQWord(0, static_cast<asQWORD>(other));
        script.ctx->Execute();
    };

    tryCall(a, b);
    tryCall(b, a);
}

void RegisterCommands(ECSWorld *ecs) {
    CommandManager::RegisterCommand("OnTriggerEnter",
                                    [ecs](const CommandArgs &args) {
                                        entt::entity a = std::get<entt::entity>(args[0]);
                                        entt::entity b = std::get<entt::entity>(args[1]);
                                        DispatchTrigger(ecs, "void OnTriggerEnter(uint64)", a, b);
                                    });

    CommandManager::RegisterCommand("OnTriggerExit",
                                    [ecs](const CommandArgs &args) {
                                        entt::entity a = std::get<entt::entity>(args[0]);
                                        entt::entity b = std::get<entt::entity>(args[1]);
                                        DispatchTrigger(ecs, "void OnTriggerExit(uint64)", a, b);
                                    });
}