module;

#include <memory>

#include <entt/entt.hpp>

export module WFE.UI.UIModule;

import WFE.Core.IModule;
import WFE.Core.ModuleManager;
import WFE.Core.Logger;
import WFE.UI.EditorLayout;
import WFE.UI.ImGuiManager;
import WFE.ECS.ECSWorld;
import WFE.Resource.ResourceModule;
import WFE.Rendering.RenderingModule;

export class UIModule : public IModule
{
    std::unique_ptr<EditorLayout> editorLayout;
    std::unique_ptr<ImGuiManager> imGuiManager;

    ECSWorld* ecs = nullptr;
    entt::entity* mainCameraEntity = nullptr;
    ModuleManager* mm = nullptr;
    bool* _isPlayMode = nullptr;

public:
    UIModule(ECSWorld* ecs, entt::entity* cameraEntity, bool* isPlayMode, ModuleManager* mm)
        : ecs(ecs), mainCameraEntity(cameraEntity), _isPlayMode(isPlayMode), mm(mm)
    {
        if (!ecs)
            Logger::Log(LogLevel::ERROR, "RenderingModule: ecs is null!");
    }

    bool Initialize() override
    {
        try 
        {
            editorLayout = std::make_unique<EditorLayout>();
            imGuiManager = std::make_unique<ImGuiManager>();
            
            isInitialized = true;

            return true;
        } 
        catch (const std::exception& e) 
        {
            isInitialized = true;

            return false;
        }
    }

    void Update(float deltaTime) override
    {}

    void RenderUI()
    {
        editorLayout->RenderEditor(
            ecs, 
            *mainCameraEntity,
            mm->GetModule<RenderingModule>("Rendering")->GetRenderer(), 
            mm->GetModule<ResourceModule>("Resource")->GetShaderManager(), 
            mm->GetModule<ResourceModule>("Resource")->GetMaterialManager(),
            _isPlayMode
        );
    }

    void Shutdown() override 
    {
        imGuiManager->Shutdown();

        imGuiManager.reset();
        editorLayout.reset();

    }

    /// @name IModule interface
    /// @{
    const char* GetName() const override{ return "UI"; }
    int GetPriority() const override { return 100; }
    bool IsRequired() const override { return true; }
    /// }@

    EditorLayout* GetEditorLayout() { return editorLayout.get(); }
    ImGuiManager* GetImGuiManager() { return imGuiManager.get(); }
};