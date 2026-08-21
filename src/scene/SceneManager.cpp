#include "SceneManager.h"
#include <entt/entt.hpp>
#include <any>
#include "core/logging/Logger.h"
#include "core/CommandManager.h"
#include "core/EventBus.h"

SceneManager::SceneManager(ECSWorld *ecs)
    : m_ecs(ecs) {
    RegisterSceneCommands();
    RegisterDebugEvents();
}

void SceneManager::StartPlayMode() {
    if (m_IsPlayMode)
        return;

    if (!m_ecs)
    {
        Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
        return;
    }

    m_ecs->Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script)
    {
        script.active = true;
        script.loaded = false;
        script.failed = false;
    });

    m_IsPlayMode = true;

    Logger::Log(LogLevel::INFO, "Publishing play_mode_started, path=" + m_currentScenePath);
    GetEventBus().Publish("play_mode_started", m_currentScenePath);

    Logger::Log(LogLevel::INFO, "SceneManager: Entered Play Mode");
}

void SceneManager::StopPlayMode() {
    if (!m_IsPlayMode)
        return;

    if (!m_ecs)
    {
        Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
        return;
    }

    m_ecs->Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script)
    {
        if (script.ctx)
        {
            script.ctx->Release();
            script.ctx = nullptr;
        }
        if (script.module)
        {
            script.module->GetEngine()->DiscardModule(script.module->GetName());
            script.module = nullptr;
        }
        script.fnOnStart  = nullptr;
        script.fnOnUpdate = nullptr;
        script.fnOnStop   = nullptr;

        script.active = false;
        script.loaded = false;
        script.failed = false;
    });

    m_IsPlayMode = false;

    GetEventBus().Publish("play_mode_stopped");

    Logger::Log(LogLevel::INFO, "SceneManager: Exited Play Mode");
}

void SceneManager::PauseScripts() {
    if (!m_ecs || m_IsDebugPaused) return;

    m_ecs->Each<ScriptComponent>([](entt::entity, ScriptComponent &script) {
        script.active = false;
    });

    m_IsDebugPaused = true;
    GetEventBus().Publish("debug_paused");
    Logger::Log(LogLevel::INFO, "SceneManager: Scripts PAUSED (debug)");
}

void SceneManager::ResumeScripts() {
    if (!m_ecs || !m_IsDebugPaused) return;

    m_ecs->Each<ScriptComponent>([](entt::entity, ScriptComponent &script) {
        if (!script.failed)
            script.active = true;
    });

    m_IsDebugPaused = false;
    GetEventBus().Publish("debug_resumed");
    Logger::Log(LogLevel::INFO, "SceneManager: Scripts RESUMED");
}

bool SceneManager::IsInPlayMode() {
    return m_IsPlayMode;
}

void SceneManager::RegisterSceneCommands() {
    CommandManager::RegisterCommand("onPlayGame",
                                    [this](const CommandArgs &) {
                                        if (m_IsPlayMode) {
                                            Logger::Log(LogLevel::WARNING, "Already in play mode");
                                            return;
                                        }

                                        Logger::Log(LogLevel::INFO, "=== ENTERING PLAY MODE ===");
                                        StartPlayMode();
                                    });

    CommandManager::RegisterCommand("onStopGame",
                                    [this](const CommandArgs &) {
                                        if (!m_IsPlayMode) {
                                            Logger::Log(LogLevel::WARNING, "Not in play mode");
                                            return;
                                        }

                                        Logger::Log(LogLevel::INFO, "=== EXITING PLAY MODE ===");
                                        StopPlayMode();
                                    });
}

void SceneManager::RegisterDebugEvents() {
    GetEventBus().Subscribe("debug_pause",
                            [this](const std::any &) { PauseScripts(); });

    GetEventBus().Subscribe("debug_resume",
                            [this](const std::any &) { ResumeScripts(); });

    CommandManager::RegisterCommand("onDebugPauseToggle",
                                    [this](const CommandArgs &) {
                                        if (m_IsDebugPaused)
                                            ResumeScripts();
                                        else
                                            PauseScripts();
                                    });
}
