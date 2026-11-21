#include "../core/base.as"

// Plays a sound when something enters the trigger.
// NOTE: PlayOneShot is not registered yet (see ASRegisterAudio.h), so this script won't build until it is.

string soundPath = "../assets/sounds/meow.wav";
float  volume    = 1.0f;
float  pitch     = 1.0f;

void OnTriggerEnter(uint64 other)
{
    PlayOneShot(soundPath, volume, pitch);
}
