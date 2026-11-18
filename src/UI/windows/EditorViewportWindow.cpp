#include "EditorViewportWindow.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void EditorViewportWindow::Render(ECSWorld &ecs, const entt::entity &selected, Framebuffer *framebuffer,
    const c_Camera &camera, c_Transform &transform, c_CameraOrientation &orientation) {
    if (!isOpen) return;

    ImGui::SetNextWindowSize({800, 650}, ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

    std::string windowTitle = "Editor Viewport";
    ImGui::Begin(windowTitle.c_str(), &isOpen);

    isHovered = ImGui::IsWindowHovered();
    isFocused = ImGui::IsWindowFocused();

    ImVec2 size = ImGui::GetContentRegionAvail();

    if (size.x > 0 && size.y > 0 &&
        (size.x != viewportSize.x || size.y != viewportSize.y)) {
        viewportSize = size;
        if (framebuffer)
            framebuffer->Resize(static_cast<int>(size.x), static_cast<int>(size.y));

        Logger::Log(LogLevel::INFO, "Viewport resized to " +
                                    std::to_string(static_cast<int>(size.x)) + "x" + std::to_string(
                                        static_cast<int>(size.y)));
    }

    viewportPos = ImGui::GetCursorScreenPos();

    if (framebuffer) {
        ImGui::Image(
            framebuffer->GetTextureID(),
            size,
            {0, 1},
            {1, 0}
        );

        if (selected != entt::null && ecs.IsValid(selected) && ecs.HasComponent<c_Transform>(selected)) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

            float aspect = size.x / size.y;
            glm::mat4 cameraView = orientation.GetViewMatrix(transform.position);
            glm::mat4 cameraProj = camera.GetProjectionMatrix(aspect);

            if (isFocused || isHovered) {
                ProcessInput();
            }

            auto &tc = ecs.GetComponent<c_Transform>(selected);
            glm::mat4 trans = tc.GetModelMatrix();

            ImGuizmo::Manipulate(
                glm::value_ptr(cameraView),
                glm::value_ptr(cameraProj),
                static_cast<ImGuizmo::OPERATION>(m_GizmoOperation),
                ImGuizmo::WORLD,
                glm::value_ptr(trans)
            );

            if (ImGuizmo::IsUsing()) {
                UpdateTransformFromMatrix(tc, trans);
                if (ecs.HasComponent<c_CameraOrientation>(selected)) {
                    auto &orient = ecs.GetComponent<c_CameraOrientation>(selected);
                    orient.yaw = tc.rotation.y;
                    orient.pitch = tc.rotation.x;
                }
            }
        }

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

        glm::mat4 view = orientation.GetViewMatrix(transform.position);
        glm::mat4 before = view;

        constexpr float gizmoSize = 128.0f;
        constexpr float pivotDistance = 16.0f;

        ImGuizmo::ViewManipulate(glm::value_ptr(view), pivotDistance,
            ImVec2(viewportPos.x + viewportSize.x - gizmoSize, viewportPos.y),
            ImVec2(gizmoSize, gizmoSize), 0x10101010);

        if (view != before) {
            SetFromViewMatrix(transform, orientation, view);
            FocusOnEntity(ecs, selected, transform, orientation);
        }

    } else {
        ImGui::Text("Framebuffer error");
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorViewportWindow::UpdateTransformFromMatrix(c_Transform &tc, const glm::mat4 &matrix) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, tc.scale, tc.rotation, tc.position, skew, perspective);
    tc.rotation = glm::normalize(tc.rotation);
}

void EditorViewportWindow::SetFromViewMatrix(c_Transform& t, c_CameraOrientation& o, const glm::mat4& view)
{
    glm::mat4 inv = glm::inverse(view);

    t.position = glm::vec3(inv[3]);
    const glm::vec3 forward = glm::normalize(-glm::vec3(inv[2]));

    o.yaw   = glm::degrees(std::atan2(forward.z, forward.x));
    o.pitch = glm::clamp(glm::degrees(std::asin(forward.y)), -89.0f, 89.0f);
}

void EditorViewportWindow::FocusOnEntity(ECSWorld &ecs, entt::entity target, c_Transform &camTransform,
        const c_CameraOrientation &orientation) {
    if (target == entt::null || !ecs.IsValid(target) || !ecs.HasComponent<c_Transform>(target))
        return;

    auto& tc = ecs.GetComponent<c_Transform>(target);

    glm::vec3 center = glm::vec3(tc.GetModelMatrix()[3]);

    float radius   = glm::max(tc.scale.x, glm::max(tc.scale.y, tc.scale.z));
    float distance = glm::max(radius * 3.0f, 3.0f);

    camTransform.position = center - orientation.GetFront() * distance;
}

void EditorViewportWindow::ProcessInput() {
    if (ImGui::IsKeyPressed(ImGuiKey_R) && !ImGui::GetIO().WantTextInput) m_GizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_S) && !ImGui::GetIO().WantTextInput) m_GizmoOperation = ImGuizmo::SCALE;
    if (ImGui::IsKeyPressed(ImGuiKey_T) && !ImGui::GetIO().WantTextInput) m_GizmoOperation = ImGuizmo::TRANSLATE;
}
