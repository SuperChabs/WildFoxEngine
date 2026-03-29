module;

#include <ext/entt.hpp>
#include <ext/json.hpp>

export module WFE.Scene.Serializer.Component.MeshSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.Scene.Mesh;

using json = nlohmann::json;

export class MeshSerializer : public IComponentSerializer
{
private:
    std::string PrimitiveTypeToString(PrimitiveType type) const
    {
        switch (type)
        {
            case PrimitiveType::CUBE:   return "CUBE";
            case PrimitiveType::PLANE:  return "PLANE";
            case PrimitiveType::QUAD:   return "QUAD";
            default:                    return "CUBE";
        }
    }

    PrimitiveType StringToPrimitiveType(const std::string& name) const
    {
        if (name == "QUAD")  return PrimitiveType::QUAD;
        if (name == "PLANE") return PrimitiveType::PLANE;
        return PrimitiveType::CUBE;
    }

public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        json meshData;

        if (world->HasComponent<ModelComponent>(entity))
        {
            auto& m = world->GetComponent<ModelComponent>(entity);
            meshData["type"] = "model";
            meshData["path"] = m.filePath;
            return meshData;
        }
        
        // if (world->HasComponent<MeshComponent>(entity) && 
        //     !world->HasComponent<ModelComponent>(entity))
        // {
        //     auto& meshComp = world->GetComponent<MeshComponent>(entity);
        //     if (meshComp.mesh)
        //     {
        //         meshData["type"] = "primitive";
        //         meshData["primitiveType"] = PrimitiveTypeToString(meshComp.type);
        //         return meshData;
        //     }
        // }

        return json::object();
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        if (data.is_null() || data.empty())
            return;

        std::string meshType = data.value("type", "primitive");

        if (meshType == "primitive")
        {
            std::string primType = data.value("primitiveType", "CUBE");
            PrimitiveType type = StringToPrimitiveType(primType);
            Mesh* mesh = PrimitivesFactory::CreatePrimitive(type);
            if (mesh)
                world->AddComponent<MeshComponent>(entity, mesh);
        }
        else if (meshType == "model")
        {
            std::string modelPath = data.value("path", "");
            if (!modelPath.empty())
            {
                world->AddComponent<ModelComponent>(entity, modelPath);
            }
        }
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<MeshComponent>(entity) || 
               world->HasComponent<ModelComponent>(entity);
    }
};