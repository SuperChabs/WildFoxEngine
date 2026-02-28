module;

#include <imgui.h>
#include <entt/entt.hpp>
#include <string>
#include <cstring>

export module WFE.UI.Panels.ScriptPanel;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;

export class ScriptPanel
{
private:
    char scriptPathBuffer[512] = "";

public:
    void Render(ECSWorld* ecs, entt::entity entity)
    {
        if (!ImGui::CollapsingHeader("Script")) return;

        if (ecs->HasComponent<ScriptComponent>(entity))
        {
            auto& script = ecs->GetComponent<ScriptComponent>(entity);

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

            ImGui::InputText("##ScriptPath", scriptPathBuffer, sizeof(scriptPathBuffer));

            if (ImGui::Button("Update Script", ImVec2(-1, 0)))
            {
                if (scriptPathBuffer[0])
                {
                    UpdateScript(script, scriptPathBuffer);
                    scriptPathBuffer[0] = '\0';
                }
            }

            ImGui::Spacing();

            if (ImGui::Button("Remove Script", ImVec2(-1, 0)))
            {
                RemoveScript(ecs, entity, script);
            }
        }
        else
        {
            ImGui::InputTextWithHint(
                "##NewScript",
                "scripts/player.as",
                scriptPathBuffer,
                sizeof(scriptPathBuffer)
            );

            if (ImGui::Button("Attach Script", ImVec2(-1, 0)))
            {
                if (scriptPathBuffer[0])
                {
                    AttachScript(ecs, entity, scriptPathBuffer);
                    scriptPathBuffer[0] = '\0';
                }
            }
        }
    }

private:
    void AttachScript(ECSWorld* ecs, entt::entity entity, const std::string& path)
    {
        auto& script = ecs->AddComponent<ScriptComponent>(entity);
        script.scriptPath = path;
        script.loaded = false;
        script.failed = false;

        Logger::Log(LogLevel::INFO, "Script attached: " + path);
    }

    void UpdateScript(ScriptComponent& script, const std::string& newPath)
    {
        if (script.ctx)
        {
            script.ctx->Release();
            script.ctx = nullptr;
        }

        if (script.module)
        {
            script.module->GetEngine()->DiscardModule(script.module->GetName());
            script.module = nullptr;
        }
        script.fnOnStart  = nullptr;
        script.fnOnUpdate = nullptr;
        script.fnOnStop   = nullptr;

        script.scriptPath = newPath;
        script.loaded = false;
        script.failed = false;

        Logger::Log(LogLevel::INFO, "Script path updated: " + newPath);
    }

    void RemoveScript(ECSWorld* ecs, entt::entity entity, ScriptComponent& script)
    {
        ecs->RemoveComponent<ScriptComponent>(entity);
        Logger::Log(LogLevel::INFO, "Script removed");
    }
};
