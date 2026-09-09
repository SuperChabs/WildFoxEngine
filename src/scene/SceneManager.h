#pragma once

#include <glm/glm.hpp>

#include <string>

#include "core/EventBus.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

class SceneManager {
    ECSWorld *m_ecs;
    bool m_IsPlayMode = true;
    bool m_IsDebugPaused = false;

    std::string m_currentScenePath;

    glm::vec3 m_SavedEditorCameraPos;
    float m_SavedEditorCameraYaw;
    float m_SavedEditorCameraPitch;

    glm::vec3 m_SavedDebugCameraPos;
    float m_SavedDebugCameraYaw;
    float m_SavedDebugCameraPitch;
    bool m_HasSavedDebugCamera = false;

public:
    SceneManager(ECSWorld *ecs);

    void StartPlayMode();

    void StopPlayMode();

    void PauseScripts();

    void ResumeScripts();

    bool IsInPlayMode();

    void SetCurrentScenePath(const std::string &path) { m_currentScenePath = path; }
    void SetPlayMode(bool playMode) { m_IsPlayMode = playMode; }

    std::string GetCurrentScenePath() { return m_currentScenePath; }

private:
    void RegisterSceneCommands();

    void RegisterDebugEvents();
};