module;

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

export module XEngine.Rendering.Material;

import XEngine.Rendering.MeshData; 
import XEngine.Core.Shader;   

export class Material
{
private:
    std::string name;
    std::vector<Texture> textures;
    glm::vec3 color;
    bool useColor = false;

public:
    Material(const std::vector<Texture>& textures)
        : textures(textures), useColor(false) {}

    Material(const glm::vec3& solidColor)
        : color(solidColor), useColor(true) {}

    void Bind(Shader& shader)
    {
        shader.setBool("useColor", useColor);

        if (useColor)
        {
            shader.setVec3("Material.color", color);
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
                std::string name = textures[i].type;
                if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
                else if (name == "texture_specular") number = std::to_string(specularNr++);
                else if (name == "texture_normal") number = std::to_string(normalNr++);
                else if (name == "texture_height") number = std::to_string(heightNr++);

                shader.setInt(("Material." + name + number).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            glActiveTexture(GL_TEXTURE0);
        }
    }

    void Unbind()
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glActiveTexture(GL_TEXTURE0);
    }

    // Сеттери
    void SetColor(glm::vec3 newColor) { color = newColor; }
    void SetTextures(std::vector<Texture> newTextures) { textures = newTextures; }
    void SetColorUsing(bool newUsing) { useColor = newUsing; }

    // Геттери
    const std::vector<Texture>& GetTextures() const { return textures; }
    size_t GetTextureCount() const { return textures.size(); }
    glm::vec3 GetColor() const { return color; }
    bool IsUsingColor() const { return useColor; }
};
