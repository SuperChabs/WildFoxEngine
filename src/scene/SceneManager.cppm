module;

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <any>

export module WFE.Scene.SceneManager;

import WFE.Core.EventBus;
import WFE.Core.Logger;
import WFE.Core.CommandManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class SceneManager
{
    ECSWorld* m_ecs;
    bool m_IsPlayMode    = false;
    bool m_IsDebugPaused = false;

    glm::vec3 m_SavedEditorCameraPos;
    float m_SavedEditorCameraYaw;
    float m_SavedEditorCameraPitch;

    glm::vec3 m_SavedDebugCameraPos;
    float m_SavedDebugCameraYaw;
    float m_SavedDebugCameraPitch;
    bool  m_HasSavedDebugCamera = false;

public:
    SceneManager(ECSWorld* ecs)
        : m_ecs(ecs)
    {
        RegisterSceneCommands();
        RegisterDebugEvents();
    }

    void StartPlayMode()
    {
        /*
        if (!m_ecs || m_IsPlayMode)
            return;

        if (!m_ecs)
        {
            Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
            return;
        }

        entt::entity editorCam = m_ecs->FindEditorCamera();
        if (editorCam != entt::null && m_ecs->IsValid(editorCam))
        {
            auto& transform = m_ecs->GetComponent<TransformComponent>(editorCam);
            auto& orientation = m_ecs->GetComponent<CameraOrientationComponent>(editorCam);

            m_SavedEditorCameraPos = transform.position;
            m_SavedEditorCameraYaw = orientation.yaw;
            m_SavedEditorCameraPitch = orientation.pitch;
        }

        m_ecs->Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script)
        {
            script.active = true;
            script.loaded = false;
            script.failed = false;
        });

        m_IsPlayMode = true;

        GetEventBus().Publish("play_mode_started");

        Logger::Log(LogLevel::INFO, "SceneManager: Entered Play Mode");
        */
    }

    void StopPlayMode()
    {
        /*
        if (!m_ecs || !m_IsPlayMode)
            return;

        if (!m_ecs)
        {
            Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
            return;
        }

        entt::entity editorCam = m_ecs->FindEditorCamera();
        if (editorCam != entt::null && m_ecs->IsValid(editorCam))
        {
            auto& transform = m_ecs->GetComponent<TransformComponent>(editorCam);
            auto& orientation = m_ecs->GetComponent<CameraOrientationComponent>(editorCam);

            transform.position = m_SavedEditorCameraPos;
            orientation.yaw = m_SavedEditorCameraYaw;
            orientation.pitch = m_SavedEditorCameraPitch;
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
        */
    }
    
    void PauseScripts()
    {
        if (!m_ecs || m_IsDebugPaused) return;

        entt::entity gameCam = m_ecs->FindGameCamera();
        if (gameCam != entt::null && m_ecs->IsValid(gameCam)
            && m_ecs->HasComponent<TransformComponent>(gameCam)
            && m_ecs->HasComponent<CameraOrientationComponent>(gameCam))
        {
            auto& transform    = m_ecs->GetComponent<TransformComponent>(gameCam);
            auto& orientation  = m_ecs->GetComponent<CameraOrientationComponent>(gameCam);

            m_SavedDebugCameraPos   = transform.position;
            m_SavedDebugCameraYaw   = orientation.yaw;
            m_SavedDebugCameraPitch = orientation.pitch;
            m_HasSavedDebugCamera   = true;

            Logger::Log(LogLevel::INFO, "SceneManager: Camera state saved for debug pause");
        }

        m_ecs->Each<ScriptComponent>([](entt::entity, ScriptComponent& script)
        {
            script.active = false;
        });

        m_IsDebugPaused = true;
        GetEventBus().Publish("debug_paused");
        Logger::Log(LogLevel::INFO, "SceneManager: Scripts PAUSED (debug)");
    }

    void ResumeScripts()
    {
        if (!m_ecs || !m_IsDebugPaused) return;

        if (m_HasSavedDebugCamera)
        {
            entt::entity gameCam = m_ecs->FindGameCamera();
            if (gameCam != entt::null && m_ecs->IsValid(gameCam)
                && m_ecs->HasComponent<TransformComponent>(gameCam)
                && m_ecs->HasComponent<CameraOrientationComponent>(gameCam))
            {
                auto& transform   = m_ecs->GetComponent<TransformComponent>(gameCam);
                auto& orientation = m_ecs->GetComponent<CameraOrientationComponent>(gameCam);

                transform.position  = m_SavedDebugCameraPos;
                orientation.yaw     = m_SavedDebugCameraYaw;
                orientation.pitch   = m_SavedDebugCameraPitch;

                Logger::Log(LogLevel::INFO, "SceneManager: Camera state restored after debug pause");
            }
            m_HasSavedDebugCamera = false;
        }

        m_ecs->Each<ScriptComponent>([](entt::entity, ScriptComponent& script)
        {
            if (!script.failed)
                script.active = true;
        });

        m_IsDebugPaused = false;
        GetEventBus().Publish("debug_resumed");
        Logger::Log(LogLevel::INFO, "SceneManager: Scripts RESUMED");
    }

    bool IsInPlayMode()  { return m_IsPlayMode; }
    bool IsInDebugMode() { return m_IsDebugPaused; }

private:
    void RegisterSceneCommands() 
    {
        CommandManager::RegisterCommand("onPlayGame",
        [this](const CommandArgs&)
        {
            if (m_IsPlayMode)
            {
                Logger::Log(LogLevel::WARNING, "Already in play mode");
                return;
            }

            Logger::Log(LogLevel::INFO, "=== ENTERING PLAY MODE ===");
            StartPlayMode();
        });

        CommandManager::RegisterCommand("onStopGame",
        [this](const CommandArgs&)
        {
            if (!m_IsPlayMode)
            {
                Logger::Log(LogLevel::WARNING, "Not in play mode");
                return;
            }

            Logger::Log(LogLevel::INFO, "=== EXITING PLAY MODE ===");
            StopPlayMode();
        });
    }

    void RegisterDebugEvents()
    {
        GetEventBus().Subscribe("debug_pause",
        [this](const std::any&) { PauseScripts(); });

        GetEventBus().Subscribe("debug_resume",
        [this](const std::any&) { ResumeScripts(); });

        CommandManager::RegisterCommand("onDebugPauseToggle",
        [this](const CommandArgs&)
        {
            if (m_IsDebugPaused)
                ResumeScripts();
            else
                PauseScripts();
        });
    }
};