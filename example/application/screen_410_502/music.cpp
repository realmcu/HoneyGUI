#ifdef _WIN32
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <SDL.h>
#include <exception>

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"
using namespace std;

// Custom exception class for MP3 player errors
class MP3PlayerException : public std::runtime_error
{
public:
    explicit MP3PlayerException(const std::string &message) : std::runtime_error(message) {}
};

// MP3Player class handles loading, initialization, and playback of MP3 files
class MP3Player
{
public:
    MP3Player() : dev(0), playing(false), data( {}), want(), have(), info(), buffer(), thread(),
              mutex(), cv(), load_flag()
    {

        // Initialize SDL; throw an exception if initialization fails
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            throw MP3PlayerException("Failed to initialize SDL: " + std::string(SDL_GetError()));
        }
    }

    ~MP3Player()
    {
        // Stop playback and clean up resources
        stop();
        if (thread.joinable())
        {
            thread.join();
        }

    }
    bool get_completion()
    {
        return completion;
    }
    bool load(const char *filename)
    {
        mp3dec_t mp3d;
        mp3dec_init(&mp3d);

        // Load MP3 file; return false if loading fails
        if (mp3dec_load(&mp3d, filename, &info, nullptr, nullptr))
        {
            cerr << "Failed to decode MP3 file" << endl;
            load_flag = false;
            return false;
        }

        // Point buffer to the decoded audio data
        buffer.reset(info.buffer);
        data = { buffer.get(), info.samples, 0 };
        load_flag = true;
        return true;
    }

    bool initSDL()
    {
        // Set up desired SDL audio specifications
        SDL_memset(&want, 0, sizeof(want));
        want.freq = info.hz;
        want.format = AUDIO_S16SYS;
        want.channels = info.channels;
        want.samples = 4096;
        want.callback = audio_callback;
        want.userdata = &data;

        // Open SDL audio device; return false if opening fails
        dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
        if (dev == 0)
        {
            cerr << "Failed to open audio device: " << SDL_GetError() << endl;
            return false;
        }

        return true;
    }

    void play()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!playing)
        {
            // Start playback in a new thread
            playing = true;
            thread = std::thread(&MP3Player::playAudio, this);
            completion = false;
            //load_flag = false;
        }
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            playing = false;
            completion = true;
        }
        cv.notify_one();
        if (dev)
        {
            SDL_CloseAudioDevice(dev);
            dev = 0;
        }
        // Ensure playback thread has exited
        if (thread.joinable())
        {
            thread.join();
        }

    }

    bool reload(const char *filename)
    {
        try
        {
            stop();  // Stop current playback
            if (thread.joinable())
            {
                thread.join();  // Ensure playback thread has exited
            }
            if (!load_flag && !load(filename))
            {
                throw MP3PlayerException("Failed to load MP3 file");
            }
            if (!initSDL())
            {
                throw MP3PlayerException("Failed to initialize SDL for new MP3 file");
            }
            play();  // Start playing the new audio file
            return true;
        }
        catch (const std::exception &ex)
        {
            cerr << "Error while reloading MP3 file: " << ex.what() << endl;
            return false;
        }
    }
    double getDuration() const
    {
        if (info.hz == 0 || info.channels == 0)
        {
            throw MP3PlayerException("Invalid sample rate or number of channels");
        }
        return static_cast<double>(info.samples) / (info.hz * info.channels);
    }
    double getCurrentTime() const
    {
        if (info.hz == 0 || info.channels == 0)
        {
            throw MP3PlayerException("Invalid sample rate or number of channels");
        }

        return static_cast<double>(data.position) / (info.hz * info.channels);
    }
private:
    // Struct to hold playback data
    struct PlaybackData
    {
        const int16_t *pcm;
        size_t samples;
        size_t position;
    };

    // SDL audio callback function
    static void audio_callback(void *userdata, Uint8 *stream, int len)
    {
        auto *data = (PlaybackData *)userdata;
        int16_t *output = (int16_t *)stream;
        int samples_to_copy = len / sizeof(int16_t); // sizeof(int16_t) = 2 bytes
        if (data->position < data->samples)
        {
            int remaining_samples = data->samples - data->position;
            if (samples_to_copy > remaining_samples)
            {
                samples_to_copy = remaining_samples;
            }
            memcpy(output, &data->pcm[data->position], samples_to_copy * sizeof(int16_t));
            data->position += samples_to_copy;
        }
        else
        {
            // If no more samples, fill the buffer with silence
            memset(output, 0, samples_to_copy * sizeof(int16_t));
            completion = true;
        }
    }

    // Main playback loop that runs in a separate thread
    void playAudio()
    {
        SDL_PauseAudioDevice(dev, 0); // Start playing
        std::unique_lock<std::mutex> lock(mutex);
        while (playing)
        {
            cv.wait_for(lock, std::chrono::milliseconds(100));
        }
        SDL_PauseAudioDevice(dev, 1); // Stop audio device

    }

    SDL_AudioDeviceID dev;  // SDL audio device ID
    SDL_AudioSpec want, have;  // SDL audio specifications
    mp3dec_file_info_t info;  // MP3 file information
    PlaybackData data;  // Current playback data
    std::unique_ptr<int16_t[]> buffer; // Pointer to the audio buffer
    std::thread thread;  // Thread for handling playback
    std::mutex mutex;  // Mutex for synchronization
    std::condition_variable cv;  // Condition variable for thread signaling
    bool playing;  // Flag to indicate if audio is playing
    bool load_flag;
    static bool completion;
};

// Static instance of MP3Player
static MP3Player player;
bool MP3Player::completion;
extern "C" {
    // C interface function to play music
    void win32_play_music(void *p)
    {
        const char *music_file = (const char *)p;
        try
        {
            if (!player.reload(music_file))
            {
                // return -1;  // Return -1 if reloading fails
            }

            // return 0;  // Success
            return;
        }
        catch (const std::exception &ex)
        {
            cerr << "Error playing music: " << ex.what() << endl;
            // return -1;  // Return -1 if exception occurs
        }
    }

    // C interface function to stop music
    void win32_stop_music(void)
    {
        player.stop();  // Stop playback
        // return 0;
    }

    // C interface function to check music completion status
    bool win32_music_completion_status(void)
    {
        return player.get_completion();
    }
    float win32_music_get_music_length(void)
    {
        return (float)player.getDuration();
    }
    float win32_music_get_music_current_time(void)
    {
        return (float)player.getCurrentTime();
    }
    void win32_load_music(void *p)
    {
        const char *music_file = (const char *)p;
        try
        {
            if (!player.load(music_file))
            {
                // return -1;  // Return -1 if reloading fails
            }
            return;
            // return 0;  // Success
        }
        catch (const std::exception &ex)
        {
            cerr << "Error playing music: " << ex.what() << endl;
            // return -1;  // Return -1 if exception occurs
        }
    }

}
#endif