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
#include "resource/material/MaterialManager.h"

class MaterialPanel {
private:
    std::string selectedMaterial = "";

public:
    void Render(ECSWorld *ecs, entt::entity entity, MaterialManager *materialManager);

private:
    void AssignMaterial(ECSWorld *ecs, entt::entity entity,
                        MaterialManager *materialManager, const std::string &name);

    void RemoveMaterial(ECSWorld *ecs, entt::entity entity);
};