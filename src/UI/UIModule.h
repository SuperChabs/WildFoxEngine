#pragma once

#include <memory>


#include <entt/entt.hpp>
#include <GLFW/glfw3.h>

#include "core/IModule.h"
#include "core/ModuleManager.h"
#include "UI/ImGuiManager.h"
#include "ECS/World.h"
#include "scene/SceneManager.h"

class UIModule : public IModule {
    std::unique_ptr<ImGuiManager> imGuiManager;

    ECSWorld *ecs = nullptr;
    entt::entity *mainCameraEntity = nullptr;
    ModuleManager *mm = nullptr;
    GLFWwindow *window = nullptr;
    SceneManager *m_sceneManager = nullptr;

public:
    UIModule(ECSWorld *ecs, entt::entity *cameraEntity, SceneManager *sceneManager, ModuleManager *mm,
             GLFWwindow *window);

    bool Initialize() override;

    void Update(float deltaTime) override;

    void RenderUI();

    void Shutdown() override;

    /// @name IModule interface
        /// @{
    const char *GetName() const override;

    int GetPriority() const override;

    bool IsRequired() const override;

    /// }@

    //EditorLayout* GetEditorLayout() { return editorLayout.get(); }
    ImGuiManager *GetImGuiManager();
};