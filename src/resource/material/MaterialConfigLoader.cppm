module;

#include <string>
#include <vector>
#include <fstream>

#include <nlohmann/json.hpp>

#include <glm/vec3.hpp>

export module WildFoxEngine.Resource.Material.MaterialConfigLoader;

import WildFoxEngine.Core.Logger;

import WildFoxEngine.Rendering.MeshData;

import WildFoxEngine.Resource.Texture.TextureManager;

using json = nlohmann::json;

export struct MaterialConfig
{
    std::string name;
    std::string type;
    glm::vec3 color;
    
    std::string diffusePath;
    std::string specularPath;
    std::string normalPath;
    std::string heightPath;
    
    MaterialConfig()
        : name(""), type(""), color(1.0f), 
          diffusePath(""), specularPath(""), normalPath(""), heightPath("")
    {}
};

export class MaterialConfigLoader
{
    std::vector<MaterialConfig> materialConfigs;

public:
    const std::vector<MaterialConfig>& LoadMaterialConfigs(TextureManager& textureManager, const std::string& path)
    {
        materialConfigs.clear();

        std::ifstream f(path);
        if (!f.is_open())
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, "Failed to open material JSON file: " + path);
            return materialConfigs;
        }

        try 
        {
            json data = json::parse(f);

            if (!data.contains("material"))
            {
                Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, "No 'material' array in config file");
                return materialConfigs;
            }

            for (auto& m : data["material"])
            {
                MaterialConfig config;
                
                if (m.contains("name"))
                    config.name = m["name"];
                else
                    continue; 

                if (m.contains("type"))
                    config.type = m["type"];
                else
                    config.type = "color"; 

                if (m.contains("color"))
                    config.color = ParseColor(m["color"]);

                if (m.contains("diffuse"))
                    config.diffusePath = m["diffuse"];
                if (m.contains("specular"))
                    config.specularPath = m["specular"];
                if (m.contains("normal"))
                    config.normalPath = m["normal"];
                if (m.contains("height"))
                    config.heightPath = m["height"];

                materialConfigs.push_back(config);
                
                Logger::Log(LogLevel::INFO, LogCategory::RENDERING, 
                    "Loaded material config: " + config.name + " (type: " + config.type + ")");
            }
        }
        catch (const json::parse_error& e)
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, 
                "JSON parse error in materials: " + std::string(e.what()));
        }
        catch (const json::type_error& e)
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, 
                "JSON type error in materials: " + std::string(e.what()));
        }

        return materialConfigs;
    }

    const std::vector<MaterialConfig>& GetConfigs() const
    {
        return materialConfigs;
    }

private:
    glm::vec3 ParseColor(const json& colorArray)
    {
        if (!colorArray.is_array())
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                "Color is not an array, using default white");
            return glm::vec3(1.0f, 1.0f, 1.0f);
        }
        
        if (colorArray.size() < 3)
        {
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                "Color array has less than 3 elements, using default white");
            return glm::vec3(1.0f, 1.0f, 1.0f);
        }
        
        try
        {
            float r = colorArray[0].get<float>();
            float g = colorArray[1].get<float>();
            float b = colorArray[2].get<float>();
            
            r = std::clamp(r, 0.0f, 1.0f);
            g = std::clamp(g, 0.0f, 1.0f);
            b = std::clamp(b, 0.0f, 1.0f);
            
            return glm::vec3(r, g, b);
        }
        catch (const json::type_error& e)
        {
            Logger::Log(LogLevel::ERROR, LogCategory::RENDERING, 
                "Color parsing error: " + std::string(e.what()));
            return glm::vec3(1.0f, 1.0f, 1.0f);
        }
    }
};