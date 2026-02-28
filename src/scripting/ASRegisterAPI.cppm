module;

#include <angelscript.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <cassert>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

export module WFE.Scripting.ASRegisterAPI;

import WFE.Scripting.ASState;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Input;
import WFE.Core.Logger;

export class ASRegisterAPI
{
public:
    static void RegisterAll(ECSWorld* ecs, Input* input)
    {
        asIScriptEngine* engine = ASState::Get();
        int r;

        r = engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec3>());
        assert(r >= 0);
        AS_CHECK(r, "vec3 type");
        engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vec3Constructor), asCALL_CDECL_OBJLAST);
        AS_CHECK(r, "vec3 constructor");
        engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(Vec3ConstructorXYZ), asCALL_CDECL_OBJLAST);
        AS_CHECK(r, "vec3 constructor xyz");
        engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
        AS_CHECK(r, "vec3.x");
        engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
        AS_CHECK(r, "vec3.y");
        engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
        AS_CHECK(r, "vec3.z");

        r = engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT);
        assert(r >= 0);
        AS_CHECK(r, "Transform type");
        engine->RegisterObjectProperty("Transform", "vec3 position", asOFFSET(TransformComponent, position));
        AS_CHECK(r, "Transform.position");
        engine->RegisterObjectProperty("Transform", "vec3 scale",    asOFFSET(TransformComponent, scale));
        AS_CHECK(r, "Transform.scale");

        engine->RegisterGlobalFunction(
            "bool IsValid(uint64 e)",
            asMETHOD(ECSWorld, IsValid), asCALL_THISCALL_ASGLOBAL, ecs);

        engine->RegisterGlobalFunction(
            "void DestroyEntity(uint64 e)",
            asMETHOD(ECSWorld, DestroyEntity), asCALL_THISCALL_ASGLOBAL, ecs);

        engine->RegisterGlobalFunction(
            "Transform@ GetTransform(uint64 e)",
            asFUNCTIONPR(GetTransform, (ECSWorld*, entt::entity), TransformComponent*),
            asCALL_CDECL_OBJFIRST, ecs);

        r = engine->RegisterEnum("Key");
        AS_CHECK(r, "Key enum");
        engine->RegisterEnumValue("Key", "KEY_W",     static_cast<int>(Key::KEY_W));
        engine->RegisterEnumValue("Key", "KEY_A",     static_cast<int>(Key::KEY_A));
        engine->RegisterEnumValue("Key", "KEY_S",     static_cast<int>(Key::KEY_S));
        engine->RegisterEnumValue("Key", "KEY_D",     static_cast<int>(Key::KEY_D));
        engine->RegisterEnumValue("Key", "KEY_SPACE", static_cast<int>(Key::KEY_SPACE));
        engine->RegisterEnumValue("Key", "KEY_ESCAPE",static_cast<int>(Key::KEY_ESCAPE));

        engine->RegisterGlobalFunction(
            "bool IsKeyPressed(Key key)",
            asFUNCTIONPR(IsKeyPressed, (Input*, Key), bool),
            asCALL_CDECL_OBJFIRST, input);

        engine->RegisterGlobalFunction(
            "bool IsKeyJustPressed(Key key)",
            asFUNCTIONPR(IsKeyJustPressed, (Input*, Key), bool),
            asCALL_CDECL_OBJFIRST, input);

        engine->RegisterGlobalFunction(
            "bool IsKeyReleased(Key key)",
            asFUNCTIONPR(IsKeyReleased, (Input*, Key), bool),
            asCALL_CDECL_OBJFIRST, input);

    }

private:
    static void Vec3Constructor(glm::vec3* self)
    {
        new(self) glm::vec3(0.0f);
    }

    static void Vec3ConstructorXYZ(float x, float y, float z, glm::vec3* self)
    {
        new(self) glm::vec3(x, y, z);
    }

    static TransformComponent* GetTransform(ECSWorld* ecs, entt::entity e)
    {
        if (!ecs->HasComponent<TransformComponent>(e))
            return nullptr;
        return &ecs->GetComponent<TransformComponent>(e);
    }

    static bool IsValid(ECSWorld* ecs, entt::entity e)
    {
        return ecs->IsValid(e);
    }

    static void DestroyEntity(ECSWorld* ecs, entt::entity e)
    {
        ecs->DestroyEntity(e);
    }

    static bool IsKeyPressed(Input* input, Key key)
    {
        return input->IsKeyPressed(key);
    }

    static bool IsKeyJustPressed(Input* input, Key key)
    {
        return input->IsKeyJustPressed(key);
    }

    static bool IsKeyReleased(Input* input, Key key)
    {
        return input->IsKeyReleased(key);
    }
};