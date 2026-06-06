#pragma once

#include <angelscript.h>
#include <glm/glm.hpp>

#include <string>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

#include "core/logging/Logger.h"
#include "ECS/components/Components.h"

void RegisterVec3(asIScriptEngine * engine);
void RegisterTransformComponent(asIScriptEngine * engine);

void Vec3Add(const glm::vec3 &other, const glm::vec3 &self, glm::vec3 &out);

void Vec3Sub(const glm::vec3 &other, const glm::vec3 &self, glm::vec3 &out);

void Vec3Mul(float scalar, const glm::vec3 &self, glm::vec3 &out);

void Vec3Constructor(glm::vec3 * self);

void Vec3ConstructorXYZ(float x, float y, float z, glm::vec3 *self);

void RegisterTypes(asIScriptEngine *engine) {
    RegisterVec3(engine);
    RegisterTransformComponent(engine);
}

void RegisterVec3(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType("vec3", sizeof(glm::vec3),
                                   asOBJ_VALUE |
                                   asOBJ_POD |
                                   asOBJ_APP_CLASS_CDAK
    );
    assert(r >= 0);
    AS_CHECK(r, "vec3 type");

    r = engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vec3Constructor),
                                        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 constructor");

    r = engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)",
                                        asFUNCTION(Vec3ConstructorXYZ), asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 constructor xyz");

    r = engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
    AS_CHECK(r, "vec3.x");

    r = engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
    AS_CHECK(r, "vec3.y");

    r = engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
    AS_CHECK(r, "vec3.z");

    r = engine->RegisterObjectMethod(
        "vec3",
        "vec3 opAdd(const vec3 &in) const",
        asFUNCTION(Vec3Add),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 add");

    r = engine->RegisterObjectMethod(
        "vec3",
        "vec3 opSub(const vec3 &in) const",
        asFUNCTION(Vec3Sub),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 sub");

    r = engine->RegisterObjectMethod(
        "vec3",
        "vec3 opMul(float) const",
        asFUNCTION(Vec3Mul),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 mul");
}

void RegisterTransformComponent(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT);
    assert(r >= 0);
    AS_CHECK(r, "Transform type");

    r = engine->RegisterObjectProperty("Transform", "vec3 position", asOFFSET(TransformComponent, position));
    AS_CHECK(r, "Transform.position");

    r = engine->RegisterObjectProperty("Transform", "vec3 scale", asOFFSET(TransformComponent, scale));
    AS_CHECK(r, "Transform.scale");

    r = engine->RegisterObjectProperty("Transform", "vec3 rotation", asOFFSET(TransformComponent, eulerHint));
    AS_CHECK(r, "Transform.rotation");
}

/// @name glm::vec3 
/// @{
void Vec3Add(const glm::vec3 &other, const glm::vec3 &self, glm::vec3 &out) {
    out = self + other;
}

void Vec3Sub(const glm::vec3 &other, const glm::vec3 &self, glm::vec3 &out) {
    out = self - other;
}

void Vec3Mul(float scalar, const glm::vec3 &self, glm::vec3 &out) {
    out = self * scalar;
}

void Vec3Constructor(glm::vec3 *self) {
    new(self) glm::vec3(0.0f);
}

void Vec3ConstructorXYZ(float x, float y, float z, glm::vec3 *self) {
    new(self) glm::vec3(x, y, z);
}

/// @}