#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <string>
#include <memory>
#include <vector>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class ScriptPanel {
private:
    char scriptPathBuffer[512] = "";

public:
    void Render(ECSWorld *ecs, entt::entity entity);

private:
    void AttachScript(ECSWorld *ecs, entt::entity entity, const std::string &path);

    void UpdateScript(ScriptComponent &script, const std::string &newPath);

    void RemoveScript(ECSWorld *ecs, entt::entity entity, ScriptComponent &script);
};