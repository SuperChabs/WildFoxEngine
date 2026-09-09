#pragma once

#include <algorithm>

#include <imgui.h>

class IViewportWindow {
protected:
    bool isOpen = true;
    ImVec2 viewportSize = {0, 0};
    ImVec2 viewportPos = {0, 0};
    bool isHovered = false;
    bool isFocused = false;

public:
    virtual ~IViewportWindow() = default;

    bool IsOpen() const { return isOpen; }

    ImVec2 GetViewportSize() const {
        return ImVec2(
            std::max(viewportSize.x, 1.0f),
            std::max(viewportSize.y, 1.0f)
        );
    }

    ImVec2 GetViewportPos() const { return viewportPos; }
    bool IsHovered() const { return isHovered; }
    bool IsFocused() const { return isFocused; }

    void SetOpen(bool open) { isOpen = open; }
};