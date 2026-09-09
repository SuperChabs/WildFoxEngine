#pragma once

#include <angelscript.h>
#include <string>

#include "core/logging/Logger.h"

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

inline void ScriptLog(const std::string &msg) { Logger::Log(LogLevel::SCRIPT, msg); }

inline void RegisterLogging(asIScriptEngine *engine) {
    int r = engine->RegisterGlobalFunction(
        "void Log(string massage)",
        asFUNCTIONPR(ScriptLog, (const std::string &), void),
        asCALL_CDECL);
    AS_CHECK(r, "Log");
}