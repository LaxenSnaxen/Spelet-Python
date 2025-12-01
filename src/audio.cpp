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

void AudioManager::cleanupFinishedSources() {
    if (!initialized_) {
        return;
    }

    // Remove finished (stopped) sources to prevent unbounded memory growth
    auto it = sources_.begin();
    while (it != sources_.end()) {
        ALint state;
        alGetSourcei(*it, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            alDeleteSources(1, &(*it));
            it = sources_.erase(it);
        } else {
            ++it;
        }
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
    // Note: WAV files use little-endian byte order. This code assumes a little-endian
    // system (x86, x64, ARM in little-endian mode). For big-endian systems, byte
    // swapping would be required for multi-byte values.
    
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open WAV file: " << filepath << std::endl;
        return false;
    }

    // Get file size for bounds checking
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Minimum WAV header size (44 bytes for standard PCM)
    if (fileSize < 44) {
        std::cerr << "Invalid WAV file: file too small" << std::endl;
        return false;
    }

    // Read RIFF header
    char riff[4];
    file.read(riff, 4);
    if (!file || strncmp(riff, "RIFF", 4) != 0) {
        std::cerr << "Invalid WAV file: missing RIFF header" << std::endl;
        return false;
    }

    // Read and validate file size from header
    int32_t headerFileSize;
    file.read(reinterpret_cast<char*>(&headerFileSize), sizeof(headerFileSize));
    if (!file) {
        std::cerr << "Invalid WAV file: truncated header" << std::endl;
        return false;
    }

    // Read WAVE format
    char wave[4];
    file.read(wave, 4);
    if (!file || strncmp(wave, "WAVE", 4) != 0) {
        std::cerr << "Invalid WAV file: missing WAVE format" << std::endl;
        return false;
    }

    // Read fmt chunk
    char fmt[4];
    file.read(fmt, 4);
    if (!file || strncmp(fmt, "fmt ", 4) != 0) {
        std::cerr << "Invalid WAV file: missing fmt chunk" << std::endl;
        return false;
    }

    // Read fmt chunk size
    int32_t fmtSize;
    file.read(reinterpret_cast<char*>(&fmtSize), sizeof(fmtSize));
    if (!file || fmtSize < 16 || fmtSize > 1024) {
        std::cerr << "Invalid WAV file: invalid fmt chunk size" << std::endl;
        return false;
    }

    // Read audio format (1 = PCM)
    int16_t audioFormat;
    file.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));
    if (!file || audioFormat != 1) {
        std::cerr << "Unsupported WAV format: only PCM is supported" << std::endl;
        return false;
    }

    // Read number of channels
    int16_t numChannels;
    file.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
    if (!file || numChannels < 1 || numChannels > 2) {
        std::cerr << "Invalid WAV file: unsupported number of channels" << std::endl;
        return false;
    }
    channels = numChannels;

    // Read sample rate
    int32_t sr;
    file.read(reinterpret_cast<char*>(&sr), sizeof(sr));
    if (!file || sr <= 0) {
        std::cerr << "Invalid WAV file: invalid sample rate" << std::endl;
        return false;
    }
    sampleRate = sr;

    // Skip byte rate and block align
    file.seekg(6, std::ios::cur);
    if (!file) {
        std::cerr << "Invalid WAV file: truncated header" << std::endl;
        return false;
    }

    // Read bits per sample
    int16_t bps;
    file.read(reinterpret_cast<char*>(&bps), sizeof(bps));
    if (!file || (bps != 8 && bps != 16)) {
        std::cerr << "Invalid WAV file: unsupported bits per sample" << std::endl;
        return false;
    }
    bitsPerSample = bps;

    // Skip any extra format bytes
    if (fmtSize > 16) {
        file.seekg(fmtSize - 16, std::ios::cur);
        if (!file) {
            std::cerr << "Invalid WAV file: truncated format chunk" << std::endl;
            return false;
        }
    }

    // Find data chunk with bounds checking
    char chunkId[4];
    int32_t chunkSize = 0;
    bool foundData = false;
    
    while (file.read(chunkId, 4)) {
        if (!file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize))) {
            break;
        }
        
        if (strncmp(chunkId, "data", 4) == 0) {
            foundData = true;
            break;
        }
        
        // Validate chunk size before seeking
        std::streamoff currentPos = file.tellg();
        if (chunkSize < 0 || currentPos < 0 || 
            chunkSize > fileSize - currentPos) {
            std::cerr << "Invalid WAV file: chunk size exceeds file bounds" << std::endl;
            return false;
        }
        
        file.seekg(chunkSize, std::ios::cur);
    }

    if (!foundData) {
        std::cerr << "Invalid WAV file: missing data chunk" << std::endl;
        return false;
    }

    // Validate data chunk size (avoid overflow by subtracting instead of adding)
    std::streamoff dataPos = file.tellg();
    if (chunkSize <= 0 || dataPos < 0 || chunkSize > fileSize - dataPos) {
        std::cerr << "Invalid WAV file: data chunk size exceeds file bounds" << std::endl;
        return false;
    }

    // Read audio data
    data.resize(static_cast<size_t>(chunkSize));
    file.read(data.data(), chunkSize);
    
    if (!file) {
        std::cerr << "Failed to read WAV audio data" << std::endl;
        return false;
    }

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
