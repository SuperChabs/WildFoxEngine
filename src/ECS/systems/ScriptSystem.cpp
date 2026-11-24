#include "ScriptSystem.h"

#include <string>
#include <filesystem>

#include "core/logging/Logger.h"
#include "scripting/ASState.h"

void ScriptSystem::Start(ECSWorld &ecs) {
    ecs.Each<c_Script>([&](entt::entity, c_Script &script) {
        if (script.failed || !script.active || !script.loaded)
            return;

        CallFunction(script, script.fnOnStart);
    });
}

void ScriptSystem::Update(ECSWorld &ecs, const float deltaTime) {
    ecs.Each<c_Script>([&](const entt::entity e, c_Script &script) {
        if (script.failed || !script.active)
            return;

        if (!script.loaded) {
            LoadScript(e, script);
            return;
        }

        if (!script.fnOnUpdate || !script.ctx)
            return;

        script.ctx->Prepare(script.fnOnUpdate);
        script.ctx->SetArgFloat(0, deltaTime);

        if (script.ctx->Execute() == asEXECUTION_EXCEPTION) {
            Logger::Log(LogLevel::ERROR,
                        "Script exception in OnUpdate: " +
                        std::string(script.ctx->GetExceptionString()));
            script.failed = true;
        }
    });
}

void ScriptSystem::FixedUpdate(ECSWorld &ecs, float deltaTime) {
    ecs.Each<c_Script>([&](const entt::entity e, c_Script &script) {
        if (script.failed || !script.active)
            return;

        if (!script.loaded) {
            LoadScript(e, script);
            return;
        }

        if (!script.fnOnFixedUpdate || !script.ctx)
            return;

        script.ctx->Prepare(script.fnOnFixedUpdate);
        script.ctx->SetArgFloat(0, deltaTime);

        if (script.ctx->Execute() == asEXECUTION_EXCEPTION) {
            Logger::Log(LogLevel::ERROR,
                        "Script exception in OnFixedUpdate: " +
                        std::string(script.ctx->GetExceptionString()));
            script.failed = true;
        }
    });
}

void ScriptSystem::Stop(ECSWorld &ecs) {
    ecs.Each<c_Script>([&](entt::entity e, c_Script &script) {
        if (script.failed || !script.loaded)
            return;

        CallFunction(script, script.fnOnStop);
    });
}

void ScriptSystem::Destroy(ECSWorld &ecs) {
    ecs.Each<c_Script>([&](entt::entity e, c_Script &script) {
        if (script.failed || !script.loaded)
            return;

        CallFunction(script, script.fnOnDestroy);
    });
}

void ScriptSystem::LoadScript(entt::entity e, c_Script &script) {
    if (!std::filesystem::exists(script.scriptPath)) {
        Logger::Log(LogLevel::ERROR, "Script not found: " + script.scriptPath);
        script.failed = true;
        return;
    }

    asIScriptEngine *engine = ASState::Get();

    CScriptBuilder builder;
    std::string moduleName = script.scriptPath;

    if (builder.StartNewModule(engine, moduleName.c_str()) < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to start module: " + moduleName);
        script.failed = true;
        return;
    }

    if (builder.AddSectionFromFile(script.scriptPath.c_str()) < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to add script: " + script.scriptPath);
        script.failed = true;
        return;
    }

    if (builder.BuildModule() < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to build script: " + script.scriptPath);
        script.failed = true;
        return;
    }

    script.module = engine->GetModule(moduleName.c_str());

    script.fnOnStart       = script.module->GetFunctionByDecl("void OnStart()");
    script.fnOnUpdate      = script.module->GetFunctionByDecl("void OnUpdate(float)");
    script.fnOnFixedUpdate = script.module->GetFunctionByDecl("void OnFixedUpdate(float)");
    script.fnOnStop        = script.module->GetFunctionByDecl("void OnStop()");
    script.fnOnDestroy     = script.module->GetFunctionByDecl("void OnDestroy()");

    script.ctx = engine->CreateContext();

    if (asIScriptFunction *setEntity = script.module->GetFunctionByDecl("void _SetEntity(uint64)")) {
        script.ctx->Prepare(setEntity);
        script.ctx->SetArgQWord(0, static_cast<asQWORD>(e));
        script.ctx->Execute();
    }

    script.loaded = true;
    Logger::Log(LogLevel::INFO, "Script loaded: " + script.scriptPath);

    CallFunction(script, script.fnOnStart);
}

void ScriptSystem::CallFunction(c_Script &script, asIScriptFunction *fn) {
    if (!fn || !script.ctx)
        return;

    script.ctx->Prepare(fn);

    if (script.ctx->Execute() == asEXECUTION_EXCEPTION) {
        Logger::Log(LogLevel::ERROR,
                    "Script exception in " + std::string(fn->GetName()) + ": " +
                    std::string(script.ctx->GetExceptionString()));
        script.failed = true;
    }
}