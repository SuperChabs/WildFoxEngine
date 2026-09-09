#pragma once

#include "IViewportWindow.h"
#include "ECS/components/Components.h"
#include "rendering/core/Framebuffer.h"

class GameViewportWindow : public IViewportWindow {
public:
    void Render(Framebuffer *framebuffer);
};