#include "ScriptPanel.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "FileSelector.h"
#include "core/logging/Logger.h"

void ScriptPanel::Render(ECSWorld *ecs, entt::entity entity) {
    if (!ImGui::CollapsingHeader("Script")) return;

    if (ecs->HasComponent<c_Script>(entity)) {
        auto &script = ecs->GetComponent<c_Script>(entity);

        ImGui::Text("Script:");
        ImGui::TextWrapped("%s", script.scriptPath.c_str());

        ImGui::Spacing();
        ImGui::Text("Status:");

        if (script.failed)
            ImGui::BulletText("Failed");
        else if (script.loaded)
            ImGui::BulletText("Loaded");
        else
            ImGui::BulletText("Pending");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        auto &dialog = FileSelector::Instance();

        if (ImGui::Button("Load Script", ImVec2(ImGui::GetContentRegionAvail().x - 120.0f, 0)))
            dialog.OpenFile();

        if (dialog.Render())
            if (dialog.SelectedOpenFile()) {
                script.scriptPath = dialog.GetSelectedPath();
                scriptPathBuffer = dialog.GetSelectedPath();
            }

        ImGui::SameLine();

        if (ImGui::Button("Update Script", ImVec2(-1, 0)))
            UpdateScript(script, scriptPathBuffer);

        ImGui::Spacing();

        if (ImGui::Button("Remove Script", ImVec2(-1, 0))) {
            RemoveScript(ecs, entity, script);
        }
    } else {
        ImGui::InputTextWithHint("##NewScript", "scripts/player.as", &scriptPathBuffer);

        if (ImGui::Button("Attach Script", ImVec2(-1, 0))) {
            if (scriptPathBuffer[0]) {
                AttachScript(ecs, entity, scriptPathBuffer);
                scriptPathBuffer[0] = '\0';
            }
        }
    }
}

void ScriptPanel::ReleaseScript(c_Script &script) {
    if (script.ctx) {
        script.ctx->Release();
        script.ctx = nullptr;
    }

    if (script.module) {
        script.module->GetEngine()->DiscardModule(script.module->GetName());
        script.module = nullptr;
    }

    script.fnOnStart = nullptr;
    script.fnOnUpdate = nullptr;
    script.fnOnStop = nullptr;
}

void ScriptPanel::AttachScript(ECSWorld *ecs, entt::entity entity, const std::string &path) {
    auto &script = ecs->AddComponent<c_Script>(entity);
    script.scriptPath = path;
    script.loaded = false;
    script.failed = false;

    Logger::Log(LogLevel::INFO, "Script attached: " + path);
}

void ScriptPanel::UpdateScript(c_Script &script, const std::string &newPath) {
    ReleaseScript(script);

    script.scriptPath = newPath;
    script.loaded = false;
    script.failed = false;

    Logger::Log(LogLevel::INFO, "Script path updated: " + newPath);
}

void ScriptPanel::RemoveScript(ECSWorld *ecs, entt::entity entity, c_Script &script) {
    ReleaseScript(script);
    ecs->RemoveComponent<c_Script>(entity);

    Logger::Log(LogLevel::INFO, "Script removed");
}