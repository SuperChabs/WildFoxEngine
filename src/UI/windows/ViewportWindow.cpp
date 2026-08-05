#include "ViewportWindow.h"

void ViewportWindow::Render(ECSWorld &ecs, entt::entity &selected, Framebuffer *framebuffer, CameraComponent &camera,
        TransformComponent &transform, CameraOrientationComponent &orientation, int viewportType) {
    if (!isOpen) return;

    ImGui::SetNextWindowSize({800, 650}, ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

    std::string windowTitle ="Viewport";
    ImGui::Begin(windowTitle.c_str(), &isOpen);

    isHovered = ImGui::IsWindowHovered();
    isFocused = ImGui::IsWindowFocused();

    ImVec2 size = ImGui::GetContentRegionAvail();

    if (size.x > 0 && size.y > 0 &&
        (size.x != viewportSize.x || size.y != viewportSize.y))
    {
        viewportSize = size;
        framebuffer->Resize(static_cast<int>(size.x),
                          static_cast<int>(size.y));

        Logger::Log(LogLevel::INFO,
            "Viewport resized to " +
            std::to_string(static_cast<int>(size.x)) + "x" +
            std::to_string(static_cast<int>(size.y)));
    }

    viewportPos = ImGui::GetCursorScreenPos();

    if (framebuffer)
    {
        ImGui::Image(
            reinterpret_cast<void*>(
                static_cast<intptr_t>(framebuffer->GetTextureID())
            ),
            size,
            {0, 1},
            {1, 0}
        );

        if (viewportType == 1 && selected != entt::null && ecs.IsValid(selected)){
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

            float aspect = size.x / size.y;
            glm::mat4 cameraView = orientation.GetViewMatrix(transform.position);
            glm::mat4 cameraProj = camera.GetProjectionMatrix(aspect);

            if (isFocused)
                ProcessInput();

            auto& tc = ecs.GetComponent<TransformComponent>(selected);
            glm::mat4 trans = tc.GetModelMatrix();

            ImGuizmo::Manipulate(
                glm::value_ptr(cameraView),
                glm::value_ptr(cameraProj),
                (ImGuizmo::OPERATION)m_GizmoOperation,
                ImGuizmo::WORLD,
                glm::value_ptr(trans)
            );

            if (ImGuizmo::IsUsing())
            {
                UpdateTransformFromMatrix(tc, trans);
                if (ecs.HasComponent<CameraOrientationComponent>(selected))
                {
                    auto& orient = ecs.GetComponent<CameraOrientationComponent>(selected);
                    orient.yaw = tc.rotation.y;
                    orient.pitch = tc.rotation.x;
                }
            }
        }
    }
    else
    {
        ImGui::Text("Framebuffer error");
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void ViewportWindow::UpdateTransformFromMatrix(TransformComponent &tc, const glm::mat4 &matrix) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, tc.scale, tc.rotation, tc.position, skew, perspective);
    tc.rotation = glm::normalize(tc.rotation);
}

void ViewportWindow::ProcessInput() {
    if (ImGui::IsKeyPressed(ImGuiKey_R)) m_GizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_S)) m_GizmoOperation = ImGuizmo::SCALE;
    if (ImGui::IsKeyPressed(ImGuiKey_T)) m_GizmoOperation = ImGuizmo::TRANSLATE;
}
