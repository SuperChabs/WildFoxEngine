#include "LevelScript.h"

#include <filesystem>
#include <scriptbuilder.h>

#include "core/CommandManager.h"
#include "core/logging/Logger.h"
#include "scripting/ASState.h"

LevelScript::LevelScript() {
    Logger::Log(LogLevel::INFO, "LevelScript Subscribing...");

    m_subPlayMode = GetEventBus().Subscribe("play_mode_started",
        [this](const std::any &payload) {
            Start(std::any_cast<std::string>(payload));
        });
    m_subEditMode = GetEventBus().Subscribe("play_mode_stopped",
        [this](const std::any &) {
            Stop();
        });
    m_subTrigger = GetEventBus().Subscribe("trigger_enter",
        [this](const std::any& payload) {
            auto pr = std::any_cast<std::pair<entt::entity, entt::entity>>(payload);
            CallTrigger(pr.first, pr.second);
        });
}

void LevelScript::Update(const float deltaTime) {
    if (m_failed || !m_loaded)
        return;

    CallUpdate(deltaTime);
}

void LevelScript::Start(const std::string &scriptPath) {
    if (m_failed)
        return;

    m_scriptPath = scriptPath;

    if (!m_loaded) {
        LoadScript();
        return;
    }

    CallFunction(m_fnOnStart);


}

void LevelScript::Stop() {
    if (m_failed || !m_loaded)
        return;

    m_loaded = false;

    CallFunction(m_fnOnStop);
}

void LevelScript::LoadScript() {
    const std::string moduleName = m_scriptPath + "/level.as";

    if (!std::filesystem::exists(moduleName)) {
        Logger::Log(LogLevel::ERROR, "Level script not found: " + moduleName);
        m_failed = true;
        return;
    }

    asIScriptEngine *engine = ASState::Get();
    CScriptBuilder builder;

    if (builder.StartNewModule(engine, moduleName.c_str()) < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to start module: " + moduleName);
        m_failed = true;
        return;
    }

    if (builder.AddSectionFromFile(moduleName.c_str()) < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to add main script: " + moduleName);
        m_failed = true;
        return;
    }

    if (builder.BuildModule() < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to build main script: " + moduleName);
        m_failed = true;
        return;
    }

    m_module = engine->GetModule(moduleName.c_str());

    m_fnOnStart = m_module->GetFunctionByDecl("void OnStart()");
    m_fnOnUpdate = m_module->GetFunctionByDecl("void OnUpdate(float)");
    m_fnOnStop = m_module->GetFunctionByDecl("void OnStop()");

    if (m_ctx) {
        m_ctx->Release();
        m_ctx = nullptr;
    }
    m_ctx = engine->CreateContext();

    m_loaded = true;
    Logger::Log(LogLevel::INFO, "Level Script loaded: " + moduleName);

    CallFunction(m_fnOnStart);
}

void LevelScript::CallUpdate(const float deltaTime) {
    if (!m_fnOnUpdate || !m_ctx)
        return;

    m_ctx->Prepare(m_fnOnUpdate);
    m_ctx->SetArgFloat(0, deltaTime);

    if (m_ctx->Execute() == asEXECUTION_EXCEPTION) {
        Logger::Log(LogLevel::ERROR, "Main script exception in OnUpdate: " + std::string(m_ctx->GetExceptionString()));
        m_failed = true;
    }
}

void LevelScript::CallFunction(asIScriptFunction *fn) {
    if (!fn || !m_ctx)
        return;

    m_ctx->Prepare(fn);

    if (m_ctx->Execute() == asEXECUTION_EXCEPTION) {
        Logger::Log(LogLevel::ERROR, "Main script exception in " + std::string(fn->GetName()) + ": " +
                    std::string(m_ctx->GetExceptionString()));
        m_failed = true;
    }
}

void LevelScript::CallTrigger(entt::entity a, entt::entity b) {
    if (m_failed || !m_loaded || !m_module)
        return;

    asIScriptFunction *fn = m_module->GetFunctionByDecl("void OnTriggerEnter(uint64, uint64)");
    if (!fn)
        return;

    m_ctx->Prepare(fn);
    m_ctx->SetArgQWord(0, static_cast<asQWORD>(a));
    m_ctx->SetArgQWord(1, static_cast<asQWORD>(b));
    m_ctx->Execute();
}
