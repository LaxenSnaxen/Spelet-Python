#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace audio {

/**
 * @brief Audio manager class for handling OpenAL audio playback
 * 
 * This class provides a simple interface for initializing the audio system,
 * loading sounds, and playing them.
 */
class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    // Prevent copying
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    /**
     * @brief Initialize the audio system
     * @return true if initialization was successful, false otherwise
     */
    bool init();

    /**
     * @brief Shutdown the audio system and release all resources
     */
    void shutdown();

    /**
     * @brief Load a WAV file as a sound buffer
     * @param name Identifier for the sound
     * @param filepath Path to the WAV file
     * @return true if loading was successful, false otherwise
     */
    bool loadSound(const std::string& name, const std::string& filepath);

    /**
     * @brief Play a loaded sound
     * @param name Identifier of the sound to play
     * @param loop Whether to loop the sound
     * @return Source ID for the playing sound, or 0 on failure
     */
    ALuint playSound(const std::string& name, bool loop = false);

    /**
     * @brief Stop a playing sound
     * @param sourceId The source ID returned by playSound
     */
    void stopSound(ALuint sourceId);

    /**
     * @brief Set the master volume
     * @param volume Volume level (0.0 to 1.0)
     */
    void setMasterVolume(float volume);

    /**
     * @brief Check if the audio system is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return initialized_; }

private:
    bool initialized_;
    ALCdevice* device_;
    ALCcontext* context_;
    std::unordered_map<std::string, ALuint> buffers_;
    std::vector<ALuint> sources_;
    float masterVolume_;

    /**
     * @brief Load a WAV file and return raw audio data
     * @param filepath Path to the WAV file
     * @param channels Output: number of channels
     * @param sampleRate Output: sample rate
     * @param bitsPerSample Output: bits per sample
     * @param data Output: raw audio data
     * @return true if loading was successful, false otherwise
     */
    bool loadWAV(const std::string& filepath, int& channels, int& sampleRate, 
                 int& bitsPerSample, std::vector<char>& data);

    /**
     * @brief Get the OpenAL format for the given audio parameters
     * @param channels Number of channels
     * @param bitsPerSample Bits per sample
     * @return OpenAL format constant
     */
    ALenum getALFormat(int channels, int bitsPerSample);
};

} // namespace audio

#endif // AUDIO_HPP
