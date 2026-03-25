module;

#include <imgui.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <cstring>
#include <functional>

export module WFE.UI.DebugOverlay;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Light;
import WFE.Resource.Material.MaterialManager;
import WFE.Core.CommandManager;
import WFE.Core.Logger;

import WFE.UI.Panels.TagPanel;
import WFE.UI.Panels.TransformPanel;
import WFE.UI.Panels.MaterialPanel;
import WFE.UI.Panels.LightPanel;
import WFE.UI.Panels.CameraPanel;
import WFE.UI.Panels.ScriptPanel;
import WFE.UI.Panels.IconPanel;

export struct SceneBuilderCallbacks
{
    std::function<void(const std::string&)> onLoadScene;
    std::function<void(const std::string&)> onSaveScene;
    std::function<void()>                   onNewScene;  
    std::function<void(entt::entity)>       onDeleteEntity;
    std::function<entt::entity()>           onCreateEntity;
    std::function<void(entt::entity, const std::string&)> onAttachScript;
};

export class DebugOverlay
{
public:
    bool visible = true;

    void Render(ECSWorld* ecs, entt::entity cameraEntity,
                const SceneBuilderCallbacks& cb, MaterialManager* materialManager)
    {
        if (!visible || !ecs) return;

        ImGui::SetNextWindowPos({10.f, 10.f}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({320.f, 700.f}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.92f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
        if (!ImGui::Begin("Scene Builder", &visible, flags))
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginTabBar("##sbo_tabs"))
        {
            if (ImGui::BeginTabItem("Scene"))
            {
                RenderSceneTab(ecs, cb);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Hierarchy"))
            {
                RenderHierarchyTab(ecs, cb);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Inspector"))
            {
                RenderInspectorTab(ecs, materialManager);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Create"))
            {
                RenderCreateEntityTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        RenderOpenModelDialog();
        RenderOpenMaterialDialog();

        ImGui::End();
    }

private:
    entt::entity m_selected = entt::null;

    char m_scenePathBuf[512]  = "scene.json";
    char m_scriptPathBuf[256] = "";
    char m_entityNameBuf[128] = "New Entity";

    char m_modelPath[512]{};

    char m_name[128]{};
    char m_diffusePath[512]{};
    char m_specularPath[512]{};
    char m_normalPath[512]{};
    char m_heightPath[512]{};

    bool m_showOpenModelDialog = false;
    bool m_showOpenMaterialDialog = false;

    TagPanel tagPanel;
    TransformPanel transformPanel;
    MaterialPanel materialPanel;
    LightPanel lightPanel;
    CameraPanel cameraPanel;
    ScriptPanel scriptPanel;
    IconPanel iconPanel;
    
    void RenderSceneTab(ECSWorld* ecs, const SceneBuilderCallbacks& cb)
    {
        ImGui::Spacing();
        ImGui::Text("Scene file:");
        ImGui::SetNextItemWidth(-1.f);
        ImGui::InputText("##scenepath", m_scenePathBuf, sizeof(m_scenePathBuf));
        ImGui::Spacing();

        float btnW = (ImGui::GetContentRegionAvail().x - 8.f) / 2.f;

        if (ImGui::Button("Load", {btnW, 0}))
            Execute("onLoadScene", m_scenePathBuf);

        ImGui::SameLine();

        if (ImGui::Button("Save", {btnW, 0}))
            Execute("onSaveScene", m_scenePathBuf);

        ImGui::Spacing();

        if (ImGui::Button("New scene", {-1.f, 0}) && cb.onNewScene)
            Execute("onNewScene");

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Toggle Debug"))
        {
            Logger::Log(LogLevel::INFO, 
                CommandManager::HasCommand("onDebugPauseToggle") 
                    ? "Command found, executing..." 
                    : "Command NOT registered!");
            Execute("onDebugPauseToggle");
        }
    }

    void RenderHierarchyTab(ECSWorld* ecs, const SceneBuilderCallbacks& cb)
    {
        ImGui::Spacing();
        ImGui::Text("%zu entities", ecs->GetEntityCount());
        ImGui::Separator();

        entt::entity toDelete = entt::null;

        ecs->Each<TagComponent, IDComponent>(
            [&](entt::entity e, TagComponent& tag, IDComponent&)
            {
                if (ecs->HasComponent<HierarchyComponent>(e))
                    if (ecs->GetComponent<HierarchyComponent>(e).HasParent())
                        return;

                RenderEntityNode(e, ecs, toDelete);
            }
        );

        if (toDelete != entt::null)
        {
            if (m_selected == toDelete) m_selected = entt::null;
            if (cb.onDeleteEntity) cb.onDeleteEntity(toDelete);
            else ecs->DestroyEntity(toDelete);
        }
    }

    void RenderEntityNode(entt::entity e, ECSWorld* ecs, entt::entity& toDelete)
    {
        if (!ecs->IsValid(e)) return;

        auto& tag = ecs->GetComponent<TagComponent>(e);

        bool hasChildren = ecs->HasComponent<HierarchyComponent>(e) &&
                           ecs->GetComponent<HierarchyComponent>(e).HasCildren();

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth;

        if (!hasChildren)
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (m_selected == e)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx((void*)(intptr_t)e, flags, "%s", tag.name.c_str());

        if (ImGui::IsItemClicked())
            m_selected = e;

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))      toDelete = e;
            if (ImGui::MenuItem("Select"))      m_selected = e;
            ImGui::EndPopup();
        }

        if (open && hasChildren)
        {
            for (auto child : ecs->GetComponent<HierarchyComponent>(e).children)
                RenderEntityNode(child, ecs, toDelete);
            ImGui::TreePop();
        }
    }

    void RenderInspectorTab(ECSWorld* ecs, MaterialManager* materialManager)
    {
        ImGui::Spacing();

        if (m_selected == entt::null || !ecs->IsValid(m_selected))
        {
            ImGui::TextDisabled("Nothing selected");
            ImGui::TextWrapped("Pick an entity in Hierarchy tab");
            return;
        }

        if (ecs->HasComponent<TagComponent>(m_selected))
        {
            auto& tag = ecs->GetComponent<TagComponent>(m_selected);
            char buf[256];
            strncpy(buf, tag.name.c_str(), sizeof(buf));
            buf[255] = '\0';
            ImGui::Text("Name:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1.f);
            if (ImGui::InputText("##tag", buf, sizeof(buf)))
                tag.name = buf;
        }

        ImGui::Separator();
        ImGui::Spacing();

        if (ecs->HasComponent<TransformComponent>(m_selected))
            transformPanel.Render(ecs, m_selected);
        if (ecs->HasComponent<MaterialComponent>(m_selected))
            materialPanel.Render(ecs, m_selected, materialManager);
        if (ecs->HasComponent<LightComponent>(m_selected))
            lightPanel.Render(ecs, m_selected);
        if (ecs->HasComponent<CameraComponent>(m_selected) &&
            ecs->HasComponent<CameraOrientationComponent>(m_selected))
            cameraPanel.Render(ecs, m_selected);
        if (ecs->HasComponent<ScriptComponent>(m_selected))
            scriptPanel.Render(ecs, m_selected);
        if (ecs->HasComponent<IconComponent>(m_selected))
            iconPanel.Render(ecs, m_selected);
            

        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::Button("+ Add Component", {-1.f, 0}))
            ImGui::OpenPopup("##addcomp");

        if (ImGui::BeginPopup("##addcomp"))
        {
            if (!ecs->HasComponent<ScriptComponent>(m_selected))
                if (ImGui::MenuItem("Script"))
                    ecs->AddComponent<ScriptComponent>(m_selected);

            if (!ecs->HasComponent<MaterialComponent>(m_selected))
                if (ImGui::MenuItem("Material"))
                    ecs->AddComponent<MaterialComponent>(m_selected);

            ImGui::EndPopup();
        }
    }

    void RenderCreateEntityTab()
    {
        ImGui::Spacing();

        if (ImGui::BeginMenu("Mesh"))
        {
            ExecuteItem("Cube", "onCreateCube");
            ExecuteItem("Cone", "onCreateCone");
            ExecuteItem("Cylinder", "onCreateCylinder");
            ExecuteItem("Plane", "onCreatePlane");
            ExecuteItem("Sphere", "onCreateSphere");
            ExecuteItem("Torus", "onCreateTorus");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light"))
        {
            ExecuteItem("Directional", "onCreateDirectionalLight");
            ExecuteItem("Point", "onCreatePointLight");
            ExecuteItem("Spot", "onCreateSpotLight");
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Camera"))
            Execute("onCreateCamera");

        if (ImGui::MenuItem("Open Model"))
            m_showOpenModelDialog = true;

        if (ImGui::MenuItem("New Material"))
            m_showOpenMaterialDialog = true;
    }

    void RenderOpenModelDialog()
    {
        if (!m_showOpenModelDialog) return;

        ImGui::OpenPopup("Open Model");

        if (ImGui::BeginPopupModal(
                "Open Model",
                &m_showOpenModelDialog,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Path to model file:");
            ImGui::InputTextWithHint(
                "##modelpath",
                "/home/user/model.obj",
                m_modelPath,
                sizeof(m_modelPath));

            ImGui::Spacing();

            if (ImGui::Button("Open"))
            {
                std::string path = m_modelPath;

                if (!path.empty())
                {
                    Logger::Log(LogLevel::INFO,
                        "Opening model: " + path);
                    Execute("onLoadModel", path);
                }

                m_showOpenModelDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                m_showOpenModelDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void RenderOpenMaterialDialog()
    {
        if (!m_showOpenMaterialDialog) return;

        ImGui::OpenPopup("NewMaterial");

        if (ImGui::BeginPopupModal(
            "NewMaterial", 
            &m_showOpenMaterialDialog, 
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Name:");
            ImGui::InputTextWithHint(
                "##name",
                "material",
                m_name,
                sizeof(m_name));

            ImGui::Text("Diff path:");
            ImGui::InputTextWithHint(
                "##diffpath",
                "/home/user/texture.png",
                m_diffusePath,
                sizeof(m_diffusePath));

            ImGui::Text("Spec path:");
            ImGui::InputTextWithHint(
                "##specpath",
                "/home/user/texture.png",
                m_specularPath,
                sizeof(m_specularPath));

            ImGui::Text("Norm path:");
            ImGui::InputTextWithHint(
                "##normpath",
                "/home/user/texture.png",
                m_normalPath,
                sizeof(m_normalPath));

            ImGui::Text("Height path:");
            ImGui::InputTextWithHint(
                "##heightpath",
                "/home/user/texture.png",
                m_heightPath,
                sizeof(m_heightPath));

            ImGui::Spacing();

            if (ImGui::Button("Open"))
            {
                Execute("onCreateMaterial",
                    {
                        m_name,
                        m_diffusePath,
                        m_specularPath,
                        m_normalPath,
                        m_heightPath
                    }
                );

                m_showOpenMaterialDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                m_showOpenMaterialDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    inline void Execute(const char* name, const CommandArgs& args)
    {
        if (CommandManager::HasCommand(name))
            CommandManager::ExecuteCommand(name, args);
    }

    inline void ExecuteItem(const char* label, const char* command)
    {
        if (ImGui::MenuItem(label))
            Execute(command);
    }

    inline void Execute(const char* name)
    {
        CommandArgs args;
        Execute(name, args);
    }

    inline void Execute(const char* name, const std::string& arg)
    {
        CommandArgs args;
        args.emplace_back(arg);
        Execute(name, args);
    }
};