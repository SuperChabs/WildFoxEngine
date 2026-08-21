#include <string>

#include <entt/meta/factory.hpp>
#include <glm/glm.hpp>

#include "Components.h"

using namespace entt::literals;

using ColliderShape = std::variant<AABB, Sphere>;

// --------------------------
// Helper Functions
// --------------------------

std::size_t VariantIndex(const ColliderShape& v) {
    return v.index();
}

entt::meta_any VariantGetActive(ColliderShape& v) {
    return std::visit([](auto& shape) -> entt::meta_any {
        return entt::forward_as_meta(shape);
    }, v);
}

// --------------------------
// Helper Template Functions
// --------------------------

template<typename T>
void VariantSet(ColliderShape& v, T value) {
    v = std::move(value);
}

template<typename T>
bool PtrValid(const std::shared_ptr<T>& ptr) {
    return ptr != nullptr;
}

template<typename T>
entt::meta_any PtrGet(std::shared_ptr<T>& ptr) {
    return ptr ? entt::forward_as_meta(*ptr) : entt::meta_any{};
}

template<typename T>
void PtrReset(std::shared_ptr<T>& ptr) {
    ptr.reset();
}

template<typename T>
bool HasComponent(entt::registry &registry, const entt::entity entity) {
    return registry.all_of<T>(entity);
}

template<typename T>
entt::meta_any GetComponent(entt::registry &registry, const entt::entity entity) {
    return entt::forward_as_meta(registry.get<T>(entity));
}

// --------------------------
// Register Types
// --------------------------

[[maybe_unused]] static const bool registerMath = []() {
    entt::meta_factory<glm::vec2>()
        .type("vec2"_hs, "Vector2")
        .data<&glm::vec2::x>("x"_hs, "X")
        .data<&glm::vec2::y>("y"_hs, "Y");

    entt::meta_factory<glm::vec3>()
        .type("vec3"_hs, "Vector3")
        .data<&glm::vec3::x>("x"_hs, "X")
        .data<&glm::vec3::y>("y"_hs, "Y")
        .data<&glm::vec3::z>("z"_hs, "Z");

    entt::meta_factory<glm::quat>()
    .type("quat"_hs, "Quaternion")
        .data<&glm::quat::x>("x"_hs, "X")
        .data<&glm::quat::y>("y"_hs, "Y")
        .data<&glm::quat::z>("z"_hs, "Z")
        .data<&glm::quat::w>("w"_hs, "W");

    return true;
}();

[[maybe_unused]] static const bool registerSTD = []() {
    entt::meta_factory<std::string>()
        .type("string"_hs, "String");

    entt::meta_factory<entt::entity>()
        .type("entity"_hs, "Entity");

    entt::meta_factory<std::unordered_set<entt::entity>>()
        .type("unordered_set_entity"_hs, "EntitySet");

    return true;
}();

[[maybe_unused]] static const bool registerLightType = []() {
    entt::meta_factory<LightType>()
        .type("LightType"_hs, "LightType")
        .data<LightType::DIRECTIONAL>("DIRECTIONAL"_hs, "Directional")
        .data<LightType::POINT>("POINT"_hs, "Point")
        .data<LightType::SPOT>("SPOT"_hs, "Spot");
    return true;
}();

// --------------------------
// Register Components
// --------------------------

[[maybe_unused]] static const bool registerAudio = []() {
    entt::meta_factory<AudioSourceComponent>()
        .type("AudioSourceComponent"_hs, "AudioSourceComponent")
        .data<&AudioSourceComponent::path>("path"_hs, "Path")
        .data<&AudioSourceComponent::volume>("volume"_hs, "Volume")
        .data<&AudioSourceComponent::pitch>("pitch"_hs, "Pitch")
        .data<&AudioSourceComponent::loop>("loop"_hs, "Loop")
        .data<&AudioSourceComponent::spatical>("spatical"_hs, "Spatical")
        .data<&AudioSourceComponent::playOnStart>("playOnStart"_hs, "PlayOnStart")
        .data<&AudioSourceComponent::_alSource>("_alSource"_hs, "_alSource")
        .data<&AudioSourceComponent::_playing>("playing"_hs, "Playing")
        .func<&HasComponent<AudioSourceComponent>>("has"_hs)
        .func<&GetComponent<AudioSourceComponent>>("get"_hs);

    return true;
}();

[[maybe_unused]] static const bool registerCamera = []() {
    entt::meta_factory<CameraComponent>()
        .type("CameraComponent"_hs, "CameraComponent")
        .data<&CameraComponent::fov>("fov"_hs, "Fov")
        .data<&CameraComponent::nearPlane>("nearPlane"_hs, "NearPlane")
        .data<&CameraComponent::farPlane>("farPlane"_hs, "FarPlane")
        .data<&CameraComponent::movementSpeed>("movementSpeed"_hs, "MovementSpeed")
        .data<&CameraComponent::isMainCamera>("mainCamera"_hs, "MainCamera")
        .data<&CameraComponent::isActive>("active"_hs, "Active")
        .func<&CameraComponent::GetProjectionMatrix>("GetProjectionMatrix"_hs)
        .func<&HasComponent<CameraComponent>>("has"_hs)
        .func<&GetComponent<CameraComponent>>("get"_hs);

    entt::meta_factory<CameraOrientationComponent>()
        .type("CameraOrientationComponent"_hs, "CameraOrientationComponent")
        .data<&CameraOrientationComponent::yaw>("yaw"_hs, "Yaw")
        .data<&CameraOrientationComponent::pitch>("pitch"_hs, "Pitch")
        .func<&CameraOrientationComponent::GetFront>("GetFront"_hs)
        .func<&CameraOrientationComponent::GetUp>("GetUp"_hs)
        .func<&CameraOrientationComponent::GetRight>("GetRight"_hs)
        .func<&CameraOrientationComponent::GetViewMatrix>("GetViewMatrix"_hs)
        .func<&HasComponent<CameraOrientationComponent>>("has"_hs)
        .func<&GetComponent<CameraOrientationComponent>>("get"_hs);

    return true;
}();

[[maybe_unused]] static const bool registerHierarchy = []() {
    entt::meta_factory<HierarchyComponent>()
        .type("HierarchyComponent"_hs, "HierarchyComponent")
        .data<&HierarchyComponent::parent>("parent"_hs, "Parent")
        .data<&HierarchyComponent::children>("children"_hs, "Children")
        .func<&HierarchyComponent::HasParent>("HasParent"_hs)
        .func<&HierarchyComponent::HasChildren>("HasChildren"_hs)
        .func<&HierarchyComponent::AddChild>("AddChild"_hs)
        .func<&HierarchyComponent::RemoveChild>("RemoveChild"_hs)
        .func<&HasComponent<HierarchyComponent>>("has"_hs)
        .func<&GetComponent<HierarchyComponent>>("get"_hs);
    return true;
}();

[[maybe_unused]] static const bool registerIcon = []() {
    entt::meta_factory<IconComponent>()
        .type("IconComponent"_hs, "IconComponent")
        .data<&IconComponent::iconTexturePath>("iconTexturePath"_hs, "IconTexturePath")
        .data<&IconComponent::textureID>("textureId"_hs, "TextureID")
        .data<&IconComponent::scale>("scale"_hs, "Scale")
        .data<&IconComponent::billboardMode>("billboardMode"_hs, "BillboardMode")
        .func<&HasComponent<IconComponent>>("has"_hs)
        .func<&GetComponent<IconComponent>>("get"_hs);

    return true;
}();

[[maybe_unused]] static const bool registerLight = []() {
    entt::meta_factory<LightComponent>()
        .type("LightComponent"_hs, "LightComponent")
        .data<&LightComponent::type>("type"_hs, "Type")
        .data<&LightComponent::position>("position"_hs, "Position")
        .data<&LightComponent::direction>("direction"_hs, "Direction")
        .data<&LightComponent::ambient>("ambient"_hs, "Ambient")
        .data<&LightComponent::diffuse>("diffuse"_hs, "Diffuse")
        .data<&LightComponent::specular>("specular"_hs, "Specular")
        .data<&LightComponent::intensity>("intensity"_hs, "Intensity")
        .data<&LightComponent::isActive>("isActive"_hs, "IsActive")
        .data<&LightComponent::castShadows>("castShadows"_hs, "CastShadows")
        .data<&LightComponent::constant>("constant"_hs, "Constant")
        .data<&LightComponent::linear>("linear"_hs, "Linear")
        .data<&LightComponent::quadratic>("quadratic"_hs, "Quadratic")
        .data<&LightComponent::radius>("radius"_hs, "Radius")
        .data<&LightComponent::innerCutoff>("innerCutoff"_hs, "InnerCutoff")
        .data<&LightComponent::outerCutoff>("outerCutoff"_hs, "OuterCutoff")
        .func<&LightComponent::SyncWithTransform>("SyncWithTransform"_hs)
        .func<&HasComponent<LightComponent>>("has"_hs)
        .func<&GetComponent<LightComponent>>("get"_hs);
    return true;
}();

[[maybe_unused]] static const bool registerTag = []() {
    entt::meta_factory<TagComponent>()
        .type("TagComponent"_hs, "TagComponent")
        .ctor<std::string>()
        .data<&TagComponent::name>("name"_hs, "Name")
        .func<&HasComponent<TagComponent>>("has"_hs)
        .func<&GetComponent<TagComponent>>("get"_hs);
    return true;
}();

[[maybe_unused]] static const bool registerID = []() {
    entt::meta_factory<IDComponent>()
        .type("IDComponent"_hs, "IDComponent")
        .ctor<uint64_t>()
        .data<&IDComponent::id>("id"_hs, "ID")
        .func<&HasComponent<IDComponent>>("has"_hs)
        .func<&GetComponent<IDComponent>>("get"_hs);
    return true;
}();

[[maybe_unused]] static const bool registerModel = []() {
    entt::meta_factory<ModelComponent>()
        .type("ModelComponent"_hs, "ModelComponent")
        .ctor<std::string>()
        .data<&ModelComponent::filePath>("filePath"_hs, "FilePath")
        .func<&HasComponent<ModelComponent>>("has"_hs)
        .func<&GetComponent<ModelComponent>>("get"_hs);
    return true;
}();

[[maybe_unused]] static const bool registerPhysics = []() {
    entt::meta_factory<AABB>()
        .type("AABB"_hs, "AABB")
        .data<&AABB::min>("min"_hs, "Min")
        .data<&AABB::max>("max"_hs, "Max");

    entt::meta_factory<Sphere>()
        .type("Sphere"_hs, "Sphere")
        .data<&Sphere::center>("center"_hs, "Center")
        .data<&Sphere::radius>("radius"_hs, "Radius");

    entt::meta_factory<RigidBodyComponent>()
        .type("RigidBodyComponent"_hs, "RigidBodyComponent")
        .data<&RigidBodyComponent::inv_mass>("inv_mass"_hs, "InverseMass")
        .data<&RigidBodyComponent::velocity>("velocity"_hs, "Velocity")
        .data<&RigidBodyComponent::angular_velocity>("angular_velocity"_hs, "AngularVelocity")
        .data<&RigidBodyComponent::inertia>("inertia"_hs, "Inertia")
        .data<&RigidBodyComponent::force_accum>("force_accum"_hs, "ForceAccum")
        .data<&RigidBodyComponent::torque_accum>("torque_accum"_hs, "TorqueAccum")
        .func<&HasComponent<RigidBodyComponent>>("has"_hs)
        .func<&GetComponent<RigidBodyComponent>>("get"_hs);

    entt::meta_factory<ColliderShape>()
        .type("ColliderShape"_hs, "ColliderShape")
        .func<&VariantIndex>("index"_hs)
        .func<&VariantGetActive>("getActive"_hs)
        .func<&VariantSet<AABB>>("setAABB"_hs)
        .func<&VariantSet<Sphere>>("setSphere"_hs);

    entt::meta_factory<ColliderComponent>()
        .type("ColliderComponent"_hs, "ColliderComponent")
        .data<&ColliderComponent::shape>("shape"_hs, "Shape")
        .data<&ColliderComponent::isTrigger>("isTrigger"_hs, "IsTrigger")
        .func<&HasComponent<ColliderComponent>>("has"_hs)
        .func<&GetComponent<ColliderComponent>>("get"_hs);
    return true;
}();

[[maybe_unused]] static const bool registerRendering = []() {
    entt::meta_factory<std::shared_ptr<Mesh>>()
        .type("shared_ptr_Mesh"_hs, "MeshRef")
        .func<&PtrValid<Mesh>>("valid"_hs)
        .func<&PtrGet<Mesh>>("get"_hs)
        .func<&PtrReset<Mesh>>("reset"_hs);

    entt::meta_factory<std::shared_ptr<Material>>()
        .type("shared_ptr_Material"_hs, "MaterialRef")
        .func<&PtrValid<Material>>("valid"_hs)
        .func<&PtrGet<Material>>("get"_hs)
        .func<&PtrReset<Material>>("reset"_hs);

    entt::meta_factory<MeshComponent>()
        .type("MeshComponent"_hs, "MeshComponent")
        .data<&MeshComponent::mesh>("mesh"_hs, "Mesh")
        .func<&HasComponent<MeshComponent>>("has"_hs)
        .func<&GetComponent<MeshComponent>>("get"_hs);

    entt::meta_factory<MaterialComponent>()
        .type("MaterialComponent"_hs, "MaterialComponent")
        .data<&MaterialComponent::material>("material"_hs, "Material")
        .data<&MaterialComponent::tiling>("tiling"_hs, "Tiling")
        .func<&HasComponent<MaterialComponent>>("has"_hs)
        .func<&GetComponent<MaterialComponent>>("get"_hs);

    entt::meta_factory<VisibilityComponent>()
        .type("VisibilityComponent"_hs, "VisibilityComponent")
        .ctor<bool>()
        .data<&VisibilityComponent::isActive>("isActive"_hs, "IsActive")
        .data<&VisibilityComponent::visible>("visible"_hs, "Visible")
        .func<&HasComponent<VisibilityComponent>>("has"_hs)
        .func<&GetComponent<VisibilityComponent>>("get"_hs);

    return true;
}();

[[maybe_unused]] static const bool registerScript = []() {
    entt::meta_factory<ScriptComponent>()
        .type("ScriptComponent"_hs, "ScriptComponent")
        .data<&ScriptComponent::scriptPath>("scriptPath"_hs, "ScriptPath")
        .data<&ScriptComponent::active>("active"_hs, "Active")
        .data<&ScriptComponent::loaded, entt::as_ref_t>("loaded"_hs, "Loaded")
        .data<&ScriptComponent::failed, entt::as_ref_t>("failed"_hs, "Failed")
        .func<&HasComponent<ScriptComponent>>("has"_hs)
        .func<&GetComponent<ScriptComponent>>("get"_hs);

    return true;
}();

[[maybe_unused]] static const bool registerTransform = []() {
    entt::meta_factory<TransformComponent>()
        .type("Transform"_hs, "Transform")
        .data<&TransformComponent::position>("position"_hs, "Position")
        .data<&TransformComponent::rotation>("rotation"_hs, "Rotation")
        .data<&TransformComponent::scale>("scale"_hs, "Scale")
        .data<&TransformComponent::eulerHint>("eulerHint"_hs, "EulerHint")
        .func<&TransformComponent::GetModelMatrix>("GetModelMatrix"_hs)
        .func<&HasComponent<TransformComponent>>("has"_hs)
        .func<&GetComponent<TransformComponent>>("get"_hs);

    return true;
}();