#include "GameViewportWindow.h"

void GameViewportWindow::Render(Framebuffer *framebuffer) {
        if (!isOpen) return;

    ImGui::SetNextWindowSize({800, 650}, ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

    std::string windowTitle = "Game Viewport";
    ImGui::Begin(windowTitle.c_str(), &isOpen);

    isHovered = ImGui::IsWindowHovered();
    isFocused = ImGui::IsWindowFocused();

    ImVec2 size = ImGui::GetContentRegionAvail();

    if (size.x > 0 && size.y > 0 &&
        (size.x != viewportSize.x || size.y != viewportSize.y))
    {
        viewportSize = size;
        if (framebuffer)
            framebuffer->Resize(static_cast<int>(size.x), static_cast<int>(size.y));

        Logger::Log(LogLevel::INFO, "Viewport resized to " +
            std::to_string(static_cast<int>(size.x)) + "x" + std::to_string(static_cast<int>(size.y)));
    }

    viewportPos = ImGui::GetCursorScreenPos();

    if (framebuffer)
    {
        ImGui::Image(
            framebuffer->GetTextureID(),
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
