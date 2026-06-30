#pragma once

#include <imgui.h>

#include <string>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "resource/material/MaterialManager.h"
#include "core/CommandManager.h"

#include "UI/panels/TagPanel.h"
#include "UI/panels/TransformPanel.h"
#include "UI/panels/MaterialPanel.h"
#include "UI/panels/LightPanel.h"
#include "UI/panels/CameraPanel.h"
#include "UI/panels/ScriptPanel.h"
#include "UI/panels/AudioPanel.h"
#include "UI/panels/IconPanel.h"
#include "UI/panels/RigidBodyPanel.h"
#include "UI/panels/ColliderPanel.h"

class DebugOverlay {
public:
    bool visible = true;
    AudioPanel audioPanel;

    void Render(ECSWorld *ecs, entt::entity cameraEntity,
                MaterialManager *materialManager);

private:
    entt::entity m_selected = entt::null;

    char m_scenePathBuf[512] = "scene.json";
    char m_scriptPathBuf[256] = "assets/scripts/script.as";
    char m_entityNameBuf[128] = "New Entity";

    char m_modelPath[512]{};

    bool m_showOpenModelDialog = false;

    TagPanel tagPanel;
    TransformPanel transformPanel;
    MaterialPanel materialPanel;
    LightPanel lightPanel;
    CameraPanel cameraPanel;
    ScriptPanel scriptPanel;
    IconPanel iconPanel;
    RigidBodyPanel rigidPanel;
    ColliderPanel colliderPanel;

    void RenderSceneTab(ECSWorld *ecs);

    void RenderHierarchyTab(ECSWorld *ecs);

    void RenderEntityNode(entt::entity e, ECSWorld *ecs, entt::entity &toDelete);

    void RenderInspectorTab(ECSWorld *ecs, MaterialManager *materialManager);

    void RenderCreateEntityTab();

    void RenderOpenModelDialog();

    inline void Execute(const char *name, const CommandArgs &args) {
        if (CommandManager::HasCommand(name))
            CommandManager::ExecuteCommand(name, args);
    }

    inline void ExecuteItem(const char *label, const char *command) {
        if (ImGui::MenuItem(label))
            Execute(command);
    }

    inline void Execute(const char *name) {
        CommandArgs args;
        Execute(name, args);
    }

    inline void Execute(const char *name, const std::string &arg) {
        CommandArgs args;
        args.emplace_back(arg);
        Execute(name, args);
    }
};