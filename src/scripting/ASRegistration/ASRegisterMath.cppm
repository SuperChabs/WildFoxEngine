module;

#include <angelscript.h>

#include <string>
#include <cmath>

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

export module WFE.Scripting.ASRegisterAPI.Math;

import WFE.Core.Logger;

float FuncSqrt(float x)
{
    return std::sqrt(x);
}

float FuncCos(float x)
{
    return std::cos(x);
}

float FuncSin(float x)
{
    return std::sin(x);
}

float FuncATan2(float x, float y)
{
    return std::atan2(x, y);
}

float FuncAbs(float x)
{
    return std::abs(x);
}

export void RegisterMath(asIScriptEngine* engine)
{
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