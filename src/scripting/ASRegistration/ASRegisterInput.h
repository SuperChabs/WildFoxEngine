#pragma once

#include <angelscript.h>

#include <string>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

#include "core/Input.h"
#include "core/logging/Logger.h"

inline bool IsKeyPressed(Input *input, Key key) {
    return input->IsKeyPressed(key);
}

inline bool IsKeyJustPressed(Input *input, Key key) {
    return input->IsKeyJustPressed(key);
}

inline bool IsKeyReleased(Input *input, Key key) {
    return input->IsKeyReleased(key);
}

inline void RegisterInput(asIScriptEngine *engine, Input *input) {
    int r;

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
        asFUNCTIONPR(IsKeyPressed, (Input *, Key), bool),
        asCALL_CDECL_OBJFIRST, input);
    AS_CHECK(r, "IsKeyPressed");

    r = engine->RegisterGlobalFunction(
        "bool IsKeyJustPressed(Key key)",
        asFUNCTIONPR(IsKeyJustPressed, (Input *, Key), bool),
        asCALL_CDECL_OBJFIRST, input);
    AS_CHECK(r, "IsKeyJustPressed");

    r = engine->RegisterGlobalFunction(
        "bool IsKeyReleased(Key key)",
        asFUNCTIONPR(IsKeyReleased, (Input *, Key), bool),
        asCALL_CDECL_OBJFIRST, input);
    AS_CHECK(r, "IsKeyReleased");
}