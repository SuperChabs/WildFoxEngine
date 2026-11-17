#include "ScriptSerializer.h"

json ScriptSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<c_Script>(entity))
        return json::object();

    auto &script = world->GetComponent<c_Script>(entity);
    return json{{"path", script.scriptPath}};
}

entt::entity ScriptSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    std::string scriptPath = data.value("path", "");
    auto &scriptComp = dcx.world->AddComponent<c_Script>(entity);
    scriptComp.scriptPath = scriptPath;

    return entity;
}

bool ScriptSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<c_Script>(entity);
}