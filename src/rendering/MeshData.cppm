module;

#include <glm/glm.hpp>
#include <string>

export module WFE.Rendering.MeshData;

export struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

export struct Texture
{
    unsigned int id;
    std::string  type;
    std::string  path;
};