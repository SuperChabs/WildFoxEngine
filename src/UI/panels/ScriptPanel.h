#pragma once

#include <string>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class ScriptPanel {
    std::string scriptPathBuffer;

public:
    void Render(ECSWorld *ecs, entt::entity entity);

private:
    static void ReleaseScript(c_Script &script);

    static void AttachScript(ECSWorld *ecs, entt::entity entity, const std::string &path);
    static void UpdateScript(c_Script &script, const std::string &newPath);
    static void RemoveScript(ECSWorld *ecs, entt::entity entity, c_Script &script);
};