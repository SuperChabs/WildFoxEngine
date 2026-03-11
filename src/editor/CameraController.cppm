module;

#include <functional>
#include <memory>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

export module WFE.CameraController;

import WFE.Core.Input;
import WFE.Core.Window;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class CameraController
{
private:
    ECSWorld* ecs = nullptr;
    Input* input = nullptr;
    Window* window = nullptr;
    entt::entity* mainCameraEntity = nullptr;
    bool* isPlayMode = nullptr;
    bool* cameraControlEnabled = nullptr;

public:
    CameraController(ECSWorld* ecsWorld, Input* inputPtr, Window* wnd, entt::entity* mainCamPtr, bool* isPlayModePtr, bool* cameraControlEnabledPtr)
        : ecs(ecsWorld), input(inputPtr), window(wnd), mainCameraEntity(mainCamPtr), isPlayMode(isPlayModePtr), cameraControlEnabled(cameraControlEnabledPtr)
    {}

    void OnMouseMove(double xpos, double ypos)
    {
        if (!input || !ecs || !mainCameraEntity) return;

        input->UpdateMousePosition(xpos, ypos);

        if (cameraControlEnabled && *cameraControlEnabled && *mainCameraEntity != entt::null)
        {
            glm::vec2 delta = input->GetMouseDelta();
            auto& orientation = ecs->GetComponent<CameraOrientationComponent>(*mainCameraEntity);
            auto& config = ecs->GetComponent<CameraComponent>(*mainCameraEntity);

            orientation.yaw   += delta.x * config.mouseSensitivity;
            orientation.pitch += delta.y * config.mouseSensitivity;

            if (orientation.pitch > 89.0f)  orientation.pitch = 89.0f;
            if (orientation.pitch < -89.0f) orientation.pitch = -89.0f;

            input->ResetMouseDelta();
        }
    }

    void OnMouseButton(int button, int action, int mods)
    {
        if (!isPlayMode || !cameraControlEnabled) return;

        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        {
            if (!(*isPlayMode)) // allow camera control only when not in play mode
            {
                SetControlMode(!(*cameraControlEnabled));
            }
        }
    }

    void SetControlMode(bool enabled)
    {
        if (cameraControlEnabled)
            *cameraControlEnabled = enabled;

        if (!window) return;

        if (enabled)
            window->SetCursorMode(GLFW_CURSOR_DISABLED);
        else
            window->SetCursorMode(GLFW_CURSOR_NORMAL);
    }
};
