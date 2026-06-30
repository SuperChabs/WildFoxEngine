#include "MaterialPanel.h"

#include "core/logging/Logger.h"

void MaterialPanel::Render(ECSWorld *ecs, entt::entity entity, MaterialManager *materialManager) {
    if (!ImGui::CollapsingHeader("Material")) return;

    if (ecs->HasComponent<MaterialComponent>(entity)) {
        auto &matComp = ecs->GetComponent<MaterialComponent>(entity);
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

    ImGui::DragFloat2("Tiling", &ecs->GetComponent<MaterialComponent>(entity).tiling[0]);

    if (ImGui::Button("Remove Material", ImVec2(-1, 0)))
        RemoveMaterial(ecs, entity);

    if (ImGui::Button("Add Material", ImVec2(-1, 0)))
        m_showOpenMaterialDialog = true;

    RenderOpenMaterialDialog();
}

void MaterialPanel::AssignMaterial(ECSWorld *ecs, entt::entity entity,
                                   MaterialManager *materialManager, const std::string &name) {
    auto material = materialManager->GetMaterial(name);

    if (ecs->HasComponent<MaterialComponent>(entity)) {
        auto &matComp = ecs->GetComponent<MaterialComponent>(entity);
        matComp.material = material;
    } else {
        ecs->AddComponent<MaterialComponent>(entity, material);
    }

    if (ecs->HasComponent<ColorComponent>(entity))
        ecs->RemoveComponent<ColorComponent>(entity);

    Logger::Log(LogLevel::INFO, LogCategory::RENDERING,
                "Material '" + name + "' assigned to entity");
}

void MaterialPanel::RemoveMaterial(ECSWorld *ecs, entt::entity entity) {
    if (ecs->HasComponent<MaterialComponent>(entity)) {
        ecs->RemoveComponent<MaterialComponent>(entity);
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING,
                    "Material removed from entity");
    }
}

void MaterialPanel::RenderOpenMaterialDialog() {
    if (!m_showOpenMaterialDialog) return;

    ImGui::OpenPopup("NewMaterial");

    if (ImGui::BeginPopupModal(
        "NewMaterial",
        &m_showOpenMaterialDialog,
        ImGuiWindowFlags_AlwaysAutoResize)) {
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

        if (ImGui::Button("Open")) {
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

        if (ImGui::Button("Cancel")) {
            m_showOpenMaterialDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
        }
}