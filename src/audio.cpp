#include "audio.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstring>

namespace audio {

AudioManager::AudioManager()
    : initialized_(false)
    , device_(nullptr)
    , context_(nullptr)
    , masterVolume_(1.0f)
{
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::init() {
    if (initialized_) {
        return true;
    }

    // Open the default audio device
    device_ = alcOpenDevice(nullptr);
    if (!device_) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return false;
    }

    // Create audio context
    context_ = alcCreateContext(device_, nullptr);
    if (!context_) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        alcCloseDevice(device_);
        device_ = nullptr;
        return false;
    }

    // Make context current
    if (!alcMakeContextCurrent(context_)) {
        std::cerr << "Failed to make OpenAL context current" << std::endl;
        alcDestroyContext(context_);
        alcCloseDevice(device_);
        context_ = nullptr;
        device_ = nullptr;
        return false;
    }

    // Set listener properties
    ALfloat listenerPos[] = {0.0f, 0.0f, 0.0f};
    ALfloat listenerVel[] = {0.0f, 0.0f, 0.0f};
    ALfloat listenerOri[] = {0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};

    alListenerfv(AL_POSITION, listenerPos);
    alListenerfv(AL_VELOCITY, listenerVel);
    alListenerfv(AL_ORIENTATION, listenerOri);

    initialized_ = true;
    std::cout << "Audio system initialized successfully" << std::endl;
    return true;
}

void AudioManager::shutdown() {
    if (!initialized_) {
        return;
    }

    // Delete all sources
    for (ALuint source : sources_) {
        alDeleteSources(1, &source);
    }
    sources_.clear();

    // Delete all buffers
    for (auto& pair : buffers_) {
        alDeleteBuffers(1, &pair.second);
    }
    buffers_.clear();

    // Destroy context and close device
    alcMakeContextCurrent(nullptr);
    if (context_) {
        alcDestroyContext(context_);
        context_ = nullptr;
    }
    if (device_) {
        alcCloseDevice(device_);
        device_ = nullptr;
    }

    initialized_ = false;
    std::cout << "Audio system shut down" << std::endl;
}

bool AudioManager::loadSound(const std::string& name, const std::string& filepath) {
    if (!initialized_) {
        std::cerr << "Audio system not initialized" << std::endl;
        return false;
    }

    // Check if sound is already loaded
    if (buffers_.find(name) != buffers_.end()) {
        std::cerr << "Sound '" << name << "' already loaded" << std::endl;
        return true;
    }

    // Load WAV file
    int channels, sampleRate, bitsPerSample;
    std::vector<char> data;
    if (!loadWAV(filepath, channels, sampleRate, bitsPerSample, data)) {
        std::cerr << "Failed to load WAV file: " << filepath << std::endl;
        return false;
    }

    // Create buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);

    ALenum format = getALFormat(channels, bitsPerSample);
    alBufferData(buffer, format, data.data(), static_cast<ALsizei>(data.size()), sampleRate);

    // Check for errors
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error loading sound: " << error << std::endl;
        alDeleteBuffers(1, &buffer);
        return false;
    }

    buffers_[name] = buffer;
    std::cout << "Loaded sound: " << name << std::endl;
    return true;
}

ALuint AudioManager::playSound(const std::string& name, bool loop) {
    if (!initialized_) {
        std::cerr << "Audio system not initialized" << std::endl;
        return 0;
    }

    auto it = buffers_.find(name);
    if (it == buffers_.end()) {
        std::cerr << "Sound '" << name << "' not found" << std::endl;
        return 0;
    }

    // Create source
    ALuint source;
    alGenSources(1, &source);

    // Set source properties
    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second));
    alSourcef(source, AL_GAIN, masterVolume_);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);

    // Play the source
    alSourcePlay(source);

    sources_.push_back(source);
    return source;
}

void AudioManager::stopSound(ALuint sourceId) {
    if (!initialized_ || sourceId == 0) {
        return;
    }

    alSourceStop(sourceId);
    alDeleteSources(1, &sourceId);

    // Remove from sources vector
    auto it = std::find(sources_.begin(), sources_.end(), sourceId);
    if (it != sources_.end()) {
        sources_.erase(it);
    }
}

void AudioManager::setMasterVolume(float volume) {
    masterVolume_ = std::max(0.0f, std::min(1.0f, volume));
    
    // Update all playing sources
    for (ALuint source : sources_) {
        alSourcef(source, AL_GAIN, masterVolume_);
    }
}

bool AudioManager::loadWAV(const std::string& filepath, int& channels, int& sampleRate,
                           int& bitsPerSample, std::vector<char>& data) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open WAV file: " << filepath << std::endl;
        return false;
    }

    // Read RIFF header
    char riff[4];
    file.read(riff, 4);
    if (strncmp(riff, "RIFF", 4) != 0) {
        std::cerr << "Invalid WAV file: missing RIFF header" << std::endl;
        return false;
    }

    // Skip file size
    file.seekg(4, std::ios::cur);

    // Read WAVE format
    char wave[4];
    file.read(wave, 4);
    if (strncmp(wave, "WAVE", 4) != 0) {
        std::cerr << "Invalid WAV file: missing WAVE format" << std::endl;
        return false;
    }

    // Read fmt chunk
    char fmt[4];
    file.read(fmt, 4);
    if (strncmp(fmt, "fmt ", 4) != 0) {
        std::cerr << "Invalid WAV file: missing fmt chunk" << std::endl;
        return false;
    }

    // Read fmt chunk size
    int32_t fmtSize;
    file.read(reinterpret_cast<char*>(&fmtSize), sizeof(fmtSize));

    // Read audio format (1 = PCM)
    int16_t audioFormat;
    file.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));
    if (audioFormat != 1) {
        std::cerr << "Unsupported WAV format: only PCM is supported" << std::endl;
        return false;
    }

    // Read number of channels
    int16_t numChannels;
    file.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
    channels = numChannels;

    // Read sample rate
    int32_t sr;
    file.read(reinterpret_cast<char*>(&sr), sizeof(sr));
    sampleRate = sr;

    // Skip byte rate and block align
    file.seekg(6, std::ios::cur);

    // Read bits per sample
    int16_t bps;
    file.read(reinterpret_cast<char*>(&bps), sizeof(bps));
    bitsPerSample = bps;

    // Skip any extra format bytes
    if (fmtSize > 16) {
        file.seekg(fmtSize - 16, std::ios::cur);
    }

    // Find data chunk
    char chunkId[4];
    int32_t chunkSize;
    while (file.read(chunkId, 4)) {
        file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
        if (strncmp(chunkId, "data", 4) == 0) {
            break;
        }
        file.seekg(chunkSize, std::ios::cur);
    }

    if (strncmp(chunkId, "data", 4) != 0) {
        std::cerr << "Invalid WAV file: missing data chunk" << std::endl;
        return false;
    }

    // Read audio data
    data.resize(chunkSize);
    file.read(data.data(), chunkSize);

    return true;
}

ALenum AudioManager::getALFormat(int channels, int bitsPerSample) {
    if (channels == 1) {
        return (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else {
        return (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }
}

} // namespace audio
