#pragma once

#include <imgui.h>
#include <ImGuizmo.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "core/logging/Logger.h"
#include "rendering/core/Framebuffer.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "IViewportWindow.h"

class EditViewportWindow : public IViewportWindow {
    int m_GizmoOperation = ImGuizmo::TRANSLATE;

public:
    void Render(ECSWorld &ecs, entt::entity &selected, Framebuffer *framebuffer, CameraComponent &camera,
                TransformComponent &transform, CameraOrientationComponent &orientation);

    void SetGizmoOperation(int op) { m_GizmoOperation = op; }

private:
    void UpdateTransformFromMatrix(TransformComponent &tc, const glm::mat4 &matrix);

    void ProcessInput();
};