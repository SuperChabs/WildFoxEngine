#include "MainScript.h"

#include <filesystem>
#include <scriptbuilder.h>

#include "core/logging/Logger.h"
#include "scripting/ASState.h"

MainScript::MainScript(const std::string &scriptPath)
    : m_scriptPath(scriptPath) {}

void MainScript::Update(const float deltaTime) {
    if (m_failed || !m_loaded)
        return;

    CallUpdate(deltaTime);
}

void MainScript::Start() {
    if (m_failed)
        return;

    if (!m_loaded) {
        LoadScript();
        return;
    }

    CallFunction(m_fnOnStart);
}

void MainScript::Stop() {
    if (m_failed || !m_loaded)
        return;

    CallFunction(m_fnOnStop);
}

void MainScript::LoadScript() {
    if (!std::filesystem::exists(m_scriptPath)) {
        Logger::Log(LogLevel::ERROR, "Main script not found: " + m_scriptPath);
        m_failed = true;
        return;
    }

    asIScriptEngine *engine = ASState::Get();

    CScriptBuilder builder;
    const std::string moduleName = m_scriptPath;

    if (builder.StartNewModule(engine, moduleName.c_str()) < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to start module: " + moduleName);
        m_failed = true;
        return;
    }

    if (builder.AddSectionFromFile(m_scriptPath.c_str()) < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to add main script: " + m_scriptPath);
        m_failed = true;
        return;
    }

    if (builder.BuildModule() < 0) {
        Logger::Log(LogLevel::ERROR, "Failed to build main script: " + m_scriptPath);
        m_failed = true;
        return;
    }

    m_module = engine->GetModule(moduleName.c_str());

    m_fnOnStart = m_module->GetFunctionByDecl("void OnStart()");
    m_fnOnUpdate = m_module->GetFunctionByDecl("void OnUpdate(float)");
    m_fnOnStop = m_module->GetFunctionByDecl("void OnStop()");

    m_ctx = engine->CreateContext();

    m_loaded = true;
    Logger::Log(LogLevel::INFO, "Script loaded: " + m_scriptPath);

    CallFunction(m_fnOnStart);
}

void MainScript::CallUpdate(const float deltaTime) {
    if (!m_fnOnUpdate || !m_ctx)
        return;

    m_ctx->Prepare(m_fnOnUpdate);
    m_ctx->SetArgFloat(0, deltaTime);

    if (m_ctx->Execute() == asEXECUTION_EXCEPTION) {
        Logger::Log(LogLevel::ERROR, "Main script exception in OnUpdate: " + std::string(m_ctx->GetExceptionString()));
        m_failed = true;
    }
}

void MainScript::CallFunction(asIScriptFunction *fn) {
    if (!fn || !m_ctx)
        return;

    m_ctx->Prepare(fn);

    if (m_ctx->Execute() == asEXECUTION_EXCEPTION) {
        Logger::Log(LogLevel::ERROR, "Main script exception in " + std::string(fn->GetName()) + ": " +
                    std::string(m_ctx->GetExceptionString()));
        m_failed = true;
    }
}
