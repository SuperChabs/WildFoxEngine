#pragma once

#include <angelscript.h>
#include <entt/entt.hpp>

#include <string>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

#include "core/logging/Logger.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "EntityDefinition.h"

inline c_Transform *GetTransform(ECSWorld *ecs, entt::entity e) {
    if (!ecs->HasComponent<c_Transform>(e))
        return nullptr;
    return &ecs->GetComponent<c_Transform>(e);
}

inline bool IsValid(ECSWorld *ecs, entt::entity e) {
    return ecs->IsValid(e);
}

inline void DestroyEntity(ECSWorld *ecs, entt::entity e) {
    ecs->DestroyEntity(e);
}

inline entt::entity GetEntityByName(ECSWorld *ecs, const std::string &name) {
    entt::entity result = entt::null;
    ecs->Each<c_Tag>([&](entt::entity e, c_Tag &tag) {
        if (tag.name == name)
            result = e;
    });
    return result;
}

inline void CreateEntity(ECSWorld &ecs, const EntityDefinition &def) {
    auto entity = ecs.CreateEntity(def.name);

    for (const auto &comp: def.components)
        std::visit(
            [&](const auto &c) {
                using T = std::decay_t<decltype(c)>;
                ecs.AddComponent<T>(entity, c);
            },
            comp
        );
}

inline void RegisterECS(asIScriptEngine *engine, ECSWorld *ecs) {
    int r;

    r = engine->RegisterGlobalFunction(
        "void DestroyEntity(uint64 e)",
        asFUNCTIONPR(DestroyEntity, (ECSWorld *, entt::entity), void),
        asCALL_CDECL_OBJFIRST, ecs
    );
    AS_CHECK(r, "DestroyEntity");

    r = engine->RegisterGlobalFunction(
        "Transform@ GetTransform(uint64 e)",
        asFUNCTIONPR(GetTransform, (ECSWorld *, entt::entity), c_Transform *),
        asCALL_CDECL_OBJFIRST, ecs
    );
    AS_CHECK(r, "GetTransform");

    r = engine->RegisterGlobalFunction(
                "uint64 GetEntityByName(const string &in name)",
                asFUNCTIONPR(GetEntityByName, (ECSWorld *, const std::string &), entt::entity),
            asCALL_CDECL_OBJFIRST, ecs
    );
    AS_CHECK(r, "GetEntityByName");

    r = engine->RegisterGlobalFunction(
        "bool IsValidEntity(uint64 e)",
        asFUNCTIONPR(IsValid, (ECSWorld *, entt::entity), bool),
        asCALL_CDECL_OBJFIRST, ecs
    );
    AS_CHECK(r, "IsValidEntity");

    // TODO: fix
    // r = engine->RegisterGlobalFunction(
    //     "void CreateEntity(const EntityDefinition &in)",
    //     asFUNCTIONPR(CreateEntity, (ECSWorld &, const EntityDefinition &), void),
    //     asCALL_CDECL_OBJFIRST, ecs
    // );
    // AS_CHECK(r, "CreateEntity");
}