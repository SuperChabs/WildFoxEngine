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
#include "core/CommandManager.h"

class MaterialPanel {
    std::string selectedMaterial = "";

    char m_name[128]{};
    char m_diffusePath[512]{};
    char m_specularPath[512]{};
    char m_normalPath[512]{};
    char m_heightPath[512]{};

    bool m_showOpenMaterialDialog = false;

public:
    void Render(ECSWorld *ecs, entt::entity entity, MaterialManager *materialManager);

private:
    void AssignMaterial(ECSWorld *ecs, entt::entity entity,
                        MaterialManager *materialManager, const std::string &name);

    void RemoveMaterial(ECSWorld *ecs, entt::entity entity);

    void RenderOpenMaterialDialog();

    inline void Execute(const char *name, const CommandArgs &args) {
        if (CommandManager::HasCommand(name))
            CommandManager::ExecuteCommand(name, args);
    }
};