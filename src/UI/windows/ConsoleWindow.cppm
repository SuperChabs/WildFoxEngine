module;

#include <imgui.h>

export module WFE.UI.Windows.ConsoleWindow;

export class ConsoleWindow
{
    bool isOpen = true;

public:
    void Render()
    {
        ImGui::SetNextWindowPos({300, 670}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({800, 120}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Console", &isOpen);
        ImGui::Text("[INFO] Editor ready with ECS");
        ImGui::End();
    }

    bool IsOpen() const { return isOpen; }

    void SetOpen(bool newOpen) { isOpen = newOpen; }
};