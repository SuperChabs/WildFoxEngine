module;

#include <angelscript.h>
#include <string>

export module WFE.Scripting.ASRegisterAPI.Audio;

import WFE.ECS.Systems.AudioSystem;
import WFE.Core.Logger;

#define AS_CHECK(r, msg) if ((r) < 0) { Logger::Log(LogLevel::ERROR, std::string("AS Register failed: ") + msg + " code: " + std::to_string(r)); return; }

void PlayOneShot(AudioSystem* audioSystem, const std::string& path, float volume, float pitch)
{
    if (!audioSystem) return;
    audioSystem->PlayOneShot(path, volume, pitch);
}

export void RegisterAudio(asIScriptEngine* engine, AudioSystem* audioSystem)
{
    int r = engine->RegisterGlobalFunction(
        "void PlayOneShot(const string &in, float volume = 1.0f, float pitch = 1.0f)",
        asFUNCTIONPR(PlayOneShot, (AudioSystem*, const std::string&, float, float), void),
        asCALL_CDECL_OBJFIRST, audioSystem);
    AS_CHECK(r, "PlayOneShot");
}
