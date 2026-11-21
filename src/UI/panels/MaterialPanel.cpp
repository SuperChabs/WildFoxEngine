#include "MaterialPanel.h"

#include "FileSelector.h"
#include "core/logging/Logger.h"

void MaterialPanel::Render(ECSWorld *ecs, entt::entity entity, MaterialManager *materialManager) {
    if (!ImGui::CollapsingHeader("Material")) return;

    if (ecs->HasComponent<c_Material>(entity)) {
        const auto &matComp = ecs->GetComponent<c_Material>(entity);
        ImGui::BulletText("Current: %s",
                          matComp.material ? matComp.material->GetName().c_str() : "None");
        ImGui::Separator();
    }

    ImGui::Text("Assign Material:");
    if (ImGui::BeginCombo("##MaterialSelect", selectedMaterial.c_str(), 0)) {
        for (const auto &name: materialManager->GetMaterialNames()) {
            bool is_selected = (selectedMaterial == name);
            if (ImGui::Selectable(name.c_str(), is_selected)) {
                selectedMaterial = name;
                AssignMaterial(ecs, entity, materialManager, name);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::DragFloat2("Tiling", &ecs->GetComponent<c_Material>(entity).tiling[0]);

    if (ImGui::Button("Remove Material", ImVec2(-1, 0)))
        RemoveMaterial(ecs, entity);

    if (ImGui::Button("Add Material", ImVec2(-1, 0)))
        m_showOpenMaterialDialog = true;

    RenderOpenMaterialDialog();
}

void MaterialPanel::AssignMaterial(ECSWorld *ecs, entt::entity entity,
                                   MaterialManager *materialManager, const std::string &name) {
    auto material = materialManager->GetMaterial(name);

    if (ecs->HasComponent<c_Material>(entity)) {
        auto &matComp = ecs->GetComponent<c_Material>(entity);
        matComp.material = material;
    } else {
        ecs->AddComponent<c_Material>(entity, material);
    }

    Logger::Log(LogLevel::INFO, LogCategory::RENDERING,
                "Material '" + name + "' assigned to entity");
}

void MaterialPanel::RemoveMaterial(ECSWorld *ecs, entt::entity entity) {
    if (ecs->HasComponent<c_Material>(entity)) {
        ecs->RemoveComponent<c_Material>(entity);
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING,
                    "Material removed from entity");
    }
}

void MaterialPanel::RenderOpenMaterialDialog() {
    if (!m_showOpenMaterialDialog) return;

    ImGui::OpenPopup("NewMaterial");

    if (!ImGui::BeginPopupModal("NewMaterial", &m_showOpenMaterialDialog, ImGuiWindowFlags_AlwaysAutoResize)) return;


    ImGui::Text("Name:");
    ImGui::InputTextWithHint("##name", "material", m_name, sizeof(m_name));

    RenderOpenFileDialog("Diffuse Map##diff", m_diffusePath);
    RenderOpenFileDialog("Specular Map##spec", m_specularPath);
    RenderOpenFileDialog("Normal Map##norm", m_normalPath);
    RenderOpenFileDialog("Height Map##heig", m_heightPath);

    ImGui::Spacing();

    if (ImGui::Button("Open")) {
        if (std::string(m_name).empty())
            return;

        Execute("onCreateMaterial", {
            m_name,
            m_diffusePath,
            m_specularPath,
            m_normalPath,
            m_heightPath
        });

        m_showOpenMaterialDialog = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        m_showOpenMaterialDialog = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void MaterialPanel::RenderOpenFileDialog(const std::string &name, std::string &path) {
    auto &dialog = FileSelector::Instance();

    if (ImGui::Button(name.c_str())) {
        dialog.OpenFile();
    }

    if (dialog.Render())
        if (dialog.SelectedOpenFile())
            path = dialog.GetSelectedPath();
}
