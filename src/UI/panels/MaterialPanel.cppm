module;

#include <imgui.h>
#include <entt.hpp>

#include <string>

export module WFE.UI.Panels.MaterialPanel;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Resource.Material.MaterialManager;
import WFE.Core.Logger;

export class MaterialPanel
{
private:
    std::string selectedMaterial = "";

public:
    void Render(ECSWorld* ecs, entt::entity entity, MaterialManager* materialManager)
    {
        if (!ImGui::CollapsingHeader("Material")) return;
        
        if (ecs->HasComponent<MaterialComponent>(entity))
        {
            auto& matComp = ecs->GetComponent<MaterialComponent>(entity);
            ImGui::BulletText("Current: %s", 
                matComp.material ? matComp.material->GetName().c_str() : "None");
            ImGui::Separator();
        }

        ImGui::Text("Assign Material:");
        
        if (ImGui::BeginCombo("##MaterialSelect", selectedMaterial.c_str(), 0))
        {
            for (const auto& name : materialManager->GetMaterialNames())
            {
                bool is_selected = (selectedMaterial == name);
                if (ImGui::Selectable(name.c_str(), is_selected))
                {
                    selectedMaterial = name;
                    AssignMaterial(ecs, entity, materialManager, name);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Remove Material", ImVec2(-1, 0)))
            RemoveMaterial(ecs, entity);
    }

private:
    void AssignMaterial(ECSWorld* ecs, entt::entity entity, 
                       MaterialManager* materialManager, const std::string& name)
    {
        auto material = materialManager->GetMaterial(name);
        
        if (ecs->HasComponent<MaterialComponent>(entity))
        {
            auto& matComp = ecs->GetComponent<MaterialComponent>(entity);
            matComp.material = material;
        }
        else
        {
            ecs->AddComponent<MaterialComponent>(entity, material);
        }
        
        if (ecs->HasComponent<ColorComponent>(entity))
            ecs->RemoveComponent<ColorComponent>(entity);
        
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, 
            "Material '" + name + "' assigned to entity");
    }
    
    void RemoveMaterial(ECSWorld* ecs, entt::entity entity)
    {
        if (ecs->HasComponent<MaterialComponent>(entity))
        {
            ecs->RemoveComponent<MaterialComponent>(entity);
            Logger::Log(LogLevel::INFO, LogCategory::RENDERING, 
                "Material removed from entity");
        }
    }
};