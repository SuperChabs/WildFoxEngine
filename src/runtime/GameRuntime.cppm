module;

#include <memory>

export module WFE.GameRuntime;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.ECS.Systems;

export class GameRuntime
{
private:
    ECSWorld* ecs = nullptr;
    entt::entity* mainCameraEntity = nullptr;
    bool* isPlayMode = nullptr;

    std::unique_ptr<RotationSystem> rotationSystem;

    glm::vec3 savedEditorCameraPos;
    float savedEditorCameraYaw = 0.0f;
    float savedEditorCameraPitch = 0.0f;

public:
    GameRuntime(ECSWorld* ecsWorld, entt::entity* mainCamPtr, bool* isPlayModePtr)
        : ecs(ecsWorld), mainCameraEntity(mainCamPtr), isPlayMode(isPlayModePtr)
    {
        rotationSystem = std::make_unique<RotationSystem>();
    }

    void Start()
    {
        if (!ecs || !isPlayMode || *isPlayMode) return;

        entt::entity editorCam = ecs->FindEditorCamera();
        if (editorCam != entt::null && ecs->IsValid(editorCam))
        {
            auto& transform = ecs->GetComponent<TransformComponent>(editorCam);
            auto& orientation = ecs->GetComponent<CameraOrientationComponent>(editorCam);

            savedEditorCameraPos = transform.position;
            savedEditorCameraYaw = orientation.yaw;
            savedEditorCameraPitch = orientation.pitch;
        }

        entt::entity gameCam = ecs->FindGameCamera();
        if (gameCam != entt::null)
        {
            if (mainCameraEntity) *mainCameraEntity = gameCam;
            auto& gameCamComp = ecs->GetComponent<CameraComponent>(gameCam);
            gameCamComp.isMainCamera = true;
        }

        *isPlayMode = true;
    }

    void Stop()
    {
        if (!ecs || !isPlayMode || !*isPlayMode) return;

        entt::entity editorCam = ecs->FindEditorCamera();
        if (editorCam != entt::null && ecs->IsValid(editorCam))
        {
            auto& transform = ecs->GetComponent<TransformComponent>(editorCam);
            auto& orientation = ecs->GetComponent<CameraOrientationComponent>(editorCam);

            transform.position = savedEditorCameraPos;
            orientation.yaw = savedEditorCameraYaw;
            orientation.pitch = savedEditorCameraPitch;

            auto& editorCamComp = ecs->GetComponent<CameraComponent>(editorCam);
            editorCamComp.isMainCamera = true;

            if (mainCameraEntity) *mainCameraEntity = editorCam;

            entt::entity gameCam = ecs->FindGameCamera();
            if (gameCam != entt::null)
            {
                auto& gameCamComp = ecs->GetComponent<CameraComponent>(gameCam);
                gameCamComp.isMainCamera = false;
            }
        }

        *isPlayMode = false;
    }

    bool IsRunning() const { return isPlayMode && *isPlayMode; }

    void Update(float deltaTime)
    {
        if (rotationSystem && IsRunning())
            rotationSystem->Update(*ecs, deltaTime);
    }
};
