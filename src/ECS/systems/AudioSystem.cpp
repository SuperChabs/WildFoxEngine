#include "AudioSystem.h"
#include <entt/entt.hpp>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <stdexcept>

bool AudioSystem::Init() {
    m_device = alcOpenDevice(nullptr);
    if (!m_device) return false;

    m_context = alcCreateContext(m_device, nullptr);
    if (!alcMakeContextCurrent(m_context)) return false;

    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);

    return true;
}

void AudioSystem::Shutdown() {
    for (auto &[_, buf]: m_bufferCache)
        alDeleteBuffers(1, &buf);

    for (auto &src: m_oneShotSources)
        alDeleteSources(1, &src);

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

void AudioSystem::Update(ECSWorld *ecs) {
    SyncListner(ecs);
    SyncSources(ecs);
    CleanUpOneShot();
}

ALuint AudioSystem::LoadBuffer(const std::string &path) {
    auto it = m_bufferCache.find(path);
    if (it != m_bufferCache.end()) return it->second;

    ALuint buf = LoadWavFromDisk(path);
    m_bufferCache[path] = buf;
    return buf;
}

void AudioSystem::PlayOneShot(const std::string &path, float volume, float pitch) {
    if (path.empty()) return;

    ALuint buffer = LoadBuffer(path);
    ALuint source = 0;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));
    alSourcef(source, AL_GAIN, volume);
    alSourcef(source, AL_PITCH, pitch);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcePlay(source);

    m_oneShotSources.push_back(source);
}

void AudioSystem::SyncListner(ECSWorld *ecs) {
    bool listenerFound = false;
    ecs->Each<AudioListenerComponent, TransformComponent>([&](entt::entity, TransformComponent &transform) {
        if (!listenerFound) {
            SetListenerTransform(transform);
            listenerFound = true;
        }
    });

    if (!listenerFound) {
        TransformComponent defaultTransform;
        SetListenerTransform(defaultTransform);
    }
}

void AudioSystem::SyncSources(ECSWorld *ecs) {
    ecs->Each<AudioSourceComponent>([&](entt::entity entity, AudioSourceComponent &audio) {
        const TransformComponent *transform = nullptr;
        if (ecs->HasComponent<TransformComponent>(entity))
            transform = &ecs->GetComponent<TransformComponent>(entity);

        if (audio.path.empty())
            return;

        if (audio._alSource == 0) {
            alGenSources(1, &audio._alSource);
            ALuint buffer = LoadBuffer(audio.path);
            alSourcei(audio._alSource, AL_BUFFER, static_cast<ALint>(buffer));
        } else {
            ALint currentBuffer = 0;
            alGetSourcei(audio._alSource, AL_BUFFER, &currentBuffer);
            ALuint expectedBuffer = LoadBuffer(audio.path);
            if (static_cast<ALuint>(currentBuffer) != expectedBuffer)
                alSourcei(audio._alSource, AL_BUFFER, static_cast<ALint>(expectedBuffer));
        }

        alSourcef(audio._alSource, AL_GAIN, audio.volume);
        alSourcef(audio._alSource, AL_PITCH, audio.pitch);
        alSourcei(audio._alSource, AL_LOOPING, audio.loop ? AL_TRUE : AL_FALSE);
        alSource3f(audio._alSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

        if (audio.spatical && transform) {
            alSourcei(audio._alSource, AL_SOURCE_RELATIVE, AL_FALSE);
            alSource3f(audio._alSource, AL_POSITION, transform->position.x, transform->position.y,
                       transform->position.z);
        } else {
            alSourcei(audio._alSource, AL_SOURCE_RELATIVE, AL_TRUE);
            alSource3f(audio._alSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
        }

        if (audio.playOnStart && !audio._playing) {
            alSourcePlay(audio._alSource);
            audio._playing = true;
        } else if (audio._playing) {
            ALint state = 0;
            alGetSourcei(audio._alSource, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING) {
                if (audio.loop)
                    alSourcePlay(audio._alSource);
                else
                    audio._playing = false;
            }
        }
    });
}

void AudioSystem::CleanUpOneShot() {
    m_oneShotSources.erase(
        std::remove_if(
            m_oneShotSources.begin(),
            m_oneShotSources.end(),
            [&](ALuint source) {
                ALint state = 0;
                alGetSourcei(source, AL_SOURCE_STATE, &state);
                if (state != AL_PLAYING) {
                    alDeleteSources(1, &source);
                    return true;
                }
                return false;
            }),
        m_oneShotSources.end());
}

ALuint LoadWavFromDisk(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        Logger::Log(LogLevel::ERROR, "AudioSystem: cannot open " + path);
        throw std::runtime_error("AudioSystem: cannot open " + path);
    }

    WavHeader header{};
    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    if (std::strncmp(header.riff, "RIFF", 4) != 0 || std::strncmp(header.wave, "WAVE", 4) != 0) {
        Logger::Log(LogLevel::ERROR, "AudioSystem: invalid WAV file " + path);
        throw std::runtime_error("AudioSystem: invalid WAV file " + path);
    }

    std::vector<char> data(header.dataSize);
    file.read(data.data(), header.dataSize);

    ALenum format;
    if (header.channels == 1 && header.bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (header.channels == 1 && header.bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (header.channels == 2 && header.bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (header.channels == 2 && header.bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
        throw std::runtime_error("AudioSystem: unsupported WAV format " + path);

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data.data(),
                 static_cast<ALsizei>(header.dataSize),
                 static_cast<ALsizei>(header.sampleRate));

    Logger::Log(LogLevel::INFO, "AudioSystem: open " + path);

    return buffer;
}