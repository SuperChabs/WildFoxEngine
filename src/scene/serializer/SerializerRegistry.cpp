#include "SerializerRegistry.h"
#include "core/logging/Logger.h"

SerializerRegistry::SerializerRegistry() {
    RegisterDefaultSerializers();
}

void SerializerRegistry::RegisterSerializer(const std::string &name, std::unique_ptr<IComponentSerializer> serializer) {
    serializers[name] = std::move(serializer);
}

IComponentSerializer *SerializerRegistry::GetSerializer(const std::string &name) const {
    auto it = serializers.find(name);
    if (it != serializers.end())
        return it->second.get();
    return nullptr;
}

json SerializerRegistry::SerializeAllComponents(ECSWorld *world, entt::entity entity) {
    json entityData;

    for (auto &[name, serializer]: serializers) {
        if (serializer->CanSerialize(world, entity)) {
            json componentData = serializer->Serialize(world, entity);
            if (!componentData.is_null() && !componentData.empty()) {
                entityData[name] = componentData;
            }
        }
    }

    return entityData;
}

entt::entity SerializerRegistry::DeserializeAllComponents(DeserializeContext ctx, entt::entity entity,
                                                          const json &entityData) {
    if (entityData.contains("mesh"))
        entity = serializers["mesh"]->Deserialize(ctx, entity, entityData["mesh"]);

    static std::vector<std::string> kOrder = {"material", "color"};
    for (const auto &k: kOrder) {
        if (!entityData.contains(k)) continue;
        entity = serializers[k]->Deserialize(ctx, entity, entityData[k]);
    }

    for (auto &[key, serializer]: serializers) {
        if (key == "mesh" || key == "material" || key == "color") continue;
        if (key.empty() || key[0] == '_') continue;
        if (!entityData.contains(key)) continue;

        try {
            entity = serializer->Deserialize(ctx, entity, entityData[key]);
        } catch (const std::exception &e) {
            Logger::Log(LogLevel::WARNING, "Failed to deserialize component:" + key + ": " + e.what());
        }
    }

    return entity;
}

std::vector<std::string> SerializerRegistry::GetSerializerNames() const {
    std::vector<std::string> names;
    for (auto &[name, _]: serializers) {
        names.push_back(name);
    }
    return names;
}

void SerializerRegistry::RegisterDefaultSerializers() {
    RegisterSerializer<TransformSerializer>("transform");
    RegisterSerializer<MeshSerializer>("mesh");
    RegisterSerializer<MaterialSerializer>("material");
    RegisterSerializer<LightSerializer>("light");
    RegisterSerializer<CameraSerializer>("camera");
    RegisterSerializer<ScriptSerializer>("script");
    RegisterSerializer<AudioSourceSerializer>("audioSource");
    RegisterSerializer<AudioListenerSerializer>("audioListener");
    RegisterSerializer<VisibilitySerializer>("visibility");
    RegisterSerializer<IconSerializer>("icon");
    RegisterSerializer<RigidBodySerializer>("rigidBody");
    RegisterSerializer<ColliderSerializer>("collider");
    //RegisterSerializer<HierarchySerializer>("hierarchy");
}