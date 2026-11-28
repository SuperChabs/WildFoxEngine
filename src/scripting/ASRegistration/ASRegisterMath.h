#pragma once

#include <angelscript.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

#include "core/logging/Logger.h"

#define AS_CHECK(r, msg)                                                         \
    do {                                                                         \
        if ((r) < 0) {                                                           \
            Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ")     \
                        + (msg) + " code: " + std::to_string(r));                \
            return false;                                                        \
        }                                                                        \
    } while (0)

#define AS_REG(e, decl, fn)                                                                 \
    do {                                                                                    \
        int _r = (e)->RegisterGlobalFunction(decl.c_str(), asFUNCTION(fn), asCALL_CDECL);   \
        AS_CHECK(_r, decl);                                                                 \
    } while (0)

namespace MathFuncs {
    // generic arithmetic
    template<typename T> T Abs(T x)               { return x < T(0) ? -x : x; }
    template<typename T> T Min(T a, T b)          { return a < b ? a : b; }
    template<typename T> T Max(T a, T b)          { return a > b ? a : b; }
    template<typename T> T Clamp(T v, T lo, T hi) { return std::clamp(v, lo, hi); }

    // floating-point only
    inline float Sqrt(float x)             { return std::sqrt(x); }
    inline float Sin(float x)              { return std::sin(x); }
    inline float Cos(float x)              { return std::cos(x); }
    inline float Tan(float x)              { return std::tan(x); }
    inline float ASin(float x)             { return std::asin(std::clamp(x, -1.0f, 1.0f)); }
    inline float ACos(float x)             { return std::acos(std::clamp(x, -1.0f, 1.0f)); }
    inline float ATan(float x)             { return std::atan(x); }
    inline float ATan2(float y, float x)   { return std::atan2(y, x); }
    inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
}

template<typename V>
class VecFuncs {
public:
    static V Normalize(const V& v) {
        float l = glm::length(v);
        return l > 0.0f ? v / l : V(0.0f);
    }
    static float Length(const V& v) { return glm::length(v); }
    static float LengthSquared(const V& v) { return glm::dot(v, v); }
    static float Distance(const V& v1, const V& v2) { return glm::distance(v1, v2); }
    static float Dot(const V& v1, const V& v2) { return glm::dot(v1, v2); }
    static V Reflect(const V& v1, const V& v2) { return glm::reflect(v1, v2); }
    static V Refract(const V& i, const V& n, float eta) { return glm::refract(i, n, eta); }
    static V Project(const V& v, const V& onto) {
        float d = glm::dot(onto, onto);
        return d > 0.0f ? onto * (glm::dot(v, onto) / d) : V(0.0f);
    }
    static float Angle(const V &a, const V &b) {
        float d = glm::length(a) * glm::length(b);
        if (d <= 0.0f) return 0.0f;
        return std::acos(glm::clamp(glm::dot(a, b) / d, -1.0f, 1.0f));
    }
    static V Clamp(const V &v, float lo, float hi) { return glm::clamp(v, lo, hi); }
    static V FaceForward(const V &n, const V &i, const V &nref) { return glm::faceforward(n, i, nref); }
};

template<typename T>
bool RegisterNumeric(asIScriptEngine* e, const std::string& t) {
    int r;
    r = e->RegisterGlobalFunction((t + " abs(" + t + " x)").c_str(),
            asFUNCTION(MathFuncs::Abs<T>), asCALL_CDECL);
    AS_CHECK(r, "abs " + t);
    r = e->RegisterGlobalFunction((t + " min(" + t + " a, " + t + " b)").c_str(),
            asFUNCTION(MathFuncs::Min<T>), asCALL_CDECL);
    AS_CHECK(r, "min " + t);
    r = e->RegisterGlobalFunction((t + " max(" + t + " a, " + t + " b)").c_str(),
            asFUNCTION(MathFuncs::Max<T>), asCALL_CDECL);
    AS_CHECK(r, "max " + t);
    r = e->RegisterGlobalFunction((t + " clamp(" + t + " v, " + t + " lo, " + t + " hi)").c_str(),
            asFUNCTION(MathFuncs::Clamp<T>), asCALL_CDECL);
    AS_CHECK(r, "clamp " + t);
    return true;
}

template<typename V>
bool RegVecFuncs(asIScriptEngine *engine, const std::string& t) {
    using F = VecFuncs<V>;
    const std::string cv = "const " + t + " &in ";

    AS_REG(engine, (t + " normalize(" + cv + "V)"),                      F::Normalize);
    AS_REG(engine, ("float length(" + cv + "V)"),                         F::Length);
    AS_REG(engine, ("float lengthSqrt(" + cv + "V)"),                      F::LengthSquared);
    AS_REG(engine, ("float distance(" + cv + "A, " + cv + "B)"),            F::Distance);
    AS_REG(engine, ("float dot(" + cv + "A, " + cv + "B)"),                  F::Dot);
    AS_REG(engine, (t + " reflect(" + cv + "A, " + cv + "B)"),                F::Reflect);
    AS_REG(engine, (t + " refract(" + cv + "I, " + cv + "N, float eta)"),      F::Refract);
    AS_REG(engine, (t + " project(" + cv + "A, " + cv + "Onto)"),               F::Project);
    AS_REG(engine, ("float angle(" + cv + "A, " + cv + "B)"),                    F::Angle);
    AS_REG(engine, (t + " clamp(" + cv + ", float low, float high)"),             F::Clamp);
    AS_REG(engine, (t + " faceForward(" + cv + "N, " + cv + "I, " + cv + "NRef)"), F::FaceForward);
    return true;
}

inline bool RegisterFloatFunc(asIScriptEngine* e, const char* decl, const asSFuncPtr& f)
{
    int r = e->RegisterGlobalFunction(decl, f, asCALL_CDECL);
    AS_CHECK(r, decl);
    return true;
}

inline void RegisterMath(asIScriptEngine* engine)
{
    // arithmetic
    if (!RegisterNumeric<int>(engine,    "int"))    return;
    if (!RegisterNumeric<float>(engine,  "float"))  return;
    if (!RegisterNumeric<double>(engine, "double")) return;

    // power, trigonometry, inverse trigonometry, interpolation
    if (!RegisterFloatFunc(engine, "float sqrt(float x)",                   asFUNCTION(MathFuncs::Sqrt)))  return;
    if (!RegisterFloatFunc(engine, "float sin(float x)",                    asFUNCTION(MathFuncs::Sin)))   return;
    if (!RegisterFloatFunc(engine, "float cos(float x)",                    asFUNCTION(MathFuncs::Cos)))   return;
    if (!RegisterFloatFunc(engine, "float tan(float x)",                    asFUNCTION(MathFuncs::Tan)))   return;
    if (!RegisterFloatFunc(engine, "float asin(float x)",                   asFUNCTION(MathFuncs::ASin)))  return;
    if (!RegisterFloatFunc(engine, "float acos(float x)",                   asFUNCTION(MathFuncs::ACos)))  return;
    if (!RegisterFloatFunc(engine, "float atan(float x)",                   asFUNCTION(MathFuncs::ATan)))  return;
    if (!RegisterFloatFunc(engine, "float atan2(float y, float x)",         asFUNCTION(MathFuncs::ATan2))) return;
    if (!RegisterFloatFunc(engine, "float lerp(float a, float b, float t)", asFUNCTION(MathFuncs::Lerp)))  return;


    // vectorial Operations
    if (!RegVecFuncs<glm::vec2>(engine, "vec2")) return;
    if (!RegVecFuncs<glm::vec3>(engine, "vec3")) return;
    if (!RegVecFuncs<glm::vec4>(engine, "vec4")) return;
}