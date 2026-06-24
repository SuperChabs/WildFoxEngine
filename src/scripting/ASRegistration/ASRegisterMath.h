#pragma once

#include <angelscript.h>

#include <string>
#include <cmath>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

#include "core/logging/Logger.h"

inline float FuncSqrt(float x) {
    return std::sqrt(x);
}

inline float FuncCos(float x) {
    return std::cos(x);
}

inline float FuncSin(float x) {
    return std::sin(x);
}

inline float FuncATan2(float x, float y) {
    return std::atan2(x, y);
}

inline float FuncAbs(float x) {
    return std::abs(x);
}

inline void RegisterMath(asIScriptEngine *engine) {
    int r;

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
        "float abs(float x)",
        asFUNCTION(FuncAbs),
        asCALL_CDECL);
    AS_CHECK(r, "abs");
}