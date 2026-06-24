#pragma once

#include <imgui.h>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class AudioPanel {
private:
    char m_pathBuffer[512] = "";

public:
    void Render(ECSWorld *ecs, entt::entity entity);

private:
    void RenderSource(ECSWorld *ecs, entt::entity entity);

    void RenderListener(ECSWorld *ecs, entt::entity entity);
};