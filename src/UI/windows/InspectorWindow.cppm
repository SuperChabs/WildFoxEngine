module;

#include <imgui.h>
#include <entt.hpp>
#include <glm/glm.hpp>

#include <string>

export module WildFoxEngine.UI.Windows.InspectorWindow;

import WildFoxEngine.ECS.ECSWorld;
import WildFoxEngine.ECS.Components;
import WildFoxEngine.Resource.Material.MaterialManager;
import WildFoxEngine.Resource.Shader.ShaderManager;
import WildFoxEngine.Rendering.Light;

import WildFoxEngine.UI.Panels.TagPanel;
import WildFoxEngine.UI.Panels.TransformPanel;
import WildFoxEngine.UI.Panels.RotationPanel;
import WildFoxEngine.UI.Panels.MaterialPanel;
import WildFoxEngine.UI.Panels.LightPanel;
import WildFoxEngine.UI.Panels.IconPanel;

export class InspectorWindow 
{
private:
    bool isOpen = true;

    TagPanel tagPanel;
    TransformPanel transformPanel;
    RotationPanel rotationPanel;
    MaterialPanel materialPanel;
    LightPanel lightPanel;
    IconPanel iconPanel;

public:
    void Render(ECSWorld* ecs, entt::entity selectedEntity, 
                MaterialManager* materialManager,
                ShaderManager* shaderManager = nullptr,
                ShaderObj* selectedShader = nullptr)
    {
        if (!isOpen) return;
        
        ImGui::SetNextWindowPos(ImVec2(1110, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 660), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Inspector", &isOpen);
        
        if (selectedEntity != entt::null && ecs->IsValid(selectedEntity))
        {
            RenderEntityInspector(ecs, selectedEntity, materialManager);
        }
        else if (selectedShader != nullptr && shaderManager != nullptr)
        {
            RenderShaderInspector(selectedShader, shaderManager);
        }
        else
        {
            RenderEmptyState();
        }
        
        ImGui::End();
    }
    
    bool IsOpen() const { return isOpen; }
    void SetOpen(bool open) { isOpen = open; }

private:
    void RenderEntityInspector(ECSWorld* ecs, entt::entity entity, 
                               MaterialManager* materialManager)
    {
        ImGui::Text("Entity");
        ImGui::Separator();
        
        tagPanel.Render(ecs, entity);
        
        ImGui::Spacing();
        ImGui::Separator();
        
        transformPanel.Render(ecs, entity);
        rotationPanel.Render(ecs, entity);
        materialPanel.Render(ecs, entity, materialManager);
        lightPanel.Render(ecs, entity);
        iconPanel.Render(ecs, entity); 
    }
    
    void RenderShaderInspector(ShaderObj* shader, ShaderManager* shaderManager)
    {
        if (shader == nullptr || shader->name.empty()) return;
        if (!shaderManager->IsShaderValid(shader->name)) return;
        
        ImGui::Text("Shader: %s", shader->name.c_str());
        ImGui::Separator();
        
        if (ImGui::Button("Reload Shader", ImVec2(-1, 0)))
            shaderManager->Reload(shader->name);
    }
    
    void RenderEmptyState()
    {
        ImGui::TextDisabled("Nothing selected");
        ImGui::Separator();
        ImGui::TextWrapped("Select an object from Hierarchy");
    }
};