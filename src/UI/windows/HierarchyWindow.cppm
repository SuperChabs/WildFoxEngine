module;

#include <imgui.h>
#include <entt.hpp>
#include <glm/glm.hpp>
#include "entity/fwd.hpp"

#include <cstdint>
#include <string>
#include <cstring>

export module WFE.UI.Windows.HierarchyWindow;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

import WFE.Resource.Shader.ShaderManager;
import WFE.Resource.Material.MaterialManager;

export class HierarchyWindow
{
private:
    bool isOpen = true;
    
    entt::entity selectedEntity = entt::null;
    ShaderObj* selectedShader = nullptr;
    std::string selectedMaterial = "default_white";

public:
    void Render(ECSWorld* ecs, ShaderManager* shaderManager, 
                MaterialManager* materialManager)
    {
        if (!isOpen) return;
        
        ImGui::SetNextWindowPos({10, 170}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 400}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Hierarchy", &isOpen);

        RenderObjectsSection(ecs);
        RenderShadersSection(shaderManager);
        RenderMaterialsSection(materialManager);

        ImGui::End();
    }
    
    bool IsOpen() const { return isOpen; }
    void SetOpen(bool open) { isOpen = open; }
    
    entt::entity GetSelectedEntity() const { return selectedEntity; }
    ShaderObj* GetSelectedShader() const { return selectedShader; }
    std::string GetSelectedMaterial() const { return selectedMaterial; }
    
    void SetSelectedEntity(entt::entity entity) { selectedEntity = entity; }

private:
    void RenderObjectsSection(ECSWorld* ecs)
    {
        if (!ImGui::CollapsingHeader("Objects")) return;
        
        ImGui::Text("Entities: %zu", ecs->GetEntityCount());
        ImGui::Separator();

        entt::entity entityToDelete = entt::null;

        ecs->Each<TagComponent, IDComponent>(
            [&](entt::entity entity, TagComponent& tag, IDComponent& id) 
        {
            // ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | 
            //                           ImGuiTreeNodeFlags_NoTreePushOnOpen;

            // if (selectedEntity == entity)
            //     flags |= ImGuiTreeNodeFlags_Selected;

            // std::string label = tag.name + " [" + std::to_string(id.id) + "]";

            // ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", label.c_str());

            // if (ImGui::IsItemClicked())
            //     selectedEntity = entity;

            // if (ImGui::BeginPopupContextItem())
            // {
            //     if (ImGui::MenuItem("Delete"))
            //         entityToDelete = entity;

            //     ImGui::EndPopup();
            // }

            if (ecs->HasComponent<HierarchyComponent>(entity))
            {
                auto& hierarchy = ecs->GetComponent<HierarchyComponent>(entity);
                if (hierarchy.HasParent())
                    return;
            }
            
            RenderEntityTree(entity, ecs, entityToDelete);
        });

        if (entityToDelete != entt::null)
        {
            ecs->DestroyEntity(entityToDelete);
            if (selectedEntity == entityToDelete)
                selectedEntity = entt::null;
        }
    }

    void RenderEntityTree(entt::entity entity, ECSWorld* ecs, entt::entity& entityToDelete)
    {
        if (!ecs->IsValid(entity))
            return;

        auto& tag = ecs->GetComponent<TagComponent>(entity);
        auto& id = ecs->GetComponent<IDComponent>(entity);

        bool hasChildren = false;
        if (ecs->HasComponent<HierarchyComponent>(entity))
        {
            auto& hierarchy = ecs->GetComponent<HierarchyComponent>(entity);
            hasChildren = hierarchy.HasCildren();
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;

        if (!hasChildren)
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (selectedEntity == entity)
            flags |= ImGuiTreeNodeFlags_Selected;

        std::string label = tag.name;

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", label.c_str());

        if (ImGui::IsItemClicked())
            selectedEntity = entity;

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
                entityToDelete = entity;

            ImGui::Separator();

            if (ImGui::MenuItem("Create Child"))
            {
                auto child = ecs->CreateEntity("New Entity");
                ecs->SetParent(child, entity);
            }

            ImGui::EndPopup();
        }

        if (nodeOpen && hasChildren)
        {
            auto& hierarchy = ecs->GetComponent<HierarchyComponent>(entity);
            for (auto child : hierarchy.children)
                RenderEntityTree(child, ecs, entityToDelete);

            ImGui::TreePop();
        }
    }
    
    void RenderShadersSection(ShaderManager* shaderManager)
    {
        if (!ImGui::CollapsingHeader("Shaders")) return;
        
        ImGui::Text("Shaders: %zu", shaderManager->GetCount());
        ImGui::Separator();

        for (auto& [name, shader] : shaderManager->GetShaderMap())
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | 
                                      ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (selectedShader == shader.get())
                flags |= ImGuiTreeNodeFlags_Selected;

            std::string label = name + " [" + std::to_string(shader->ID) + "]";

            ImGui::TreeNodeEx((void*)(intptr_t)shader.get(), flags, "%s", label.c_str());

            if (ImGui::IsItemClicked())
                selectedShader = shader.get();
        }
    }
    
    void RenderMaterialsSection(MaterialManager* materialManager)
    {
        if (!ImGui::CollapsingHeader("Materials")) return;
        
        ImGui::Text("Materials: %zu", materialManager->GetMaterialsCount());
        ImGui::Separator();

        RenderCreateMaterialButton(materialManager);
        
        ImGui::Spacing();
        
        RenderMaterialsList(materialManager);
    }
    
    void RenderCreateMaterialButton(MaterialManager* materialManager)
    {
        if (ImGui::Button("+ Add Color Material", ImVec2(-1, 0)))
            ImGui::OpenPopup("AddColorMaterial");

        if (ImGui::BeginPopupModal("AddColorMaterial", nullptr, 
                                   ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char matNameBuf[128] = "";
            static float matColor[3] = {1.0f, 1.0f, 1.0f};

            ImGui::InputText("Material Name##color", matNameBuf, sizeof(matNameBuf));
            ImGui::ColorEdit3("Color##material", matColor);

            if (ImGui::Button("Create", ImVec2(120, 0)))
            {
                if (strlen(matNameBuf) > 0)
                {
                    materialManager->CreateColorMaterial(
                        std::string(matNameBuf),
                        glm::vec3(matColor[0], matColor[1], matColor[2]));
                    
                    memset(matNameBuf, 0, sizeof(matNameBuf));
                    matColor[0] = matColor[1] = matColor[2] = 1.0f;
                    ImGui::CloseCurrentPopup();
                }
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                memset(matNameBuf, 0, sizeof(matNameBuf));
                ImGui::CloseCurrentPopup();
            }

            ImGui::Text("(In development!)");

            ImGui::EndPopup();
        }
    }
    
    void RenderMaterialsList(MaterialManager* materialManager)
    {
        for (const auto& [name, material] : materialManager->GetMaterialsMap())
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | 
                                      ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (selectedMaterial == name)
                flags |= ImGuiTreeNodeFlags_Selected;

            std::string label = name;

            ImGui::TreeNodeEx((void*)(intptr_t)material.get(), flags, "%s", label.c_str());

            if (ImGui::IsItemClicked())
                selectedMaterial = name;
        }
    }
};