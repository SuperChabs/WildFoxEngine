#pragma once

#include <imgui.h>

#include <string>
#include <memory>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "resource/material/MaterialManager.h"
#include "core/CommandManager.h"
#include "scene/serializer/SceneSerializer.h"
#include "rendering/core/Framebuffer.h"
#include "engine/EditorCamera.h"

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

#include "UI/windows/EditViewportWindow.h"
#include "UI/windows/GameViewportWindow.h"

class DebugOverlay {
    entt::entity m_selected = entt::null;

    char m_scenePathBuf[512] = "scene.json";
    char m_scriptPathBuf[256] = "assets/scripts/script.as";
    char m_entityNameBuf[128] = "New Entity";

    char m_modelPath[512]{};

    bool m_showOpenModelDialog = false;

    AudioPanel audioPanel;
    TagPanel tagPanel;
    TransformPanel transformPanel;
    MaterialPanel materialPanel;
    LightPanel lightPanel;
    CameraPanel cameraPanel;
    ScriptPanel scriptPanel;
    IconPanel iconPanel;
    RigidBodyPanel rigidPanel;
    ColliderPanel colliderPanel;

    EditViewportWindow editorViewportWindow;
    GameViewportWindow gameViewportWindow;

    std::unique_ptr<Framebuffer> editorFramebuffer;
    std::unique_ptr<Framebuffer> gameFramebuffer;

    std::vector<std::string> m_availableScenes;
    std::vector<std::string> m_availableObjects;
    int m_selectedScene = -1;
    int m_selectedObject = -1;
    bool m_scenesLoaded = false;

    std::string m_pendingDeleteScene;

public:
    bool visible = true;

    DebugOverlay();

    void Render(ECSWorld *ecs, MaterialManager *materialManager, SceneSerializer *ss, EditorCamera &editorCamera);

    [[nodiscard]] ImVec2 GetEditorViewportSize() const { return editorViewportWindow.GetViewportSize(); }
    [[nodiscard]] ImVec2 GetEditorViewportPose() const { return editorViewportWindow.GetViewportPos(); }
    [[nodiscard]] bool IsEditorViewportHovered() const { return editorViewportWindow.IsHovered(); }
    [[nodiscard]] bool IsEditorViewportFocused() const { return editorViewportWindow.IsFocused(); }

    [[nodiscard]] ImVec2 GetGameViewportSize() const { return gameViewportWindow.GetViewportSize(); }
    [[nodiscard]] ImVec2 GetGameViewportPos()  const { return gameViewportWindow.GetViewportPos(); }
    [[nodiscard]] bool IsGameViewportHovered() const { return gameViewportWindow.IsHovered(); }
    [[nodiscard]] bool IsGameViewportFocused() const { return gameViewportWindow.IsFocused(); }

    [[nodiscard]] Framebuffer* GetEditorFramebuffer() const { return editorFramebuffer.get(); }
    [[nodiscard]] Framebuffer* GetGameFramebuffer()   const { return gameFramebuffer.get(); }

    [[nodiscard]] EditViewportWindow* GetEditViewportWindow() { return &editorViewportWindow; }
    [[nodiscard]] GameViewportWindow* GetGameViewportWindow() { return &gameViewportWindow; }

private:
    void RenderSceneTab(SceneSerializer *ss);
    void RenderHierarchyTab(ECSWorld *ecs);
    void RenderEntityNode(entt::entity e, ECSWorld *ecs, entt::entity &toDelete);
    void RenderInspectorTab(ECSWorld *ecs, MaterialManager *materialManager);

    void RenderCreateEntityTab();
    void RenderOpenModelDialog();

    void RefreshAvailableScenes(SceneSerializer &ss);

    std::vector<std::string> GetAvailableObjects();

    static inline void Execute(const char *name, const CommandArgs &args) {
        if (CommandManager::HasCommand(name))
            CommandManager::ExecuteCommand(name, args);
    }

    static inline void ExecuteItem(const char *label, const char *command) {
        if (ImGui::MenuItem(label))
            Execute(command);
    }

    static inline void Execute(const char *name) {
        constexpr CommandArgs args;
        Execute(name, args);
    }

    static inline void Execute(const char *name, const std::string &arg) {
        CommandArgs args;
        args.emplace_back(arg);
        Execute(name, args);
    }
};