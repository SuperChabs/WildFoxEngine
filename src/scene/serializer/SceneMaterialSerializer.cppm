module;

#include <string>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module WFE.Scene.Serializer.SceneMaterialSerializer;

import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Material.Material;
import WFE.Core.Logger;

using json = nlohmann::json;

export class SceneMaterialSerializer
{
public:
    json Serialize(MaterialManager* materialManager)
    {
        json sceneData = json::array();
        auto materials = materialManager->GetMaterialsMap();

        for (auto& [name, mat] : materials)
        {
            if (name == "default" || name == "gray")
                continue;

            json matData;
            matData["name"] = name;
            if (mat->IsUsingColor())
            {
                matData["type"] = "color";
                matData["color"] = {mat->GetColor().x, mat->GetColor().y, mat->GetColor().z};
            }
            else 
            {
                matData["type"] = "texture";

                json texturesData = json::array();
                for (auto& tex : mat->GetTextures())
                {
                    json texData;
                    texData["type"] = tex.type;
                    texData["path"] = tex.path;
                    texturesData.push_back(texData);
                }

                matData["textures"] = texturesData;

            }

            sceneData.push_back(matData);
        }

        return sceneData;
    }

    void Deserialize(MaterialManager* materialManager, const json& data)
    {
        try 
        {
            for (auto& matData : data)
            {
                std::string name = matData["name"];
                std::string type = matData["type"];

                if (materialManager->HasMaterial(name))
                    continue;

                if (type == "color")
                {
                    glm::vec3 color = {matData["color"][0], matData["color"][1], matData["color"][2]};
                    materialManager->CreateColorMaterial(name, color);
                }
                else if (type == "texture")
                {
                    std::string diffusePath = "";
                    std::string normalPath = "";
                    std::string specularPath = "";
                    std::string heightPath = "";

                    for (auto& tex : matData["textures"])
                    {
                        if (tex["type"] == "texture_diffuse")
                            diffusePath = tex["path"];
                        if (tex["type"] == "texture_normal")
                            normalPath = tex["path"];
                        if (tex["type"] == "texture_specular")
                            specularPath = tex["path"];
                        if (tex["type"] == "texture_height")
                            heightPath = tex["path"];
                    }

                    materialManager->CreateTextureMaterial(
                        name, 
                        diffusePath,
                        specularPath,
                        normalPath,
                        heightPath
                    );
                }
            }
        } 
        catch (const std::exception& e) 
        {
            Logger::Log(LogLevel::ERROR, e.what());
        }
    }
};