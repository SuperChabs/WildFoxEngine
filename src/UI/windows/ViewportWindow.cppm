module;

#include <imgui.h>
#include <ImGuizmo.h>
#include <entt/entt.hpp>
#include "entt/entity/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <string>
#include <cstdint>

export module WFE.UI.Windows.ViewportWindow;

import WFE.Core.Logger;
import WFE.Rendering.Core.Framebuffer;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class ViewportWindow 
{
private:
    bool isOpen = true;
    ImVec2 viewportSize = {800, 600};
    ImVec2 viewportPos = {0, 0};
    bool isHovered = false;
    bool isFocused = false;

    int m_GizmoOperation = ImGuizmo::TRANSLATE;

public:
    void Render(const std::string& label, Framebuffer* framebuffer, ECSWorld* ecs, 
                entt::entity cameraEntity, entt::entity selectedEntity,
                bool isPlayMode) 
    {
        if (!isOpen) return;
        
        if (label == "Scene")
            ImGui::SetNextWindowPos({50, 50}, ImGuiCond_FirstUseEver);
        else if (label == "Game")
            ImGui::SetNextWindowPos({900, 50}, ImGuiCond_FirstUseEver);

        ImGui::SetNextWindowSize({800, 650}, ImGuiCond_FirstUseEver);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

        std::string windowTitle = label + " Viewport";
        ImGui::Begin(windowTitle.c_str(), &isOpen);

        isHovered = ImGui::IsWindowHovered();
        isFocused = ImGui::IsWindowFocused();

        if (isFocused && !isPlayMode) 
        {
            if (ImGui::IsKeyPressed(ImGuiKey_W)) m_GizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_E)) m_GizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) m_GizmoOperation = ImGuizmo::SCALE;
        }

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

            // Gizmos
            if (label == "Scene" && !isPlayMode && selectedEntity != entt::null && 
                ecs && cameraEntity != entt::null)
            {
                if (selectedEntity != entt::null && ecs && 
                    ecs->IsValid(cameraEntity) && 
                    ecs->HasComponent<CameraComponent>(cameraEntity)) 
                {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();
                    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
                    
                    auto& cameraComp = ecs->GetComponent<CameraComponent>(cameraEntity);
                    auto& cameraTransform = ecs->GetComponent<TransformComponent>(cameraEntity);
                    auto& cameraOrientation = ecs->GetComponent<CameraOrientationComponent>(cameraEntity);
                    
                    float aspect = size.x / size.y;
                    glm::mat4 cameraView = cameraOrientation.GetViewMatrix(cameraTransform.position);
                    glm::mat4 cameraProj = cameraComp.GetProjectionMatrix(aspect);
                    
                    auto& tc = ecs->GetComponent<TransformComponent>(selectedEntity);
                    glm::mat4 transform = tc.GetModelMatrix();
                    
                    ImGuizmo::Manipulate(
                        glm::value_ptr(cameraView),
                        glm::value_ptr(cameraProj),
                        (ImGuizmo::OPERATION)m_GizmoOperation, 
                        ImGuizmo::LOCAL,
                        glm::value_ptr(transform)
                    );
                    
                    if (ImGuizmo::IsUsing()) {
                        UpdateTransformFromMatrix(tc, transform);
                        // If the selected entity is a camera, sync orientation component
                        if (ecs->HasComponent<CameraOrientationComponent>(selectedEntity)) {
                            auto& orient = ecs->GetComponent<CameraOrientationComponent>(selectedEntity);
                            // TransformComponent stores Euler angles in degrees: x=pitch, y=yaw
                            orient.yaw = tc.rotation.y;
                            orient.pitch = tc.rotation.x;
                        }
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
    void UpdateTransformFromMatrix(TransformComponent& tc, const glm::mat4& matrix) 
    {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat orientation;
        
        glm::decompose(matrix, tc.scale, orientation, tc.position, skew, perspective);
        
        glm::vec3 rotationRadians = glm::eulerAngles(orientation);
        tc.rotation.x = glm::degrees(rotationRadians.x);
        tc.rotation.y = glm::degrees(rotationRadians.y);
        tc.rotation.z = glm::degrees(rotationRadians.z);
    }
};
