module;

#include <imgui.h>
#include <entt.hpp>

export module WFE.UI.Panels.IconPanel;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class IconPanel
{
public:
    void Render(ECSWorld* ecs, entt::entity entity)
    {
        if (!ecs->HasComponent<IconComponent>(entity)) return;
        if (!ImGui::CollapsingHeader("Icon Settings")) return;
        
        auto& icon = ecs->GetComponent<IconComponent>(entity);
        
        ImGui::Text("Icon Texture Path:");
        ImGui::TextWrapped("%s", icon.iconTexturePath.c_str());
        
        ImGui::Spacing();
        
        ImGui::Text("Scale");
        ImGui::DragFloat("##IconScale", &icon.scale, 0.01f, 0.1f, 5.0f);
        
        ImGui::Spacing();
        
        ImGui::Checkbox("Billboard Mode", &icon.billboardMode);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Always face the camera");
        
        ImGui::Spacing();
        
        if (ImGui::Button("Reload Texture", ImVec2(-1, 0)))
            icon.textureID = 0; 
    }
};