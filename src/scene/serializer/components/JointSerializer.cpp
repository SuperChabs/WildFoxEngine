#include "JointSerializer.h"

json JointSerializer::Serialize(ECSWorld *world, const entt::entity entity) {
    if (!world->HasComponent<c_Joint>(entity))
        return json::object();

    auto &joint = world->GetComponent<c_Joint>(entity);
    return json{
        {"type", static_cast<int>(joint.type)},
        {"bodyA", entt::to_integral(joint.bodyA)},
        {"bodyB", entt::to_integral(joint.bodyB)},
        {"anchorA", {joint.localAnchorA.x, joint.localAnchorA.y, joint.localAnchorA.z}},
        {"anchorB", {joint.localAnchorB.x, joint.localAnchorB.y, joint.localAnchorB.z}},
        {"axisA", {joint.axisA.x, joint.axisA.y, joint.axisA.z}},
        {"axisB", {joint.axisB.x, joint.axisB.y, joint.axisB.z}}
    };
}

entt::entity JointSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    auto &joint = dcx.world->AddComponent<c_Joint>(entity);

    joint.type = static_cast<JointType>(data.value("type", 1));
    joint.bodyA = static_cast<entt::entity>(data["bodyA"].get<std::uint32_t>());
    joint.bodyB = static_cast<entt::entity>(data["bodyB"].get<std::uint32_t>());
    joint.localAnchorA = {data["anchorA"][0], data["anchorA"][1], data["anchorA"][2]};
    joint.localAnchorB = {data["anchorB"][0], data["anchorB"][1], data["anchorB"][2]};
    joint.axisA = {data["axisA"][0], data["axisA"][1], data["axisA"][2]};
    joint.axisB = {data["axisB"][0], data["axisB"][1], data["axisB"][2]};

    return entity;
}

bool JointSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<c_Joint>(entity);
}
