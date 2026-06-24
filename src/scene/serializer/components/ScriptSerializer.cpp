#include "ScriptSerializer.h"

json ScriptSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<ScriptComponent>(entity))
        return json::object();

    auto &script = world->GetComponent<ScriptComponent>(entity);
    return json{{"path", script.scriptPath}};
}

void ScriptSerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    std::string scriptPath = data.value("path", "");
    auto &scriptComp = world->AddComponent<ScriptComponent>(entity);
    scriptComp.scriptPath = scriptPath;
}

bool ScriptSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<ScriptComponent>(entity);
}