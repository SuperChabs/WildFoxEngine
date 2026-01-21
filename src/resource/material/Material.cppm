module;

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

export module WFE.Rendering.Material;

import WFE.Rendering.MeshData; 
import WFE.Resource.Shader.ShaderManager;
import WFE.Core.Logger;

export class Material
{
private:
    std::string name;
    std::string type;  
    std::vector<Texture> textures;
    glm::vec3 color;
    bool useColor = false;

public:
    Material(const std::vector<Texture>& textures, const std::string& materialName = "unnamed")
        : name(materialName), type("texture"), textures(textures), useColor(false), color(1.0f, 1.0f, 1.0f) 
    {
        if (textures.empty()) 
        {
            useColor = true;
            color = glm::vec3(1.0f, 1.0f, 1.0f);
            Logger::Log(LogLevel::WARNING, LogCategory::RENDERING, 
                "Material '" + name + "' created with white color (no textures)");
        } 
        else 
        { 
            useColor = false;
            Logger::Log(LogLevel::INFO, LogCategory::RENDERING, 
                "Material '" + name + "' created with " + std::to_string(textures.size()) + " textures");
        }
    }

    Material(const glm::vec3& solidColor, const std::string& materialName = "unnamed")
        : name(materialName), type("color"), color(solidColor), useColor(true) 
    {
        Logger::Log(LogLevel::INFO, LogCategory::RENDERING, 
            "Material '" + name + "' created with color: (" + 
            std::to_string(color.r) + ", " + 
            std::to_string(color.g) + ", " + 
            std::to_string(color.b) + ")");
    }

    Material& operator= (Material& other)
    {
        if (this == &other) 
            return *this;
        
        this->name = other.name;
        this->type = other.type;
        this->color = other.color;
        this->textures = other.textures;
        this->useColor = other.useColor;

        return *this;
    }

    void Bind(ShaderManager& shaderManager, const std::string& shaderName)
    {
        shaderManager.Bind(shaderName);

        shaderManager.SetBool(shaderName, "useColor", useColor);

        if (useColor)
        {
            shaderManager.SetVec3(shaderName, "material.color", color);
        }
        else
        {
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;

            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);

                std::string number;
                std::string texType = textures[i].type;

                if (texType == "texture_diffuse") 
                    number = std::to_string(diffuseNr++);
                else if (texType == "texture_specular") 
                    number = std::to_string(specularNr++);
                else if (texType == "texture_normal") 
                    number = std::to_string(normalNr++);
                else if (texType == "texture_height") 
                    number = std::to_string(heightNr++);

                std::string uniformName = "material." + texType + number;
                shaderManager.SetInt(shaderName, uniformName.c_str(), i);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            glActiveTexture(GL_TEXTURE0);
        }
    }

    void Unbind()
    {
        if (!useColor)
        {
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glActiveTexture(GL_TEXTURE0);
        }
    }

    void SetColor(glm::vec3 newColor) 
    { 
        color = newColor; 
        useColor = true;
        textures.clear();
    }

    void SetTextures(std::vector<Texture> newTextures) 
    { 
        textures = newTextures; 
        useColor = false;
    }
    
    void SetColorUsing(bool newUsing) { useColor = newUsing; }
    void SetName(const std::string& newName) { name = newName; }
    void SetType(const std::string& newType) { type = newType; }

    const std::vector<Texture>& GetTextures() const { return textures; }
    size_t GetTextureCount() const { return textures.size(); }
    glm::vec3 GetColor() const { return color; }
    bool IsUsingColor() const { return useColor; }
    const std::string& GetName() const { return name; }
    const std::string& GetType() const { return type; }
};