#pragma once

#include <angelscript.h>
#include <glm/glm.hpp>

#include <string>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

#include "core/logging/Logger.h"
#include "ECS/components/Components.h"

// vec4 operators
inline void Vec4Add(const glm::vec4 &other, const glm::vec4 &self, glm::vec4 &out) { out = self + other; }
inline void Vec4Sub(const glm::vec4 &other, const glm::vec4 &self, glm::vec4 &out) { out = self - other; }
inline void Vec4Mul(float scalar, const glm::vec4 &self, glm::vec4 &out) { out = self * scalar; }
inline void Vec4Div(const glm::vec4 &other, const glm::vec4 &self, glm::vec4 &out) { out = self / other; }
inline bool Vec4Equal(const glm::vec4 &other, const glm::vec4 &self) { return self == other; }
inline void Vec4Constructor(glm::vec4 *self) { new(self) glm::vec4(0.0f); }
inline void Vec4ConstructorXYZW(float x, float y, float z, float w, glm::vec4 *self) { new(self) glm::vec4(x, y, z, w); }

// vec3 operators
inline void Vec3Add(const glm::vec3 &other, const glm::vec3 &self, glm::vec3 &out) { out = self + other; }
inline void Vec3Sub(const glm::vec3 &other, const glm::vec3 &self, glm::vec3 &out) { out = self - other; }
inline void Vec3Mul(float scalar, const glm::vec3 &self, glm::vec3 &out) { out = self * scalar; }
inline void Vec3Div(const glm::vec3 &other, const glm::vec3 &self, glm::vec3 &out) { out = self / other; }
inline bool Vec3Equal(const glm::vec3 &other, const glm::vec3 &self) { return self == other; }
inline void Vec3Constructor(glm::vec3 *self) { new(self) glm::vec3(0.0f); }
inline void Vec3ConstructorXYZ(float x, float y, float z, glm::vec3 *self) { new(self) glm::vec3(x, y, z); }

// vec2 operators
inline void Vec2Add(const glm::vec2 &other, const glm::vec2 &self, glm::vec2 &out) { out = self + other; }
inline void Vec2Sub(const glm::vec2 &other, const glm::vec2 &self, glm::vec2 &out) { out = self - other; }
inline void Vec2Mul(float scalar, const glm::vec2 &self, glm::vec2 &out) { out = self * scalar; }
inline void Vec2Div(const glm::vec2 &other, const glm::vec2 &self, glm::vec2 &out) { out = self / other; }
inline bool Vec2Equal(const glm::vec2 &other, const glm::vec2 &self) { return self == other; }
inline void Vec2Constructor(glm::vec2 *self) { new(self) glm::vec2(0.0f); }
inline void Vec2ConstructorXY(float x, float y, glm::vec2 *self) { new(self) glm::vec2(x, y); }


inline void RegisterVec4(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType("vec4", sizeof(glm::vec4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK);
    AS_CHECK(r, "vec4 type");

    r = engine->RegisterObjectBehaviour(
        "vec4",
        asBEHAVE_CONSTRUCT,
        "void f()",
        asFUNCTION(Vec4Constructor),
        asCALL_CDECL_OBJLAST
    );
    AS_CHECK(r, "vec4 constructor");

    r = engine->RegisterObjectBehaviour(
        "vec4",
        asBEHAVE_CONSTRUCT,
        "void f(float, float, float, float)",
        asFUNCTION(Vec4ConstructorXYZW),
        asCALL_CDECL_OBJLAST
    );
    AS_CHECK(r, "vec4 constructor xyz");

    r = engine->RegisterObjectProperty("vec4", "float x", asOFFSET(glm::vec4, x));
    AS_CHECK(r, "vec4.x");
    r = engine->RegisterObjectProperty("vec4", "float y", asOFFSET(glm::vec4, y));
    AS_CHECK(r, "vec4.y");
    r = engine->RegisterObjectProperty("vec4", "float z", asOFFSET(glm::vec4, z));
    AS_CHECK(r, "vec4.z");
    r = engine->RegisterObjectProperty("vec4", "float w", asOFFSET(glm::vec4, w));
    AS_CHECK(r, "vec4.w");

    r = engine->RegisterObjectMethod("vec4", "vec4 opAdd(const vec4 &in) const", asFUNCTION(Vec4Add),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec4 add");
    r = engine->RegisterObjectMethod("vec4", "vec4 opSub(const vec4 &in) const", asFUNCTION(Vec4Sub),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec4 sub");
    r = engine->RegisterObjectMethod("vec4", "vec4 opMul(float) const", asFUNCTION(Vec4Mul), asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec4 mul");
    r = engine->RegisterObjectMethod("vec4", "vec4 opDiv(const vec4 &in) const", asFUNCTION(Vec4Div),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec4 div");
}

inline void RegisterVec3(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK);
    AS_CHECK(r, "vec3 type");

    r = engine->RegisterObjectBehaviour(
        "vec3",
        asBEHAVE_CONSTRUCT,
        "void f()",
        asFUNCTION(Vec3Constructor),
        asCALL_CDECL_OBJLAST
    );
    AS_CHECK(r, "vec3 constructor");

    r = engine->RegisterObjectBehaviour(
        "vec3",
        asBEHAVE_CONSTRUCT,
        "void f(float, float, float)",
        asFUNCTION(Vec3ConstructorXYZ),
        asCALL_CDECL_OBJLAST
    );
    AS_CHECK(r, "vec3 constructor xyz");

    r = engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
    AS_CHECK(r, "vec3.x");
    r = engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
    AS_CHECK(r, "vec3.y");
    r = engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
    AS_CHECK(r, "vec3.z");

    r = engine->RegisterObjectMethod("vec3", "vec3 opAdd(const vec3 &in) const", asFUNCTION(Vec3Add),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 add");
    r = engine->RegisterObjectMethod("vec3", "vec3 opSub(const vec3 &in) const", asFUNCTION(Vec3Sub),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 sub");
    r = engine->RegisterObjectMethod("vec3", "vec3 opMul(float) const", asFUNCTION(Vec3Mul), asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 mul");
    r = engine->RegisterObjectMethod("vec3", "vec3 opDiv(const vec3 &in) const", asFUNCTION(Vec3Div),
    asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec3 div");
}

inline void RegisterVec2(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType("vec2", sizeof(glm::vec2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK);
    AS_CHECK(r, "vec2 type");

    r = engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vec2Constructor),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec2 constructor");

    r = engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float x, float y)", asFUNCTION(Vec2ConstructorXY),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec2 constructor xy");

    r = engine->RegisterObjectProperty("vec2", "float x", asOFFSET(glm::vec2, x));
    AS_CHECK(r, "vec2.x");
    r = engine->RegisterObjectProperty("vec2", "float y", asOFFSET(glm::vec2, y));
    AS_CHECK(r, "vec2.y");

    r = engine->RegisterObjectMethod("vec2", "vec2 opAdd(const vec2 &in) const", asFUNCTION(Vec2Add),
    asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec2 add");
    r = engine->RegisterObjectMethod("vec2", "vec2 opSub(const vec2 &in) const", asFUNCTION(Vec2Sub),
        asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec2 sub");
    r = engine->RegisterObjectMethod("vec2", "vec2 opMul(float) const", asFUNCTION(Vec2Mul), asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec2 mul");
    r = engine->RegisterObjectMethod("vec2", "vec2 opDiv(const vec2 &in) const", asFUNCTION(Vec2Div),
    asCALL_CDECL_OBJLAST);
    AS_CHECK(r, "vec2 div");
}

inline void RegisterTransformComponent(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType(
        "Transform",
        0,
        asOBJ_REF | asOBJ_NOCOUNT
    );
    AS_CHECK(r, "Transform type");

    r = engine->RegisterObjectProperty(
        "Transform",
        "vec3 position",
        asOFFSET(c_Transform, position)
    );
    AS_CHECK(r, "Transform.position");

    r = engine->RegisterObjectProperty(
        "Transform",
        "vec3 scale",
        asOFFSET(c_Transform, scale)
    );
    AS_CHECK(r, "Transform.scale");

    r = engine->RegisterObjectProperty(
        "Transform",
        "vec3 rotation",
        asOFFSET(c_Transform, eulerHint)
    );
    AS_CHECK(r, "Transform.rotation");
}

inline void RegisterTagComponent(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType(
        "TagComponent",
        0,
        asOBJ_REF | asOBJ_NOCOUNT);
    AS_CHECK(r, "TagComponent type");

    r = engine->RegisterObjectProperty(
        "TagComponent",
        "string name",
        asOFFSET(c_Tag, name));
    AS_CHECK(r, "TagComponent.name");
}

inline void RegisterVisibilityComponent(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType(
        "VisibilityComponent",
        0,
        asOBJ_REF | asOBJ_NOCOUNT);
    AS_CHECK(r, "VisibilityComponent type");

    r = engine->RegisterObjectProperty(
        "VisibilityComponent",
        "bool isActive",
        asOFFSET(c_Visibility, isActive));
    AS_CHECK(r, "VisibilityComponent.isActive");
}

inline void RegisterCameraOrientationComponent(asIScriptEngine *engine) {
    int r;

    r = engine->RegisterObjectType(
        "CameraOrientation",
        0,
        asOBJ_REF | asOBJ_NOCOUNT);
    AS_CHECK(r, "CameraOrientation type");

    r = engine->RegisterObjectProperty(
        "CameraOrientation",
        "float yaw",
        asOFFSET(c_CameraOrientation, yaw));
    AS_CHECK(r, "CameraOrientation.yaw");

    r = engine->RegisterObjectProperty(
        "CameraOrientation",
        "float pitch",
        asOFFSET(c_CameraOrientation, pitch));
    AS_CHECK(r, "CameraOrientation.pitch");
}

inline void RegisterTypes(asIScriptEngine *engine) {
    RegisterVec4(engine);
    RegisterVec3(engine);
    RegisterVec2(engine);
    RegisterTransformComponent(engine);
    RegisterCameraOrientationComponent(engine);
    RegisterTagComponent(engine);
    RegisterVisibilityComponent(engine);
}