#pragma once

#include <AL/al.h>
#include <string>

struct AudioSourceComponent {
    std::string path;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool loop = false;
    bool spatical = true;
    bool playOnStart = false;

    ALuint _alSource = 0;
    bool _playing = false;
};

struct AudioListenerComponent {
    AudioListenerComponent() = default;
};