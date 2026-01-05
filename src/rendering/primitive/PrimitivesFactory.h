#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "rendering/Buffer.h"
#include "core/Shader.h"
#include "scene/Mesh.h"
#include "scene/SceneManager.h"

enum class PrimitiveType
{
    CUBE,
    QUAD,
    PLANE
};

class PrimitivesFactory
{
public:
    static Mesh* CreatePrimitive(PrimitiveType type);
    static SceneManager* CreatePrimitiveInScene(PrimitiveType type, SceneManager* sceneManager);

    static std::string GetPrimitiveName(PrimitiveType type);
};

#endif // PRIMITIVES_HPP