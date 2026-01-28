#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include "unistd.h"
#include "sdl_driver.h"


#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if 0
/* soc audio example: Get mp3 info from mp3 frame (to be tested) */

typedef enum
{
    MP3_MODE_STEREO = 0,
    MP3_MODE_JOINT_STEREO = 1,
    MP3_MODE_DUAL_CHANNEL = 2,
    MP3_MODE_MONO = 3
} T_AUDIO_MP3_CHANNEL_MODE;


#define AUDIO_LOCATION_FL  0x1  // Front Left
#define AUDIO_LOCATION_FR  0x2  // Front Right
#define AUDIO_LOCATION_FC  0x4  // Front Center

typedef struct t_audio_mp3_attr
{
    uint32_t                    sample_rate;
    T_AUDIO_MP3_CHANNEL_MODE    chann_mode;
    uint8_t                     version;
    uint8_t                     layer;
    uint32_t                    bitrate;
    uint32_t                    chann_location;
} T_AUDIO_MP3_ATTR;


// sample rate lookup table (Indices: Version, Rate Index)
// Version: 0=MPEG2.5, 1=reserved, 2=MPEG2, 3=MPEG1
static const uint32_t mp3_sample_rates[4][3] =
{
    { 11025, 12000, 8000  }, // MPEG 2.5
    { 0,     0,     0     }, // Reserved
    { 22050, 24000, 16000 }, // MPEG 2
    { 44100, 48000, 32000 }  // MPEG 1
};

// bit rate table (Indices: Version_L1/L2/L3, Bitrate Index)
// 0: MPEG1, Layer 1
// 1: MPEG1, Layer 2
// 2: MPEG1, Layer 3
// 3: MPEG2/2.5, Layer 1
// 4: MPEG2/2.5, Layer 2/3
static const uint32_t mp3_bitrates[5][16] =
{
    {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0}, // V1, L1
    {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0}, // V1, L2
    {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0}, // V1, L3
    {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0}, // V2, L1
    {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0} // V2, L2/L3
};

/**
 * @brief analysis mp3 attribute from frame
 *
 * @param frame_ptr  MP3 data pointer
 * @param attr      attr
 * @return int      0: successed, -1: invalid frame
 */
int parse_mp3_frame_header(const uint8_t *frame_ptr, T_AUDIO_MP3_ATTR *attr)
{
    if (!frame_ptr || !attr) { return -1; }

    memset(attr, 0, sizeof(T_AUDIO_MP3_ATTR));

    // 1. read 32-bit Header (Big Endian)
    uint32_t header = (uint32_t)frame_ptr[0] << 24 |
                      (uint32_t)frame_ptr[1] << 16 |
                      (uint32_t)frame_ptr[2] << 8  |
                      (uint32_t)frame_ptr[3];

    // 2. check Sync Word (11 bits, all 1s) -> 0xFFE0
    if ((header & 0xFFE00000) != 0xFFE00000)
    {
        return -1; // invalid
    }

    // --- decode key ---

    // Version ID (Bits 20, 19)
    // 00=2.5, 01=res, 10=2, 11=1
    uint8_t ver_idx = (header >> 19) & 0x3;
    if (ver_idx == 1) { return -1; } // Reserved version

    // Layer Description (Bits 18, 17)
    // 00=res, 01=L3, 10=L2, 11=L1
    uint8_t layer_idx = (header >> 17) & 0x3;
    if (layer_idx == 0) { return -1; } // Reserved layer

    // convert Layer to stander value: 3->1(L1), 2->2(L2), 1->3(L3)
    uint8_t layer_val = 4 - layer_idx;

    // Bitrate Index (Bits 15, 14, 13, 12)
    uint8_t br_idx = (header >> 12) & 0xF;
    if (br_idx == 15) { return -1; } // "bad" bitrate index

    // Sampling Rate Frequency Index (Bits 11, 10)
    uint8_t sr_idx = (header >> 10) & 0x3;
    if (sr_idx == 3) { return -1; } // Reserved sample rate

    // Channel Mode (Bits 7, 6)
    uint8_t mode_val = (header >> 6) & 0x3;

    // --- fill struct ---

    attr->version = ver_idx; //  0, 2, 3
    attr->layer   = layer_val;
    attr->chann_mode = (T_AUDIO_MP3_CHANNEL_MODE)mode_val;

    // lookup table Sample Rate
    attr->sample_rate = mp3_sample_rates[ver_idx][sr_idx];

    // lookup table Bitrate
    int br_table_idx = 0;
    if (ver_idx == 3)   // MPEG 1
    {
        br_table_idx = layer_idx - 1; // L1=0, L2=1, L3=2
    }
    else     // MPEG 2 or 2.5
    {
        if (layer_idx == 3) { br_table_idx = 3; } // L1
        else { br_table_idx = 4; } // L2 or L3
    }

    // table unit kbps, * 1000 convert to bit/s
    attr->bitrate = mp3_bitrates[br_table_idx][br_idx] * 1000;

    //  Channel Location
    if (attr->chann_mode == MP3_MODE_MONO)
    {
        attr->chann_location = AUDIO_LOCATION_FC;
    }
    else
    {
        // including Stereo, Joint Stereo, Dual Channel
        attr->chann_location = AUDIO_LOCATION_FL | AUDIO_LOCATION_FR;
    }

    return 0;
}

#endif


#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

typedef struct AudioContext AudioContext;

struct AudioContext
{
    SDL_AudioDeviceID device_id; // SDL audio device ID
    mp3dec_t mp3d;               // minimp3 decoder status

    uint8_t *stream_buffer;      // buffer to save MP3 data frame
    int buffer_capacity;         // mp3 buffer size
    int buffer_usage;            // mp3 buffer data processed

    int is_device_opened;        // flag: to open a sdl audio device
    int sample_rate;             // mp3 sample rate
    int channels;                // mp3 channels
};

static AudioContext *ctx;
static AudioContext *audio_create(int buffer_capacity)
{
    ctx = (AudioContext *)malloc(sizeof(AudioContext));
    if (!ctx)
    {
        return NULL;
    }

    ctx->stream_buffer = (uint8_t *)malloc(buffer_capacity);
    if (!ctx->stream_buffer)
    {
        free(ctx);
        return NULL;
    }

    ctx->buffer_capacity = buffer_capacity;
    ctx->buffer_usage = 0;
    ctx->device_id = 0;
    ctx->is_device_opened = 0;
    ctx->sample_rate = 0;
    ctx->channels = 0;

    mp3dec_init(&ctx->mp3d);

    return ctx;
}

static void audio_destroy(void)
{
    if (!ctx)
    {
        return;
    }

    if (ctx->device_id != 0)
    {
        SDL_CloseAudioDevice(ctx->device_id);
    }

    if (ctx->stream_buffer)
    {
        free(ctx->stream_buffer);
    }

    free(ctx);
}


static int audio_open_device(int hz, int channels)
{
    SDL_AudioSpec want, have;
    SDL_zero(want);

    want.freq = hz;
    want.format = AUDIO_S16SYS; // minimp3 output format
    want.channels = channels;
    want.samples = 4096;        // cache buffer
    want.callback = NULL;       //

    ctx->device_id = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (ctx->device_id == 0)
    {
        gui_log("[Audio] Failed to open device: %s\n", SDL_GetError());
        return -1;
    }

    ctx->sample_rate = have.freq;
    ctx->channels = have.channels;
    ctx->is_device_opened = 1;

    // wait data to play
    SDL_PauseAudioDevice(ctx->device_id, 0);
    gui_log("[Audio] Device Opened: %d Hz, %d Channels\n", have.freq, have.channels);
    return 0;
}

static void audio_push_chunk(const uint8_t *data, size_t len)
{
    if (!ctx || !data || len == 0) { return; }

    // 1. check buffer overflow
    if (ctx->buffer_usage + len > ctx->buffer_capacity)
    {
        // Full, reset
        gui_log("[Audio] Buffer Overflow! Resetting...\n");
        ctx->buffer_usage = 0;
    }

    // 2. copy data into buffer
    memcpy(ctx->stream_buffer + ctx->buffer_usage, data, len);
    ctx->buffer_usage += len;

    // 3. decode mp3 data
    mp3dec_frame_info_t info;
    short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
    int offset = 0;

    while (offset < ctx->buffer_usage)
    {
        // decoding
        int samples = mp3dec_decode_frame(
                          &ctx->mp3d,
                          ctx->stream_buffer + offset,
                          ctx->buffer_usage - offset,
                          pcm,
                          &info
                      );

        if (samples > 0)
        {
            // --- decode success ---

            // A. not open device yet, open now
            if (!ctx->is_device_opened)
            {
                // gui_get_audio()
                if (audio_open_device(info.hz, info.channels) < 0)
                {
                    // open failed
                    ctx->buffer_usage = 0;
                    return;
                }
            }

            // B. sending PCM data to SDL
            // byte = samples * channels * sizeof(short)
            int pcm_bytes = samples * info.channels * 2;
            if (SDL_QueueAudio(ctx->device_id, pcm, pcm_bytes) < 0)
            {
                // queue error
            }

            offset += info.frame_bytes;
        }
        else if (info.frame_bytes > 0)
        {
            offset += info.frame_bytes;
        }
        else
        {
            // waiting for more data
            break;
        }
    }

    // 4. update, memmove buffer data
    if (offset > 0)
    {
        int remaining = ctx->buffer_usage - offset;
        if (remaining > 0)
        {
            memmove(ctx->stream_buffer, ctx->stream_buffer + offset, remaining);
        }
        ctx->buffer_usage = remaining;
    }
}

static int audio_get_queued_duration_ms(void)
{
    if (!ctx || !ctx->is_device_opened)
    {
        return 0;
    }

    Uint32 bytes = SDL_GetQueuedAudioSize(ctx->device_id);
    int bytes_per_sec = ctx->sample_rate * ctx->channels * 2;

    if (bytes_per_sec == 0)
    {
        return 0;
    }
    return (bytes * 1000) / bytes_per_sec;
}

static struct gui_audiodev audio =
{

    .decode_chunk =  audio_push_chunk,

};



void gui_port_audio_init(void)
{
    AudioContext *audio_ctx = audio_create(32768);
    if (!audio_ctx)
    {
        return ;
    }

    gui_audio_info_register(&audio);

    gui_log("Audio Engine Initialized. Waiting for data...\n");

}