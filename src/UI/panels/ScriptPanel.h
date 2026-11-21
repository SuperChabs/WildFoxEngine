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
    static void AttachScript(ECSWorld *ecs, entt::entity entity, const std::string &path);

    static void UpdateScript(c_Script &script, const std::string &newPath);

    static void RemoveScript(ECSWorld *ecs, entt::entity entity, c_Script &script);
};