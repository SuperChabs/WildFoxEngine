module;

#include <glm/glm.hpp>
#include <entt/entt.hpp>

export module WFE.Scene.SceneManager;

import WFE.Core.EventBus;
import WFE.Core.Logger;
import WFE.Core.CommandManager;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class SceneManager
{
    ECSWorld* m_ecs;
    bool m_IsPlayMode = false;

    glm::vec3 m_SavedEditorCameraPos;
    float m_SavedEditorCameraYaw;
    float m_SavedEditorCameraPitch;

public:
    SceneManager(ECSWorld* ecs)
        : m_ecs(ecs)
    {}

    void StartPlayMode()
    {
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
    }

    void StopPlayMode()
    {
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
            script.active = false;
            script.loaded = false;
            script.failed = false;
            script.env = decltype(script.env)();
        });

        m_IsPlayMode = false;

        GetEventBus().Publish("play_mode_stopped");

        Logger::Log(LogLevel::INFO, "SceneManager: Exited Play Mode");
    }
    
    bool IsInPlayMode() const { return m_IsPlayMode; }

private:
    void RegisterSceneCommands() 
    {
        CommandManager::RegisterCommand("onPlayGame",
        [this](const CommandArgs&)
        {
            if (IsInPlayMode())
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
            if (!IsInPlayMode())
            {
                Logger::Log(LogLevel::WARNING, "Not in play mode");
                return;
            }

            Logger::Log(LogLevel::INFO, "=== EXITING PLAY MODE ===");
            StopPlayMode();
        });
    }
};