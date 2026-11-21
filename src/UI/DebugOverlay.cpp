#include "DebugOverlay.h"

#include <fstream>

#include <glm/glm.hpp>

#include "core/logging/Logger.h"

namespace fs = std::filesystem;
using namespace entt::literals;

DebugOverlay::DebugOverlay() {
    editorFramebuffer = std::make_unique<Framebuffer>(1, 1);
    gameFramebuffer = std::make_unique<Framebuffer>(1, 1);
}

void DebugOverlay::Render(ECSWorld *ecs, MaterialManager *materialManager, SceneSerializer *ss, EditorCamera &editorCamera,
        PhysicsModule *physicsModule, PhysicsDebugRenderSystem *physicsDebug) {
    if (!visible || !ecs) return;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    if (!viewport) {
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
        RenderUpperMenuTab();
        ImGui::EndMenuBar();
    }

    if (ImGui::Begin("Physics"))
        physicsControlPanel.Render(ecs, physicsModule, physicsDebug);
    ImGui::End();

    editorViewportWindow.Render(*ecs, m_selected, editorFramebuffer.get(), editorCamera.camera, editorCamera.transform,
        editorCamera.orientation);

    gameViewportWindow.Render(gameFramebuffer.get());

    RenderOpenModelDialog();
    RenderPrefabsDialog();

    ImGui::End();

    editor.Render();
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

    if (ImGui::Checkbox("VSync", &m_vsync))
        Execute("onSetVSync", CommandArgs{m_vsync});
    ImGui::SameLine();
    ImGui::TextDisabled("%.0f FPS", ImGui::GetIO().Framerate);

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
            Execute("onLoadScene", m_availableScenes[i]);

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete Scene"))
                sceneToDelete = m_availableScenes[i];
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
            Logger::Log(LogLevel::INFO, "Deleted scene: " + m_pendingDeleteScene);

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

    ecs->Each<c_Tag, c_ID>(
        [&](entt::entity e, c_Tag &tag, c_ID &) {
            if (ecs->HasComponent<c_Hierarchy>(e))
                if (ecs->GetComponent<c_Hierarchy>(e).HasParent())
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
                      auto &tagA = ecs->GetComponent<c_Tag>(a);
                      auto &tagB = ecs->GetComponent<c_Tag>(b);
                      return tagA.name < tagB.name;
                  });
    }

    for (auto e: roots) {
        RenderEntityNode(e, ecs, toDelete);
    }

    if (toDelete != entt::null) {
        if (m_selected == toDelete) m_selected = entt::null;
        else ecs->DestroyEntity(toDelete);
    }
}

void DebugOverlay::RenderEntityNode(entt::entity e, ECSWorld *ecs, entt::entity &toDelete) {
    if (!ecs->IsValid(e)) return;

    const auto &tag = ecs->GetComponent<c_Tag>(e);

    const bool hasChildren = ecs->HasComponent<c_Hierarchy>(e) && ecs->GetComponent<c_Hierarchy>(e).HasChildren();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

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
        for (const auto child: ecs->GetComponent<c_Hierarchy>(e).children)
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

    if (ecs->HasComponent<c_Tag>(m_selected)) {
        auto &tag = ecs->GetComponent<c_Tag>(m_selected);
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

    if (ecs->HasComponent<c_Transform>(m_selected))
        transformPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<c_Material>(m_selected))
        materialPanel.Render(ecs, m_selected, materialManager);
    if (ecs->HasComponent<c_Light>(m_selected))
        lightPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<c_Camera>(m_selected) && ecs->HasComponent<c_CameraOrientation>(m_selected))
        cameraPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<c_AudioSource>(m_selected) || ecs->HasComponent<c_AudioListener>(m_selected))
        audioPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<c_Script>(m_selected))
        scriptPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<c_Icon>(m_selected))
        iconPanel.Render(ecs, m_selected);
    if (ecs->HasComponent<c_RigidBody>(m_selected))
        RigidBodyPanel::Render(ecs, m_selected);
    if (ecs->HasComponent<c_Collider>(m_selected))
        colliderPanel.Render(ecs, m_selected);

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::Button("Add Component", {-1.f, 0}))
        ImGui::OpenPopup("##addcomp");

    if (ImGui::BeginPopup("##addcomp")) {
        if (!ecs->HasComponent<c_Script>(m_selected))
            if (ImGui::MenuItem("Script"))
                ecs->AddComponent<c_Script>(m_selected);

        if (!ecs->HasComponent<c_AudioSource>(m_selected))
            if (ImGui::MenuItem("Audio Source"))
                ecs->AddComponent<c_AudioSource>(m_selected);

        if (!ecs->HasComponent<c_AudioListener>(m_selected))
            if (ImGui::MenuItem("Audio Listener"))
                ecs->AddComponent<c_AudioListener>(m_selected);

        if (!ecs->HasComponent<c_Material>(m_selected))
            if (ImGui::MenuItem("Material"))
                ecs->AddComponent<c_Material>(m_selected);

        if (!ecs->HasComponent<c_RigidBody>(m_selected)) {
            if (ImGui::MenuItem("RigidBody")) {
                c_RigidBody rb;
                rb.m_invMass = 0.0f;
                rb.m_linearVelocity = glm::vec3(0.0f);
                rb.m_angularVelocity = glm::vec3(0.0f);
                rb.m_forceAccum = glm::vec3(0.0f);
                rb.m_torqueAccum = glm::vec3(0.0f);
                ecs->AddComponent<c_RigidBody>(m_selected, rb);
            }
        }

        if (!ecs->HasComponent<c_Collider>(m_selected)) {
            if (ImGui::MenuItem("Collider")) {
                c_Collider collider;
                Sphere sphere{};
                sphere.m_radius = 0.5f;
                collider.shape = sphere;
                ecs->AddComponent<c_Collider>(m_selected, std::move(collider));
            }
        }

        ImGui::EndPopup();
    }

    if (ImGui::Button("Remove Component", {-1.f, 0}))
        ImGui::OpenPopup("##removecomp");

    if (ImGui::BeginPopup("##removecomp")) {
        if (ecs->HasComponent<c_Script>(m_selected))
            if (ImGui::MenuItem("Script"))
                ecs->RemoveComponent<c_Script>(m_selected);

        if (ecs->HasComponent<c_AudioSource>(m_selected))
            if (ImGui::MenuItem("Audio Source"))
                ecs->RemoveComponent<c_AudioSource>(m_selected);

        if (ecs->HasComponent<c_AudioListener>(m_selected))
            if (ImGui::MenuItem("Audio Listener"))
                ecs->RemoveComponent<c_AudioListener>(m_selected);

        if (ecs->HasComponent<c_RigidBody>(m_selected))
            if (ImGui::MenuItem("RigidBody"))
                ecs->RemoveComponent<c_RigidBody>(m_selected);

        if (ecs->HasComponent<c_Collider>(m_selected))
            if (ImGui::MenuItem("Collider"))
                ecs->RemoveComponent<c_Collider>(m_selected);

        ImGui::EndPopup();
    }
}

void DebugOverlay::RenderUpperMenuTab() {
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

    if (ImGui::MenuItem("Open Model"))
        m_showOpenModelDialog = true;

    if (ImGui::MenuItem("Prefabs"))
        m_showOpenPrefabsDialog = true;

    if (ImGui::MenuItem("HitBox"))
        Execute("onCreateBoxHitbox");

    if (ImGui::MenuItem("Script Editor"))
        editor.Show();
}

void DebugOverlay::RenderOpenModelDialog() {
    if (!m_showOpenModelDialog) return;

    m_availableObjects = GetAvailableObjects();

    ImGui::OpenPopup("Open Model");

    if (ImGui::BeginPopupModal("Open Model", &m_showOpenModelDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
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

void DebugOverlay::RenderPrefabsDialog() {
    if (m_showOpenPrefabsDialog)
    {
        ImGui::OpenPopup("Open Prefabs");
        m_showOpenPrefabsDialog = false;
    }

    if (!ImGui::BeginPopupModal("Open Prefabs", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    constexpr auto formatPrefab = [](const std::string &prefabName) {
        std::string result;

        for (int i = 0; i < prefabName.size(); i++) {
            const char c = prefabName[i];

            if ( i > 0 && std::isupper(static_cast<u_char>(c)) && std::islower(static_cast<unsigned char>(prefabName[i - 1])))
                result += ' ';

            result += c;
        }

        return result;
    };

    const size_t prefixLength = strlen(PrefabPrefix) - 3;

    std::vector<std::string> prefabs;

    const auto& commands = CommandManager::GetCommands();

    for (const auto& [name, command] : commands)
        if (name.starts_with(PrefabPrefix))
            prefabs.emplace_back(name.substr(prefixLength));

    std::ranges::sort(prefabs.begin(), prefabs.end());

    ImGui::BeginChild("PrefabList", ImVec2(250, 200), true);

    for (const auto& prefabName : prefabs) {
        const std::string displayName = formatPrefab(prefabName);

        if (ImGui::Selectable(displayName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                const std::string commandName = std::string(PrefabPrefix) + prefabName;

                CommandManager::ExecuteCommand(commandName, {});

                ImGui::CloseCurrentPopup();
                break;
            }
    }

    ImGui::EndChild();

    ImGui::Spacing();

    if (ImGui::Button("Cancel"))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

void DebugOverlay::RefreshAvailableScenes(SceneSerializer &ss) {
    m_availableScenes = ss.GetAvailableScenes();
    std::ranges::sort(m_availableScenes);
    m_selectedScene = -1;
    m_scenesLoaded = true;
}

std::vector<std::string> DebugOverlay::GetAvailableObjects() {
    std::string folderPath = "../assets/objects/";
    std::vector<std::string> filePaths;

    try {
        for (const auto &entry: fs::recursive_directory_iterator(folderPath))
            if (entry.is_regular_file())
                filePaths.push_back(entry.path().string());
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR, e.what());
    }

    return filePaths;
}