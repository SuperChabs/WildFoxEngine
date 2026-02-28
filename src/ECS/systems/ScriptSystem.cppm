module;

#include <angelscript.h>
#include <scriptbuilder.h>
#include <entt/entt.hpp>
#include <filesystem>

export module WFE.ECS.Systems.ScriptSystem;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.ECS.Components.Script;
import WFE.Core.Logger;
import WFE.Core.Input;
import WFE.Scripting.ASState;

export class ScriptSystem
{
public:
    void Update(ECSWorld& ecs, Input* input, float deltaTime)
    {
        ecs.Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script)
        {
            if (script.failed || !script.active)
                return;

            if (!script.loaded)
            {
                LoadScript(e, script);
                return;
            }

            CallUpdate(script, deltaTime);
        });
    }

    void Start(ECSWorld& ecs)
    {
        ecs.Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script)
        {
            if (script.failed || !script.active || !script.loaded)
                return;

            CallFunction(script, script.fnOnStart);
        });
    }

    void Stop(ECSWorld& ecs)
    {
        ecs.Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script)
        {
            if (script.failed || !script.loaded)
                return;

            CallFunction(script, script.fnOnStop);
        });
    }

private:
    void LoadScript(entt::entity e, ScriptComponent& script)
    {
        if (!std::filesystem::exists(script.scriptPath))
        {
            Logger::Log(LogLevel::ERROR, "Script not found: " + script.scriptPath);
            script.failed = true;
            return;
        }

        asIScriptEngine* engine = ASState::Get();

        CScriptBuilder builder;
        std::string moduleName = script.scriptPath;

        if (builder.StartNewModule(engine, moduleName.c_str()) < 0)
        {
            Logger::Log(LogLevel::ERROR, "Failed to start module: " + moduleName);
            script.failed = true;
            return;
        }

        if (builder.AddSectionFromFile(script.scriptPath.c_str()) < 0)
        {
            Logger::Log(LogLevel::ERROR, "Failed to add script: " + script.scriptPath);
            script.failed = true;
            return;
        }

        if (builder.BuildModule() < 0)
        {
            Logger::Log(LogLevel::ERROR, "Failed to build script: " + script.scriptPath);
            script.failed = true;
            return;
        }

        script.module = engine->GetModule(moduleName.c_str());

        script.fnOnStart  = script.module->GetFunctionByDecl("void OnStart()");
        script.fnOnUpdate = script.module->GetFunctionByDecl("void OnUpdate(float)");
        script.fnOnStop   = script.module->GetFunctionByDecl("void OnStop()");

        script.ctx = engine->CreateContext();

        asIScriptFunction* setEntity = script.module->GetFunctionByDecl("void _SetEntity(uint64)");
        if (setEntity)
        {
            script.ctx->Prepare(setEntity);
            script.ctx->SetArgQWord(0, static_cast<asQWORD>(e));
            script.ctx->Execute();
        }

        script.loaded = true;
        Logger::Log(LogLevel::INFO, "Script loaded: " + script.scriptPath);

        CallFunction(script, script.fnOnStart);
    }

    void CallUpdate(ScriptComponent& script, float deltaTime)
    {
        if (!script.fnOnUpdate || !script.ctx)
            return;

        script.ctx->Prepare(script.fnOnUpdate);
        script.ctx->SetArgFloat(0, deltaTime);

        if (script.ctx->Execute() == asEXECUTION_EXCEPTION)
        {
            Logger::Log(LogLevel::ERROR,
                "Script exception in OnUpdate: " +
                std::string(script.ctx->GetExceptionString()));
            script.failed = true;
        }
    }

    void CallFunction(ScriptComponent& script, asIScriptFunction* fn)
    {
        if (!fn || !script.ctx)
            return;

        script.ctx->Prepare(fn);

        if (script.ctx->Execute() == asEXECUTION_EXCEPTION)
        {
            Logger::Log(LogLevel::ERROR,
                "Script exception in " + std::string(fn->GetName()) + ": " +
                std::string(script.ctx->GetExceptionString()));
            script.failed = true;
        }
    }
};