#include "MeshSerializer.h"
#include "rendering/primitive/PrimitivesFactory.h"
#include "scene/Mesh.h"

std::string MeshSerializer::PrimitiveTypeToString(PrimitiveType type) const {
    switch (type) {
        case PrimitiveType::CUBE: return "CUBE";
        case PrimitiveType::PLANE: return "PLANE";
        case PrimitiveType::QUAD: return "QUAD";
        default: return "CUBE";
    }
}

PrimitiveType MeshSerializer::StringToPrimitiveType(const std::string &name) const {
    if (name == "QUAD") return PrimitiveType::QUAD;
    if (name == "PLANE") return PrimitiveType::PLANE;
    return PrimitiveType::CUBE;
}

json MeshSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    json meshData;

    if (world->HasComponent<ModelComponent>(entity)) {
        auto &m = world->GetComponent<ModelComponent>(entity);
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

void MeshSerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    if (data.is_null() || data.empty())
        return;

    std::string meshType = data.value("type", "primitive");

    if (meshType == "primitive") {
        std::string primType = data.value("primitiveType", "CUBE");
        PrimitiveType type = StringToPrimitiveType(primType);
        Mesh *mesh = PrimitivesFactory::CreatePrimitive(type);
        if (mesh)
            world->AddComponent<MeshComponent>(entity, mesh);
    } else if (meshType == "model") {
        std::string modelPath = data.value("path", "");
        if (!modelPath.empty()) {
            world->AddComponent<ModelComponent>(entity, modelPath);
        }
    }
}

bool MeshSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<MeshComponent>(entity) ||
           world->HasComponent<ModelComponent>(entity);
}