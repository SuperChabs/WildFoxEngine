module;

#include <imgui.h>

export module WFE.UI.Windows.PropertiesWindow;

import WFE.Core.Camera;

export class PropertiesWindow
{
    bool isOpen = true;

public:
    void Render(Camera* camera)
    {
        ImGui::SetNextWindowPos({10, 580}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 250}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Scene Settings", &isOpen);

        if (ImGui::CollapsingHeader("Camera"))
        {
            float speed = camera->GetMovementSpeed();
            if (ImGui::SliderFloat("Speed", &speed, 1.f, 20.f))
                camera->SetMovementSpeed(speed);

            ImGui::Spacing();

            float sensitivity = camera->GetMouseSensitivity();
            if (ImGui::SliderFloat("Mouse Sensitivity", &sensitivity, 0.0f, 1.0f))
                camera->SetMouseSensitivity(sensitivity);

            ImGui::Spacing();

            float zoom = camera->GetZoom();
            if (ImGui::SliderFloat("Zoom", &zoom, 1.0f, 100.0f))
                camera->SetZoom(zoom);

            ImGui::Spacing();

            float pitch = camera->GetPitch();
            if (ImGui::SliderFloat("Pitch", &pitch, -90.0f, 90.0f, "%.1f"))
                camera->SetPitch(pitch);

            ImGui::Spacing();

            float yaw = camera->GetYaw();
            if (ImGui::SliderFloat("Yaw", &yaw, -360.0f, 360.0f, "%.1f"))
                camera->SetYaw(yaw);
        }

        ImGui::End();
    }

    bool IsOpen() const { return isOpen; }

    void SetOpen(bool newOpen) { isOpen = newOpen; }
};