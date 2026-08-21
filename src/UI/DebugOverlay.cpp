#include "DebugOverlay.h"

#include <fstream>

#include <glm/glm.hpp>

#include "core/logging/Logger.h"

namespace fs = std::filesystem;
using namespace entt::literals;

DebugOverlay::DebugOverlay() {
    editorFramebuffer = std::make_unique<Framebuffer>(1, 1);
    gameFramebuffer   = std::make_unique<Framebuffer>(1, 1);
}

void DebugOverlay::Render(ECSWorld *ecs, MaterialManager *materialManager, SceneSerializer * ss, EditorCamera &editorCamera) {
    if (!visible || !ecs) return;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (!viewport)
    {
        Logger::Log(LogLevel::ERROR, "Main viewport is NULL!");
        return;
    }
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    if (ImGui::Begin("Scene"))
        RenderSceneTab(ss);
    ImGui::End();

    if (ImGui::Begin("Hierarchy"))
        RenderHierarchyTab(ecs);
    ImGui::End();

    if (ImGui::Begin("Inspector"))
        RenderInspectorTab(ecs, materialManager);
    ImGui::End();

    if (ImGui::BeginMenuBar()) {
        RenderCreateEntityTab();
        ImGui::EndMenuBar();
    }

    editorViewportWindow.Render(*ecs, m_selected, editorFramebuffer.get(), editorCamera.camera,
        editorCamera.transform, editorCamera.orientation);

    gameViewportWindow.Render(gameFramebuffer.get());

    RenderOpenModelDialog();

    ImGui::End();
}

void DebugOverlay::RenderSceneTab(SceneSerializer *ss) {
    if (!m_scenesLoaded)
        RefreshAvailableScenes(*ss);

    std::string sceneToDelete;

    ImGui::Spacing();
    ImGui::Text("Scene file:");
    ImGui::SetNextItemWidth(-1.f);
    ImGui::InputText("##scenepath", m_scenePathBuf, sizeof(m_scenePathBuf));
    ImGui::Spacing();

    ImGui::SameLine();

    if (ImGui::Button("Save", {-1.f, 0}))
        Execute("onSaveScene", m_scenePathBuf);

    ImGui::Spacing();

    if (ImGui::Button("New scene", {-1.f, 0}))
        Execute("onNewScene");

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Toggle Debug")) {
        Logger::Log(LogLevel::INFO,
                    CommandManager::HasCommand("onDebugPauseToggle")
                        ? "Command found, executing..."
                        : "Command NOT registered!");
        Execute("onDebugPauseToggle");
    }

    if (ImGui::Button("Play"))
        Execute("onPlayGame");

    ImGui::SameLine();
    if (ImGui::Button("Stop"))
        Execute("onStopGame");

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Available Scenes");
    ImGui::SameLine();
    if (ImGui::Button("Refresh"))
        RefreshAvailableScenes(*ss);

    ImGui::BeginChild("SceneList", ImVec2(0, 200), true);
    for (int i = 0; i < static_cast<int>(m_availableScenes.size()); i++) {
        const bool isSelected = (m_selectedScene == i);
        if (ImGui::Selectable(m_availableScenes[i].c_str(), isSelected))
            m_selectedScene = i;

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            Execute("onLoadScene", m_availableScenes[i].c_str());

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete Scene"))
                sceneToDelete = m_availableScenes[i].c_str();
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();

    if (!sceneToDelete.empty()) {
        m_pendingDeleteScene = sceneToDelete;
        ImGui::OpenPopup("Confirm Delete Scene");
    }

    if (ImGui::BeginPopupModal("Confirm Delete Scene", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::Text("Delete Scene '%s'?", m_pendingDeleteScene.c_str());
        ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "This cannot be undone!");
        ImGui::Spacing();

        if (ImGui::Button("Delete", {120.f, 0})) {
            if (ss->DeleteScene(m_pendingDeleteScene))
                Logger::Log(LogLevel::INFO, "Deleted scene: " + m_pendingDeleteScene);
            else
                Logger::Log(LogLevel::INFO, "Failed to delete scene: 2" + m_pendingDeleteScene);

            RefreshAvailableScenes(*ss);
            m_pendingDeleteScene.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", {120.f, 0})) {
            m_pendingDeleteScene.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void DebugOverlay::RenderHierarchyTab(ECSWorld *ecs) {
    ImGui::Spacing();
    ImGui::Text("%zu entities", ecs->GetEntityCount());
    ImGui::Separator();

    entt::entity toDelete = entt::null;
    std::vector<entt::entity> roots;
    static bool sortAlphabetically = false;

    ecs->Each<TagComponent, IDComponent>(
        [&](entt::entity e, TagComponent &tag, IDComponent &) {
            if (ecs->HasComponent<HierarchyComponent>(e))
                if (ecs->GetComponent<HierarchyComponent>(e).HasParent())
                    return;

            roots.push_back(e);
        }
    );

    if (ImGui::Button("Sort")) {
        sortAlphabetically = !sortAlphabetically;
    }

    if (sortAlphabetically) {
        std::sort(roots.begin(), roots.end(),
            [&](entt::entity a, entt::entity b) {
                auto &tagA = ecs->GetComponent<TagComponent>(a);
                auto &tagB = ecs->GetComponent<TagComponent>(b);
                return tagA.name < tagB.name;
            });
    }

    for (auto e : roots) {
        RenderEntityNode(e, ecs, toDelete);
    }

    if (toDelete != entt::null) {
        if (m_selected == toDelete) m_selected = entt::null;
        else ecs->DestroyEntity(toDelete);
    }
}

void DebugOverlay::RenderEntityNode(entt::entity e, ECSWorld *ecs, entt::entity &toDelete) {
    if (!ecs->IsValid(e)) return;

    auto &tag = ecs->GetComponent<TagComponent>(e);

    bool hasChildren = ecs->HasComponent<HierarchyComponent>(e) &&
                       ecs->GetComponent<HierarchyComponent>(e).HasChildren();

    ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!hasChildren)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (m_selected == e)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool open = ImGui::TreeNodeEx((void *) (intptr_t) e, flags, "%s", tag.name.c_str());

    if (ImGui::IsItemClicked())
        m_selected = e;

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) toDelete = e;
        if (ImGui::MenuItem("Select")) m_selected = e;
        ImGui::EndPopup();
    }

    if (open && hasChildren) {
        for (auto child: ecs->GetComponent<HierarchyComponent>(e).children)
            RenderEntityNode(child, ecs, toDelete);
        ImGui::TreePop();
    }
}

void DebugOverlay::RenderInspectorTab(ECSWorld *ecs, MaterialManager *materialManager) {
    ImGui::Spacing();

    if (m_selected == entt::null || !ecs->IsValid(m_selected)) {
        ImGui::TextDisabled("Nothing selected");
        ImGui::TextWrapped("Pick an entity in Hierarchy tab");
        return;
    }

    if (ecs->HasComponent<TagComponent>(m_selected)) {
        auto &tag = ecs->GetComponent<TagComponent>(m_selected);
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
    if (ecs->HasComponent<CameraComponent>(m_selected) && ecs->HasComponent<CameraOrientationComponent>(m_selected))
        cameraPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<AudioSourceComponent>(m_selected) || ecs->HasComponent<AudioListenerComponent>(m_selected))
        audioPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<ScriptComponent>(m_selected))
        scriptPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<IconComponent>(m_selected))
        iconPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<RigidBodyComponent>(m_selected))
        rigidPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<ColliderComponent>(m_selected))
        colliderPanel.Render(ecs, m_selected);

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::Button("Add Component", {-1.f, 0}))
        ImGui::OpenPopup("##addcomp");

    if (ImGui::BeginPopup("##addcomp")) {
        if (!ecs->HasComponent<ScriptComponent>(m_selected))
            if (ImGui::MenuItem("Script"))
                ecs->AddComponent<ScriptComponent>(m_selected);

        if (!ecs->HasComponent<AudioSourceComponent>(m_selected))
            if (ImGui::MenuItem("Audio Source"))
                ecs->AddComponent<AudioSourceComponent>(m_selected);

        if (!ecs->HasComponent<AudioListenerComponent>(m_selected))
            if (ImGui::MenuItem("Audio Listener"))
                ecs->AddComponent<AudioListenerComponent>(m_selected);

        if (!ecs->HasComponent<MaterialComponent>(m_selected))
            if (ImGui::MenuItem("Material"))
                ecs->AddComponent<MaterialComponent>(m_selected);

        if (!ecs->HasComponent<RigidBodyComponent>(m_selected)) {
            if (ImGui::MenuItem("RigidBody")) {
                RigidBodyComponent rb;
                rb.inv_mass = 0.0f;
                rb.velocity = glm::vec3(0.0f);
                rb.angular_velocity = glm::vec3(0.0f);
                rb.inertia = glm::vec3(1.0f);
                rb.force_accum = glm::vec3(0.0f);
                rb.torque_accum = glm::vec3(0.0f);
                ecs->AddComponent<RigidBodyComponent>(m_selected, rb);
            }
        }

        if (!ecs->HasComponent<ColliderComponent>(m_selected)) {
            if (ImGui::MenuItem("Collider")) {
                ColliderComponent cl;
                AABB aabb;
                aabb.min = glm::vec3(-0.5f, -0.5f, -0.5f);
                aabb.max = glm::vec3(0.5f, 0.5f, 0.5f);
                cl.shape = aabb;
                ecs->AddComponent<ColliderComponent>(m_selected, cl);
            }
        }

        ImGui::EndPopup();
    }

    if (ImGui::Button("Remove Component", {-1.f, 0}))
        ImGui::OpenPopup("##removecomp");

    if (ImGui::BeginPopup("##removecomp")) {
        if (ecs->HasComponent<ScriptComponent>(m_selected))
            if (ImGui::MenuItem("Script"))
                ecs->RemoveComponent<ScriptComponent>(m_selected);

        if (ecs->HasComponent<AudioSourceComponent>(m_selected))
            if (ImGui::MenuItem("Audio Source"))
                ecs->RemoveComponent<AudioSourceComponent>(m_selected);

        if (ecs->HasComponent<AudioListenerComponent>(m_selected))
            if (ImGui::MenuItem("Audio Listener"))
                ecs->RemoveComponent<AudioListenerComponent>(m_selected);

        if (ecs->HasComponent<RigidBodyComponent>(m_selected))
            if (ImGui::MenuItem("RigidBody"))
                ecs->RemoveComponent<RigidBodyComponent>(m_selected);

        if (ecs->HasComponent<ColliderComponent>(m_selected))
            if (ImGui::MenuItem("Collider"))
                ecs->RemoveComponent<ColliderComponent>(m_selected);

        ImGui::EndPopup();
    }

}

void DebugOverlay::RenderCreateEntityTab() {
    ImGui::Spacing();

    if (ImGui::BeginMenu("Mesh")) {
        ExecuteItem("Cube", "onCreateCube");
        ExecuteItem("Cone", "onCreateCone");
        ExecuteItem("Cylinder", "onCreateCylinder");
        ExecuteItem("Plane", "onCreatePlane");
        ExecuteItem("Sphere", "onCreateSphere");
        ExecuteItem("Torus", "onCreateTorus");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Light")) {
        ExecuteItem("Directional", "onCreateDirectionalLight");
        ExecuteItem("Point", "onCreatePointLight");
        ExecuteItem("Spot", "onCreateSpotLight");
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Camera"))
        Execute("onCreateCamera");

    if (ImGui::MenuItem("AABB hitbox"))
        Execute("onCreateAABBHitbox");

    if (ImGui::MenuItem("Open Model"))
        m_showOpenModelDialog = true;
}

void DebugOverlay::RenderOpenModelDialog() {
    if (!m_showOpenModelDialog) return;

    m_availableObjects = GetAvailableObjects();

    ImGui::OpenPopup("Open Model");

    if (ImGui::BeginPopupModal( "Open Model", &m_showOpenModelDialog, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::BeginChild("ModelList", ImVec2(250, 200), true);
        for (int i = 0; i < static_cast<int>(m_availableObjects.size()); i++) {
            const bool isSelected = (m_selectedObject == i);

            std::string displayName = fs::path(m_availableObjects[i]).filename().string();

            ImGui::PushID(i);

            if (ImGui::Selectable(displayName.c_str(), isSelected))
                m_selectedObject = i;

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                std::string path = m_availableObjects[i];

                if (!path.empty()) {
                    Logger::Log(LogLevel::INFO, "Opening model: " + path);
                    Execute("onLoadModel", path);

                    m_showOpenModelDialog = false;
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::PopID();
        }
        ImGui::EndChild();

        if (ImGui::Button("Cancel")) {
            m_showOpenModelDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void DebugOverlay::RefreshAvailableScenes(SceneSerializer &ss) {
    m_availableScenes = ss.GetAvailableScenes();
    std::sort(m_availableScenes.begin(), m_availableScenes.end());
    m_selectedScene = -1;
    m_scenesLoaded = true;
}

std::vector<std::string> DebugOverlay::GetAvailableObjects() {
    std::string folderPath = "../assets/objects/";
    std::vector<std::string> filePaths;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(folderPath))
            if (entry.is_regular_file())
                filePaths.push_back(entry.path().string());
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR, e.what());
    }

    return filePaths;
}
