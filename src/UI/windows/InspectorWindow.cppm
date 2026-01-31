module;

#include <imgui.h>
#include <entt.hpp>
#include <glm/glm.hpp>

#include <string>

export module WFE.UI.Windows.InspectorWindow;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Shader.ShaderManager;
import WFE.Rendering.Light;

import WFE.UI.Panels.TagPanel;
import WFE.UI.Panels.TransformPanel;
import WFE.UI.Panels.RotationPanel;
import WFE.UI.Panels.MaterialPanel;
import WFE.UI.Panels.LightPanel;
import WFE.UI.Panels.IconPanel;

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
            RenderEntityInspector(ecs, selectedEntity, materialManager, selectedEntity);
        else if (selectedShader != nullptr && shaderManager != nullptr)
            RenderShaderInspector(selectedShader, shaderManager);
        else
            RenderEmptyState();
        
        ImGui::End();
    }
    
    bool IsOpen() const { return isOpen; }
    void SetOpen(bool open) { isOpen = open; }

private:
    void RenderEntityInspector(ECSWorld* ecs, entt::entity entity, 
                               MaterialManager* materialManager, entt::entity selectedEntity)
    {
        ImGui::Text("Entity");
        ImGui::Separator();
        
        tagPanel.Render(ecs, entity);
        
        if (ImGui::CollapsingHeader("Hierarchy"))
        {
            entt::entity parent = ecs->GetParent(selectedEntity);
            if (parent != entt::null)
            {
                auto& parentTag = ecs->GetComponent<TagComponent>(parent);
                ImGui::Text("Parent: %s", parentTag.name.c_str());
                
                if (ImGui::Button("Clear Parent"))
                {
                    ecs->ClearParent(selectedEntity);
                }
            }
            else
            {
                ImGui::Text("Parent: None");
            }
            
            ImGui::Spacing();
            
            auto children = ecs->GetChildren(selectedEntity);
            ImGui::Text("Children: %zu", children.size());
            
            if (!children.empty())
            {
                ImGui::Indent();
                for (auto child : children)
                {
                    if (ecs->IsValid(child))
                    {
                        auto& childTag = ecs->GetComponent<TagComponent>(child);
                        ImGui::BulletText("%s", childTag.name.c_str());
                    }
                }
                ImGui::Unindent();
            }
        }

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