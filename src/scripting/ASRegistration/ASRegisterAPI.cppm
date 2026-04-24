module;

#include <angelscript.h>
#include <scriptbuilder.h>
#include <scriptstdstring.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include <string>
#include <cmath>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

export module WFE.Scripting.ASRegisterAPI;

import WFE.Scripting.ASState;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Input;
import WFE.Core.Logger;
import WFE.Core.CommandManager;

struct Vec3
{
    float x, y, z;
};

export class ASRegisterAPI
{
public:
    static void RegisterAll(ECSWorld* ecs, Input* input)
    {
        asIScriptEngine* engine = ASState::Get();

        RegisterStdString(engine);

        int r;

        r = engine->RegisterObjectType("vec3", sizeof(glm::vec3),
            asOBJ_VALUE |
            asOBJ_POD |
            asOBJ_APP_CLASS_CDAK
        );
        assert(r >= 0);
        AS_CHECK(r, "vec3 type");

        r = engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vec3Constructor), asCALL_CDECL_OBJLAST);
        AS_CHECK(r, "vec3 constructor");

        r = engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(Vec3ConstructorXYZ), asCALL_CDECL_OBJLAST);
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

        r = engine->RegisterObjectType("Transform", 0, asOBJ_REF | asOBJ_NOCOUNT);
        assert(r >= 0);
        AS_CHECK(r, "Transform type");

        r = engine->RegisterObjectProperty("Transform", "vec3 position", asOFFSET(TransformComponent, position));
        AS_CHECK(r, "Transform.position");

        r = engine->RegisterObjectProperty("Transform", "vec3 scale", asOFFSET(TransformComponent, scale));
        AS_CHECK(r, "Transform.scale");

        r = engine->RegisterObjectProperty("Transform", "vec3 rotation", asOFFSET(TransformComponent, eulerHint));
        AS_CHECK(r, "Transform.rotation");

        r = engine->RegisterGlobalFunction(
            "bool IsValid(uint64 e)",
            asMETHOD(ECSWorld, IsValid), asCALL_THISCALL_ASGLOBAL, ecs);
        AS_CHECK(r, "IsValid");

        r = engine->RegisterGlobalFunction(
            "void DestroyEntity(uint64 e)",
            asMETHOD(ECSWorld, DestroyEntity), asCALL_THISCALL_ASGLOBAL, ecs);
        AS_CHECK(r, "DestroyEntity");

        r = engine->RegisterGlobalFunction(
            "Transform@ GetTransform(uint64 e)",
            asFUNCTIONPR(GetTransform, (ECSWorld*, entt::entity), TransformComponent*),
            asCALL_CDECL_OBJFIRST, ecs);
        AS_CHECK(r, "GetTransform");

        r = engine->RegisterEnum("Key");
        AS_CHECK(r, "Key enum");

        r = engine->RegisterEnumValue("Key", "KEY_W", static_cast<int>(Key::KEY_W));
        AS_CHECK(r, "KEY_W");

        r = engine->RegisterEnumValue("Key", "KEY_A", static_cast<int>(Key::KEY_A));
        AS_CHECK(r, "KEY_A");

        r = engine->RegisterEnumValue("Key", "KEY_S", static_cast<int>(Key::KEY_S));
        AS_CHECK(r, "KEY_S");

        r = engine->RegisterEnumValue("Key", "KEY_D", static_cast<int>(Key::KEY_D));
        AS_CHECK(r, "KEY_D");

        r = engine->RegisterEnumValue("Key", "KEY_SPACE", static_cast<int>(Key::KEY_SPACE));
        AS_CHECK(r, "KEY_SPACE");

        r = engine->RegisterEnumValue("Key", "KEY_ESCAPE", static_cast<int>(Key::KEY_ESCAPE));
        AS_CHECK(r, "KEY_ESCAPE");

        r = engine->RegisterEnumValue("Key", "KEY_LEFT_SHIFT", static_cast<int>(Key::KEY_LEFT_SHIFT));
        AS_CHECK(r, "KEY_LEFT_SHIFT");

        r = engine->RegisterGlobalFunction(
            "bool IsKeyPressed(Key key)",
            asFUNCTIONPR(IsKeyPressed, (Input*, Key), bool),
            asCALL_CDECL_OBJFIRST, input);
        AS_CHECK(r, "IsKeyPressed");

        r = engine->RegisterGlobalFunction(
            "bool IsKeyJustPressed(Key key)",
            asFUNCTIONPR(IsKeyJustPressed, (Input*, Key), bool),
            asCALL_CDECL_OBJFIRST, input);
        AS_CHECK(r, "IsKeyJustPressed");

        r = engine->RegisterGlobalFunction(
            "bool IsKeyReleased(Key key)",
            asFUNCTIONPR(IsKeyReleased, (Input*, Key), bool),
            asCALL_CDECL_OBJFIRST, input);
        AS_CHECK(r, "IsKeyReleased");

        r = engine->RegisterGlobalFunction(
            "uint64 GetEntityByName(const string &in name)",
            asFUNCTIONPR(GetEntityByName, (ECSWorld*, const std::string&), entt::entity),
            asCALL_CDECL_OBJFIRST, ecs);
        AS_CHECK(r, "GetEntityByName");

        r = engine->RegisterGlobalFunction(
            "float sqrt(float x)", 
            asFUNCTION(FuncSqrt), 
            asCALL_CDECL);
        AS_CHECK(r, "sqrt");

        r = engine->RegisterGlobalFunction(
            "float sin(float x)", 
            asFUNCTION(FuncSin), 
            asCALL_CDECL);
        AS_CHECK(r, "sin");

        r = engine->RegisterGlobalFunction(
            "float cos(float x)", 
            asFUNCTION(FuncCos), 
            asCALL_CDECL);
        AS_CHECK(r, "cos");

        r = engine->RegisterGlobalFunction(
            "float atan2(float x, float y)", 
            asFUNCTION(FuncATan2), 
            asCALL_CDECL);
        AS_CHECK(r, "atan2");

        r = engine->RegisterGlobalFunction(
        "bool IsValidEntity(uint64 e)",
        asFUNCTIONPR(IsValid, (ECSWorld*, entt::entity), bool),
        asCALL_CDECL_OBJFIRST, ecs);

        RegisterCommands(ecs);
    }

private:
    static float FuncSqrt(float x)
    {
        return std::sqrt(x);
    }

    static float FuncCos(float x)
    {
        return std::cos(x);
    }

    static float FuncSin(float x)
    {
        return std::sin(x);
    }

    static float FuncATan2(float x, float y)
    {
        return std::atan2(x, y);
    }

    static void Vec3Add(const glm::vec3& other, const glm::vec3& self, glm::vec3& out)
    {
        out = self + other;
    }

    static void Vec3Sub(const glm::vec3& other, const glm::vec3& self, glm::vec3& out)
    {
        out = self - other;
    }

    static void Vec3Mul(float scalar, const glm::vec3& self, glm::vec3& out)
    {
        out = self * scalar;
    }

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

    static void DispatchTrigger(ECSWorld* ecs, const std::string& fnDecl,
                                entt::entity a, entt::entity b)
    {
        auto tryCall = [&](entt::entity self, entt::entity other)
        {
            if (!ecs->HasComponent<ScriptComponent>(self)) return;
            auto& script = ecs->GetComponent<ScriptComponent>(self);
            if (!script.loaded || !script.ctx || !script.module) return;

            asIScriptFunction* fn = script.module->GetFunctionByDecl(fnDecl.c_str());
            if (!fn) return;

            script.ctx->Prepare(fn);
            script.ctx->SetArgQWord(0, static_cast<asQWORD>(other));
            script.ctx->Execute();
        };

        tryCall(a, b);
        tryCall(b, a);
    }

    static void RegisterCommands(ECSWorld* ecs)
    {
        CommandManager::RegisterCommand("OnTriggerEnter",
            [ecs](const CommandArgs& args)
            {
                entt::entity a = std::get<entt::entity>(args[0]);
                entt::entity b = std::get<entt::entity>(args[1]);
                DispatchTrigger(ecs, "void OnTriggerEnter(uint64)", a, b);
            });

        CommandManager::RegisterCommand("OnTriggerExit",
            [ecs](const CommandArgs& args)
            {
                entt::entity a = std::get<entt::entity>(args[0]);
                entt::entity b = std::get<entt::entity>(args[1]);
                DispatchTrigger(ecs, "void OnTriggerExit(uint64)", a, b);
            });
    }

    static entt::entity GetEntityByName(ECSWorld* ecs, const std::string& name)
    {
        entt::entity result = entt::null;
        ecs->Each<TagComponent>([&](entt::entity e, TagComponent& tag)
        {
            if (tag.name == name)
                result = e;
        });
        return result;
    }
};