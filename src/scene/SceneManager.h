#pragma once

#include <glm/glm.hpp>

#include <string>

#include "core/EventBus.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

class SceneManager {
    ECSWorld *m_ecs;
    bool m_IsPlayMode = false;
    bool m_IsDebugPaused = false;

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

    bool IsInDebugMode();

private:
    void RegisterSceneCommands();

    void RegisterDebugEvents();
};
