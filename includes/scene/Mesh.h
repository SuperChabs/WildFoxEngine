#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

#include "core/Shader.h"

using namespace glm;
using namespace std;

struct Vertex
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
    
    vec3 Tangent;
    vec3 Bitangent;
};

struct Texture
{
    unsigned int id;
    string type;
    string path;
};

class Mesh 
{
public:
    // Mesh-данні
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;


    // Ренденг данні
    unsigned int VAO, VBO, EBO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader shader);

    vector<Vertex> GetVertices()      {return vertices;}
    vector<unsigned int> GetIndices() {return indices; }
    vector<Texture> GetTexture()      {return textures;}
    
    void SetVerices(vector<Vertex> newVertices)      {vertices = newVertices;}
    void SetIndices(vector<unsigned int> newIndices) {indices  = newIndices;}
    void SetTexture(vector<Texture> newTextures)     {textures = newTextures;}

private:
    void setupMesh();
};

#endif