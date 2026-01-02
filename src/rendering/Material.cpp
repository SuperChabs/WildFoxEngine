#include "rendering/Material.h"

Material::Material(const std::vector<Texture>& textures)
    : textures(textures), useColor(false)
{
}

Material::Material(const glm::vec3& solidColor)
    : color(solidColor), useColor(true)
{
}

void Material::Bind(Shader& shader)
{
    shader.setBool("useColor", useColor);

    if (useColor) 
        shader.setVec3("Material.color", color);
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
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);
                
            shader.setInt(("Material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
    }
}

void Material::Unbind()
{
    for (unsigned int i = 0; i < textures.size(); i++) 
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
}