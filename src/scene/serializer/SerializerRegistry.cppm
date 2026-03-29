module;

#include <ext/stdlib.hpp>
#include <ext/entt.hpp>
#include <ext/json.hpp>

export module WFE.Scene.Serializer.SerializerRegistry;

import WFE.ECS.ECSWorld;
import WFE.Scene.Serializer.Component;
import WFE.Core.Logger;

using json = nlohmann::json;

export class SerializerRegistry
{
private:
    std::unordered_map<std::string, std::unique_ptr<IComponentSerializer>> serializers;

public:
    SerializerRegistry()
    {
        RegisterDefaultSerializers();
    }

    void RegisterSerializer(const std::string& name, std::unique_ptr<IComponentSerializer> serializer)
    {
        serializers[name] = std::move(serializer);
    }

    template<typename T>
    void RegisterSerializer(const std::string& name)
    {
        serializers[name] = std::make_unique<T>();
    }

    IComponentSerializer* GetSerializer(const std::string& name) const
    {
        auto it = serializers.find(name);
        if (it != serializers.end())
            return it->second.get();
        return nullptr;
    }

    json SerializeAllComponents(ECSWorld* world, entt::entity entity)
    {
        json entityData;

        for (auto& [name, serializer] : serializers)
        {
            if (serializer->CanSerialize(world, entity))
            {
                json componentData = serializer->Serialize(world, entity);
                if (!componentData.is_null() && !componentData.empty())
                {
                    entityData[name] = componentData;
                }
            }
        }

        return entityData;
    }

    void DeserializeAllComponents(ECSWorld* world, entt::entity entity, const json& entityData)
    {
        for (auto& [key, value] : entityData.items())
        {
            auto it = serializers.find(key);
            if (it != serializers.end() && !key.empty() && key[0] != '_')
            {
                try
                {
                    it->second->Deserialize(world, entity, value);
                }
                catch (const std::exception& e)
                {
                    Logger::Log(LogLevel::WARNING, 
                        std::string("Failed to deserialize component ") + key + ": " + e.what());
                }
            }
        }
    }

    std::vector<std::string> GetSerializerNames() const
    {
        std::vector<std::string> names;
        for (auto& [name, _] : serializers)
        {
            names.push_back(name);
        }
        return names;
    }

private:
    void RegisterDefaultSerializers()
    {
        RegisterSerializer<TransformSerializer>("transform");
        RegisterSerializer<MeshSerializer>("mesh");
        RegisterSerializer<MaterialSerializer>("material");
        RegisterSerializer<LightSerializer>("light");
        RegisterSerializer<CameraSerializer>("camera");
        RegisterSerializer<CameraTypeSerializer>("cameraType");
        RegisterSerializer<ScriptSerializer>("script");
        RegisterSerializer<VisibilitySerializer>("visibility");
        RegisterSerializer<IconSerializer>("icon");
        //RegisterSerializer<HierarchySerializer>("hierarchy");
    }
};
