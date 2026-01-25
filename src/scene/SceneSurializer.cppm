module;

#include <string>
#include <filesystem>
#include <fstream>

#include <glm/glm.hpp>
#include <entt.hpp>

#include <nlohmann/json.hpp>

export module WFE.Scene.SceneSurializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Camera;
import WFE.Core.Logger;
import WFE.Rendering.Light;
import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Texture.TextureManager;
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.Scene.Mesh;

using json = nlohmann::json;
namespace fs = std::filesystem;

export class SceneSerializer
{
private:
    ECSWorld* world;
    Camera* camera;
    std::string savesDirectory = "../saves/";

public:
    SceneSerializer(ECSWorld* w, Camera* c) 
        : world(w), camera(c) {}
    
    bool SaveScene(const std::string& filename)
    {
        if (!EnsureDirectoryExists(savesDirectory))
        {
            Logger::Log(LogLevel::ERROR, "Cannot create saves directory");
            return false;
        }
        
        std::string filepath = savesDirectory + filename;
        
        json sceneData;
        
        sceneData["scene"]["name"] = "MyScene";
        sceneData["scene"]["version"] = "1.0";
        sceneData["scene"]["timestamp"] = GetCurrentTimestamp();
        
        sceneData["scene"]["camera"] = SerializeCamera();
        
        sceneData["scene"]["entities"] = json::array();
        
        world->Each<IDComponent, TagComponent>(
            [&](entt::entity entity, IDComponent& id, TagComponent& tag) 
        {
            json entityData;
            
            entityData["id"] = id.id;
            entityData["name"] = tag.name;
            
            if (world->HasComponent<TransformComponent>(entity))
            {
                auto& transform = world->GetComponent<TransformComponent>(entity);
                entityData["transform"] = {
                    {"position", {transform.position.x, transform.position.y, transform.position.z}},
                    {"rotation", {transform.rotation.x, transform.rotation.y, transform.rotation.z}},
                    {"scale", {transform.scale.x, transform.scale.y, transform.scale.z}}
                };
            }
            
            if (world->HasComponent<MeshComponent>(entity))
            {
                auto& meshComp = world->GetComponent<MeshComponent>(entity);
                
                entityData["mesh"] = json::object();
                
                entityData["mesh"]["type"] = "primitive";
                entityData["mesh"]["primitiveType"] = "CUBE"; 
                
                // entityData["mesh"]["type"] = "model";
                // entityData["mesh"]["path"] = "path/to/model.obj";
            }
            
            if (world->HasComponent<MaterialComponent>(entity))
            {
                auto& mat = world->GetComponent<MaterialComponent>(entity);
                if (mat.material)
                {
                    entityData["material"] = 
                    {
                        {"name", mat.material->GetName()},
                        {"type", mat.material->GetType()} // "color" або "texture"
                    };
                    
                    if (mat.material->IsUsingColor())
                    {
                        glm::vec3 col = mat.material->GetColor();
                        entityData["material"]["color"] = {col.r, col.g, col.b};
                    }
                }
            }
            
            if (world->HasComponent<ColorComponent>(entity))
            {
                auto& color = world->GetComponent<ColorComponent>(entity);
                entityData["color"] = {color.color.r, color.color.g, color.color.b};
            }
            
            if (world->HasComponent<LightComponent>(entity))
            {
                auto& light = world->GetComponent<LightComponent>(entity);
                entityData["light"] = {
                    {"type", static_cast<int>(light.type)},
                    {"position", {light.position.x, light.position.y, light.position.z}},
                    {"direction", {light.direction.x, light.direction.y, light.direction.z}},
                    {"ambient", {light.ambient.r, light.ambient.g, light.ambient.b}},
                    {"diffuse", {light.diffuse.r, light.diffuse.g, light.diffuse.b}},
                    {"specular", {light.specular.r, light.specular.g, light.specular.b}},
                    {"intensity", light.intensity},
                    {"active", light.isActive},
                    {"castShadows", light.castShadows}
                };
                
                if (light.type == LightType::POINT || light.type == LightType::SPOT)
                {
                    entityData["light"]["attenuation"] = {
                        {"constant", light.constant},
                        {"linear", light.linear},
                        {"quadratic", light.quadratic},
                        {"radius", light.radius}
                    };
                }
                
                if (light.type == LightType::SPOT)
                {
                    entityData["light"]["cone"] = {
                        {"innerCutoff", light.innerCutoff},
                        {"outerCutoff", light.outerCutoff}
                    };
                }
            }
            
            if (world->HasComponent<RotationComponent>(entity))
            {
                auto& rot = world->GetComponent<RotationComponent>(entity);
                entityData["rotation_animation"] = {
                    {"auto", rot.autoRotate},
                    {"speed", rot.speed},
                    {"axis", {rot.axis.x, rot.axis.y, rot.axis.z}}
                };
            }
            
            if (world->HasComponent<IconComponent>(entity))
            {
                auto& icon = world->GetComponent<IconComponent>(entity);
                entityData["icon"] = {
                    {"path", icon.iconTexturePath},
                    {"scale", icon.scale},
                    {"billboard", icon.billboardMode}
                };
            }
            
            if (world->HasComponent<VisibilityComponent>(entity))
            {
                auto& vis = world->GetComponent<VisibilityComponent>(entity);
                entityData["visibility"] = {
                    {"active", vis.isActive},
                    {"visible", vis.visible}
                };
            }
            
            sceneData["scene"]["entities"].push_back(entityData);
        });
        
        std::ofstream file(filepath);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open file: " + filepath);
            return false;
        }
        
        file << sceneData.dump(4);
        file.close();
        
        Logger::Log(LogLevel::INFO, "Scene saved: " + filepath + 
                    " (" + std::to_string(sceneData["scene"]["entities"].size()) + " entities)");
        return true;
    }
    
    bool LoadScene(const std::string& filename,
                   MaterialManager* materialManager,
                   TextureManager* textureManager)
    {
        std::string filepath = savesDirectory + filename;
        
        if (!fs::exists(filepath))
        {
            Logger::Log(LogLevel::ERROR, "Scene file not found: " + filepath);
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            Logger::Log(LogLevel::ERROR, "Failed to open scene: " + filepath);
            return false;
        }
        
        json sceneData;
        try 
        {
            sceneData = json::parse(file);
        }
        catch (const json::parse_error& e) 
        {
            Logger::Log(LogLevel::ERROR, "JSON parse error: " + std::string(e.what()));
            return false;
        }
        
        world->Clear();
        
        if (sceneData["scene"].contains("camera"))
            DeserializeCamera(sceneData["scene"]["camera"]);
        
        int loadedCount = 0;
        for (auto& entityData : sceneData["scene"]["entities"])
        {
            std::string name = entityData.value("name", "Entity");
            auto entity = world->CreateEntity(name);
            
            if (entityData.contains("transform"))
            {
                auto& t = entityData["transform"];
                glm::vec3 pos = {t["position"][0], t["position"][1], t["position"][2]};
                glm::vec3 rot = {t["rotation"][0], t["rotation"][1], t["rotation"][2]};
                glm::vec3 scl = {t["scale"][0], t["scale"][1], t["scale"][2]};
                
                world->AddComponent<TransformComponent>(entity, pos, rot, scl);
            }
            
            if (entityData.contains("mesh"))
            {
                std::string meshType = entityData["mesh"].value("type", "primitive");
                
                if (meshType == "primitive")
                {
                    std::string primType = entityData["mesh"].value("primitiveType", "CUBE");
                    
                    Mesh* mesh = nullptr;
                    if (primType == "CUBE")
                        mesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
                    else if (primType == "QUAD")
                        mesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::QUAD);
                    else if (primType == "PLANE")
                        mesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::PLANE);
                    
                    if (mesh)
                        world->AddComponent<MeshComponent>(entity, mesh);
                }
                else if (meshType == "model")
                {
                    std::string modelPath = entityData["mesh"].value("path", "");
                    
                    Logger::Log(LogLevel::WARNING, 
                        "Model loading not yet implemented: " + modelPath);
                    
                    // Model* model = new Model(modelPath.c_str());
                    // world->AddComponent<MeshComponent>(entity, model->GetMesh(0));
                }
                else if (meshType == "custom")
                {
                    Logger::Log(LogLevel::WARNING, "Custom mesh loading not implemented");
                }
            }
            
            if (entityData.contains("material"))
            {
                std::string materialName = entityData["material"].value("name", "default");
                auto material = materialManager->GetMaterial(materialName);
                
                if (material)
                {
                    world->AddComponent<MaterialComponent>(entity, material);
                }
                else
                {
                    if (entityData["material"].contains("color"))
                    {
                        auto& col = entityData["material"]["color"];
                        glm::vec3 color = {col[0], col[1], col[2]};
                        
                        auto newMat = materialManager->CreateColorMaterial(
                            materialName, color
                        );
                        world->AddComponent<MaterialComponent>(entity, newMat);
                    }
                }
            }
            
            if (entityData.contains("color") && !world->HasComponent<MaterialComponent>(entity))
            {
                auto& col = entityData["color"];
                glm::vec3 color = {col[0], col[1], col[2]};
                world->AddComponent<ColorComponent>(entity, color);
            }
            
            if (entityData.contains("light"))
            {
                auto& l = entityData["light"];
                
                LightType type = static_cast<LightType>(l.value("type", 0));
                auto& lightComp = world->AddComponent<LightComponent>(entity, type);
                
                if (l.contains("position"))
                    lightComp.position = {l["position"][0], l["position"][1], l["position"][2]};
                if (l.contains("direction"))
                    lightComp.direction = {l["direction"][0], l["direction"][1], l["direction"][2]};
                if (l.contains("ambient"))
                    lightComp.ambient = {l["ambient"][0], l["ambient"][1], l["ambient"][2]};
                if (l.contains("diffuse"))
                    lightComp.diffuse = {l["diffuse"][0], l["diffuse"][1], l["diffuse"][2]};
                if (l.contains("specular"))
                    lightComp.specular = {l["specular"][0], l["specular"][1], l["specular"][2]};
                
                lightComp.intensity = l.value("intensity", 1.0f);
                lightComp.isActive = l.value("active", true);
                lightComp.castShadows = l.value("castShadows", true);
                
                if (l.contains("attenuation"))
                {
                    auto& att = l["attenuation"];
                    lightComp.constant = att.value("constant", 1.0f);
                    lightComp.linear = att.value("linear", 0.09f);
                    lightComp.quadratic = att.value("quadratic", 0.032f);
                    lightComp.radius = att.value("radius", 50.0f);
                }
                
                if (l.contains("cone"))
                {
                    auto& cone = l["cone"];
                    lightComp.innerCutoff = cone.value("innerCutoff", 12.5f);
                    lightComp.outerCutoff = cone.value("outerCutoff", 17.5f);
                }
            }
            
            if (entityData.contains("rotation_animation"))
            {
                auto& r = entityData["rotation_animation"];
                auto& rotComp = world->AddComponent<RotationComponent>(entity);
                
                rotComp.autoRotate = r.value("auto", false);
                rotComp.speed = r.value("speed", 50.0f);
                if (r.contains("axis"))
                    rotComp.axis = {r["axis"][0], r["axis"][1], r["axis"][2]};
            }
            
            if (entityData.contains("icon"))
            {
                auto& i = entityData["icon"];
                std::string path = i.value("path", "");
                float scale = i.value("scale", 0.5f);
                
                auto& iconComp = world->AddComponent<IconComponent>(entity, path, scale);
                iconComp.billboardMode = i.value("billboard", true);
            }
            
            if (entityData.contains("visibility"))
            {
                auto& v = entityData["visibility"];
                auto& visComp = world->AddComponent<VisibilityComponent>(entity);
                
                visComp.isActive = v.value("active", true);
                visComp.visible = v.value("visible", true);
            }
            else
            {
                world->AddComponent<VisibilityComponent>(entity, true);
            }
            
            loadedCount++;
        }
        
        Logger::Log(LogLevel::INFO, 
            "Scene loaded: " + filepath + " (" + std::to_string(loadedCount) + " entities)");
        return true;
    }
    
    std::vector<std::string> GetAvailableScenes()
    {
        std::vector<std::string> scenes;
        
        if (!fs::exists(savesDirectory))
            return scenes;
        
        try
        {
            for (const auto& entry : fs::directory_iterator(savesDirectory))
                if (entry.is_regular_file() && entry.path().extension() == ".json") 
                    scenes.push_back(entry.path().filename().string()); 
        }
        catch (const fs::filesystem_error& e)
        {
            Logger::Log(LogLevel::ERROR, "Error listing scenes: " + std::string(e.what()));
        }
        
        return scenes;
    }

private:
    bool EnsureDirectoryExists(const std::string& path)
    {
        try 
        {
            if (!fs::exists(path))
            {
                if (fs::create_directories(path))
                {
                    Logger::Log(LogLevel::INFO, "Created directory: " + path);
                    return true;
                }
                else
                {
                    Logger::Log(LogLevel::ERROR, "Failed to create directory: " + path);
                    return false;
                }
            }
            return true;
        }
        catch (const fs::filesystem_error& e)
        {
            Logger::Log(LogLevel::ERROR, "Filesystem error: " + std::string(e.what()));
            return false;
        }
    }
    
    json SerializeCamera()
    {
        return 
        {
            {"position", {camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z}},
            {"yaw", camera->GetYaw()},
            {"pitch", camera->GetPitch()},
            {"speed", camera->GetMovementSpeed()},
            {"sensitivity", camera->GetMouseSensitivity()},
            {"zoom", camera->GetZoom()}
        };
    }
    
    void DeserializeCamera(const json& camData)
    {
        if (camData.contains("position"))
            camera->SetPosition({camData["position"][0], camData["position"][1], camData["position"][2]});
        if (camData.contains("yaw"))
            camera->SetYaw(camData["yaw"]);
        if (camData.contains("pitch"))
            camera->SetPitch(camData["pitch"]);
        if (camData.contains("speed"))
            camera->SetMovementSpeed(camData["speed"]);
        if (camData.contains("sensitivity"))
            camera->SetMouseSensitivity(camData["sensitivity"]);
        if (camData.contains("zoom"))
            camera->SetZoom(camData["zoom"]);
    }
    
    std::string GetCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::tm tm_snapshot;
        localtime_r(&time, &tm_snapshot);
        
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_snapshot);
        
        return std::string(buffer);
    }
};