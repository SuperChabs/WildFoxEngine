module;

#include <imgui.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

export module WFE.UI.Windows.PropertiesWindow;

import WFE.Core.Camera;
import WFE.Rendering.Renderer;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class PropertiesWindow
{
    bool isOpen = true;

public:
    void Render(ECSWorld* ecs, entt::entity cameraEntity, Renderer* renderer)
    {
        ImGui::SetNextWindowPos({10, 580}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 250}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Scene Settings", &isOpen);

        if (ImGui::CollapsingHeader("Camera"))
        {
            if (cameraEntity != entt::null && ecs->IsValid(cameraEntity))
            {
                auto& camera = ecs->GetComponent<CameraComponent>(cameraEntity);
                
                ImGui::SliderFloat("Speed", &camera.movementSpeed, 1.f, 20.f);
                ImGui::Spacing();
                
                ImGui::SliderFloat("Mouse Sensitivity", &camera.mouseSensitivity, 0.0f, 1.0f);
                ImGui::Spacing();
                
                ImGui::SliderFloat("FOV", &camera.fov, 1.0f, 100.0f);
                ImGui::Spacing();
                
                if (ecs->HasComponent<CameraOrientationComponent>(cameraEntity))
                {
                    auto& orientation = ecs->GetComponent<CameraOrientationComponent>(cameraEntity);
                    ImGui::SliderFloat("Pitch", &orientation.pitch, -90.0f, 90.0f);
                    ImGui::Spacing();
                    ImGui::SliderFloat("Yaw", &orientation.yaw, -360.0f, 360.0f);
                }
            }
        }

        // if (ImGui::CollapsingHeader("Grid"))
        // {
        //     if (!renderer || !renderer->GetGrid())
        //     {
        //         ImGui::Text("Grid not available");
        //         return;
        //     }
        //    
        //     auto* grid = renderer->GetGrid();
        //    
        //     bool enabled = grid->IsEnabled();
        //     if (ImGui::Checkbox("Show Grid", &enabled))
        //     {
        //         grid->SetEnabled(enabled);
        //     }
        //    
        //     ImGui::Spacing();
        //     ImGui::Separator();
        //     ImGui::Spacing();
        //    
        //     float scale = grid->GetGridScale();
        //     if (ImGui::SliderFloat("Cell Size", &scale, 0.1f, 10.0f))
        //     {
        //         grid->SetGridScale(scale);
        //     }
        //    
        //     ImGui::Spacing();
        //    
        //     float fadeStart = grid->GetFadeStart();
        //     if (ImGui::SliderFloat("Fade Start", &fadeStart, 10.0f, 200.0f))
        //     {
        //         grid->SetFadeStart(fadeStart);
        //     }
        //    
        //     float fadeDistance = grid->GetFadeDistance();
        //     if (ImGui::SliderFloat("Fade Distance", &fadeDistance, 20.0f, 300.0f))
        //     {
        //         grid->SetFadeDistance(fadeDistance);
        //     }
        //    
        //     ImGui::Spacing();
        //    
        //     float thickness = grid->GetLineThickness();
        //     if (ImGui::SliderFloat("Line Thickness", &thickness, 0.5f, 3.0f))
        //     {
        //         grid->SetLineThickness(thickness);
        //     }
        //    
        //     ImGui::Spacing();
        //     ImGui::Separator();
        //     ImGui::Spacing();
        //    
        //     if (ImGui::TreeNode("Colors"))
        //     {
        //         static float thinColor[3] = {0.3f, 0.3f, 0.3f};
        //         if (ImGui::ColorEdit3("Thin Lines", thinColor))
        //         {
        //             grid->SetThinLineColor(glm::vec3(thinColor[0], thinColor[1], thinColor[2]));
        //         }
        //        
        //         static float thickColor[3] = {0.5f, 0.5f, 0.5f};
        //         if (ImGui::ColorEdit3("Thick Lines", thickColor))
        //         {
        //             grid->SetThickLineColor(glm::vec3(thickColor[0], thickColor[1], thickColor[2]));
        //         }
        //        
        //         static float xAxisColor[3] = {1.0f, 0.3f, 0.3f};
        //         if (ImGui::ColorEdit3("X Axis", xAxisColor))
        //         {
        //             grid->SetAxisXColor(glm::vec3(xAxisColor[0], xAxisColor[1], xAxisColor[2]));
        //         }
        //        
        //         static float zAxisColor[3] = {0.3f, 0.3f, 1.0f};
        //         if (ImGui::ColorEdit3("Z Axis", zAxisColor))
        //         {
        //             grid->SetAxisZColor(glm::vec3(zAxisColor[0], zAxisColor[1], zAxisColor[2]));
        //         }
        //
        //         ImGui::TreePop();
        //     }
        // }

        ImGui::End();
    }

    bool IsOpen() const { return isOpen; }

    void SetOpen(bool newOpen) { isOpen = newOpen; }
};