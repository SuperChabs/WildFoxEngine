#pragma once

#include <imgui.h>
#include <ImGuizmo.h>
#include <entt/entt.hpp>
#include "entt/entity/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/matrix_decompose.hpp>

#include <string>
#include <cstdint>

#include "core/logging/Logger.h"
#include "rendering/core/Framebuffer.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

class ViewportWindow
{
    bool isOpen = true;
    ImVec2 viewportSize = {0, 0};
    ImVec2 viewportPos = {0, 0};
    bool isHovered = false;
    bool isFocused = false;

    int m_GizmoOperation = ImGuizmo::TRANSLATE;

public:
    void Render(ECSWorld &ecs, entt::entity &selected, Framebuffer *framebuffer, CameraComponent &camera,
        TransformComponent &transform, CameraOrientationComponent &orientation, int viewportType);

    bool IsOpen() const { return isOpen; }
    ImVec2 GetViewportSize() const 
    { 
        return ImVec2(
            std::max(viewportSize.x, 1.0f), 
            std::max(viewportSize.y, 1.0f)
        );
    }
    ImVec2 GetViewportPos() const { return viewportPos; }
    bool IsHovered() const { return isHovered; }
    bool IsFocused() const { return isFocused; }
    
    void SetOpen(bool open) { isOpen = open; }
    void SetGizmoOperation(int op) { m_GizmoOperation = op; }

private:
    void UpdateTransformFromMatrix(TransformComponent& tc, const glm::mat4& matrix);

    void ProcessInput();
};