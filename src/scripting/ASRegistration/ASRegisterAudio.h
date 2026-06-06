#pragma once

#include <angelscript.h>
#include <string>

#include "ECS/systems/AudioSystem.h"
#include "core/logging/Logger.h"

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

void PlayOneShot(AudioSystem *audioSystem, const std::string &path, float volume, float pitch) {
    if (!audioSystem) return;
    audioSystem->PlayOneShot(path, volume, pitch);
}

void RegisterAudio(asIScriptEngine *engine, AudioSystem *audioSystem) {
    int r = engine->RegisterGlobalFunction(
                "void PlayOneShot(const string &in, float volume = 1.0f, float pitch = 1.0f)",
                asFUNCTIONPR(PlayOneShot, (AudioSystem *, const std::string &, float, float), void),
            asCALL_CDECL_OBJFIRST, audioSystem
    )
    ;
    AS_CHECK(r, "PlayOneShot");
}