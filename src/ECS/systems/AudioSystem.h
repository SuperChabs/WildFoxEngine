#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#include "core/logging/Logger.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

struct WavHeader {
    char riff[4];
    uint32_t fileSize;
    char wave[4];
    char fmt[4];
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char data[4];
    uint32_t dataSize;
};

ALuint LoadWavFromDisk(const std::string &path);

static void SetListenerTransform(const TransformComponent &transform) {
    alListener3f(AL_POSITION, transform.position.x, transform.position.y, transform.position.z);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);

    glm::vec3 forward = transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = transform.rotation * glm::vec3(0.0f, 1.0f, 0.0f);
    float orientation[6] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
}

class AudioSystem {
    ALCdevice *m_device = nullptr;
    ALCcontext *m_context = nullptr;

    std::unordered_map<std::string, ALuint> m_bufferCache;

    std::vector<ALuint> m_oneShotSources;

public:
    bool Init();

    void Shutdown();

    void Update(ECSWorld *ecs);

    ALuint LoadBuffer(const std::string &path);

    void PlayOneShot(const std::string &path, float volume = 1.0f, float pitch = 1.0f);

private:
    void SyncListner(ECSWorld *ecs);

    void SyncSources(ECSWorld *ecs);

    void CleanUpOneShot();
};