#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <string>
#include <memory>
#include <vector>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class TagPanel {
public:
    void Render(ECSWorld *ecs, entt::entity entity) {
        if (!ecs->HasComponent<TagComponent>(entity)) return;

        auto &tag = ecs->GetComponent<TagComponent>(entity);

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

        if (ecs->HasComponent<VisibilityComponent>(entity)) {
            auto &vis = ecs->GetComponent<VisibilityComponent>(entity);
            ImGui::Checkbox("Active", &vis.isActive);
        }
    }
};