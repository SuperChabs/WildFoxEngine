#include "RigidBodySerializer.h"

json RigidBodySerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<RigidBodyComponent>(entity))
        return json::object();

    auto &rb = world->GetComponent<RigidBodyComponent>(entity);
    return json{
        {"inv_mass", rb.inv_mass},
        {"velocity", {rb.velocity.x, rb.velocity.y, rb.velocity.z}},
        {"angular_velocity", {rb.angular_velocity.x, rb.angular_velocity.y, rb.angular_velocity.z}},
        {"inertia", {rb.inertia.x, rb.inertia.y, rb.inertia.z}},
        {"force_accum", {rb.force_accum.x, rb.force_accum.y, rb.force_accum.z}},
        {"torque_accum", {rb.torque_accum.x, rb.torque_accum.y, rb.torque_accum.z}}
    };
}

void RigidBodySerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    if (!data.contains("inv_mass"))
        return;

    RigidBodyComponent rb;
    rb.inv_mass = data.value("inv_mass", 0.0f);
    rb.velocity = Vec3FromJson(data.value("velocity", json::array({0.0f, 0.0f, 0.0f})));
    rb.angular_velocity = Vec3FromJson(data.value("angular_velocity", json::array({0.0f, 0.0f, 0.0f})));
    rb.inertia = Vec3FromJson(data.value("inertia", json::array({0.0f, 0.0f, 0.0f})));
    rb.force_accum = Vec3FromJson(data.value("force_accum", json::array({0.0f, 0.0f, 0.0f})));
    rb.torque_accum = Vec3FromJson(data.value("torque_accum", json::array({0.0f, 0.0f, 0.0f})));

    world->AddComponent<RigidBodyComponent>(entity, rb);
}

bool RigidBodySerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<RigidBodyComponent>(entity);
}