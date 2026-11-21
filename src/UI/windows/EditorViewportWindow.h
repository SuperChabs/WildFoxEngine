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

class EditorViewportWindow : public IViewportWindow {
    int m_GizmoOperation = ImGuizmo::TRANSLATE;

public:
    void Render(ECSWorld &ecs, entt::entity &selected, Framebuffer *framebuffer, const c_Camera &camera,
                c_Transform &transform, c_CameraOrientation &orientation);

    void SetGizmoOperation(int op) { m_GizmoOperation = op; }

private:
    static void UpdateTransformFromMatrix(c_Transform &tc, const glm::mat4 &matrix);
    static void SetFromViewMatrix(c_Transform& t, c_CameraOrientation& o, const glm::mat4& view);
    static void FocusOnEntity(ECSWorld& ecs, entt::entity target, c_Transform& camTransform,
        const c_CameraOrientation& orientation);

    void ProcessInput();
    void HandlePicking(ECSWorld &ecs, entt::entity &selected, const glm::mat4 &view, const glm::mat4 &proj,
                       float farPlane, ImVec2 viewCubeMin, ImVec2 viewCubeMax) const;
};