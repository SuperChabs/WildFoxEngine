#include "RigidBodySerializer.h"

json RigidBodySerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<RigidBodyComponent>(entity))
        return json::object();

    auto &rb = world->GetComponent<RigidBodyComponent>(entity);
    return json{
        {"inv_mass", rb.m_invMass},
        {"elasticity", rb.m_elasticity},
        {"friction", rb.m_friction},
        {"velocity", {rb.m_linearVelocity.x, rb.m_linearVelocity.y, rb.m_linearVelocity.z}},
        {"angular_velocity", {rb.m_angularVelocity.x, rb.m_angularVelocity.y, rb.m_angularVelocity.z}},
        {"force_accum", {rb.m_forceAccum.x, rb.m_forceAccum.y, rb.m_forceAccum.z}},
        {"torque_accum", {rb.m_torqueAccum.x, rb.m_torqueAccum.y, rb.m_torqueAccum.z}}
    };
}

entt::entity RigidBodySerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    if (!data.contains("inv_mass"))
        return entt::null;

    RigidBodyComponent rb{
        .m_invMass = data.value("inv_mass", 0.0f),
        .m_elasticity = data.value("elasticity", 0.0f),
        .m_friction = data.value("friction", 0.0f),
        .m_linearVelocity = Vec3FromJson(data.value("velocity", json::array({0.0f, 0.0f, 0.0f}))),
        .m_angularVelocity = Vec3FromJson(data.value("angular_velocity", json::array({0.0f, 0.0f, 0.0f}))),
        .m_forceAccum = Vec3FromJson(data.value("force_accum", json::array({0.0f, 0.0f, 0.0f}))),
        .m_torqueAccum = Vec3FromJson(data.value("torque_accum", json::array({0.0f, 0.0f, 0.0f})))
    };

    dcx.world->AddComponent<RigidBodyComponent>(entity, rb);

    return entity;
}

bool RigidBodySerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<RigidBodyComponent>(entity);
}