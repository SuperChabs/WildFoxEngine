module;

#include <imgui.h>

#include <string>
#include <cstdint>

export module XEngine.UI.Windows.ViewportWindow;

import XEngine.Core.Logger;
import XEngine.Rendering.Framebuffer;

export class ViewportWindow 
{
private:
    bool isOpen = true;
    ImVec2 viewportSize = {800, 600};
    ImVec2 viewportPos = {0, 0};
    bool isHovered = false;
    bool isFocused = false;

public:
    void Render(Framebuffer* framebuffer) 
    {
        if (!isOpen) return;
        
        ImGui::SetNextWindowPos({300, 10}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({800, 650}, ImGuiCond_FirstUseEver);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        ImGui::Begin("Game Viewport", &isOpen);

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
        }
        else
        {
            ImGui::Text("Framebuffer error");
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }
    
    bool IsOpen() const { return isOpen; }
    ImVec2 GetViewportSize() const { return viewportSize; }
    ImVec2 GetViewportPos() const { return viewportPos; }
    bool IsHovered() const { return isHovered; }
    bool IsFocused() const { return isFocused; }
    
    void SetOpen(bool open) { isOpen = open; }
};