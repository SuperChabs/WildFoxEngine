#pragma once

#include <string>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "resource/material/MaterialManager.h"
#include "core/CommandManager.h"

class MaterialPanel {
    std::string selectedMaterial;

    char m_name[128]{};
    std::string m_diffusePath;
    std::string m_specularPath;
    std::string m_normalPath;
    std::string m_heightPath;

    bool m_showOpenMaterialDialog = false;

public:
    void Render(ECSWorld *ecs, entt::entity entity, MaterialManager *materialManager);

private:
    static void AssignMaterial(ECSWorld *ecs, entt::entity entity, MaterialManager *materialManager, const std::string &name);
    static void RemoveMaterial(ECSWorld *ecs, entt::entity entity);

    void RenderOpenMaterialDialog();

    static void RenderOpenFileDialog(const std::string &name, std::string &out);

    static void Execute(const char *name, const CommandArgs &args) {
        if (CommandManager::HasCommand(name))
            CommandManager::ExecuteCommand(name, args);
    }
};