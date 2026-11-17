#include "TagPanel.h"
#include <string>

void TagPanel::Render(ECSWorld *ecs, entt::entity entity) {
    if (!ecs->HasComponent<c_Tag>(entity)) return;

    auto &tag = ecs->GetComponent<c_Tag>(entity);

    char buffer[256];
    strncpy(buffer, tag.name.c_str(), sizeof(buffer));
    buffer[255] = '\0';

    ImGui::Text("Name:");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
        tag.name = std::string(buffer);
    ImGui::PopItemWidth();

    ImGui::Spacing();

    if (ecs->HasComponent<c_Visibility>(entity)) {
        auto &vis = ecs->GetComponent<c_Visibility>(entity);
        ImGui::Checkbox("Active", &vis.isActive);
    }
}