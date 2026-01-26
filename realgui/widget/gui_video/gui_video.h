/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI___VIDEO___H__
#define __GUI___VIDEO___H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "draw_img.h"
#include "gui_img.h"
#include "gui_fb.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

#define GUI_VIDEO_REPEAT_INFINITE      0xFFFFFFFF

typedef enum
{
    GUI_VIDEO_EVENT_FRAME_UPDATE,
    GUI_VIDEO_EVENT_PLAY,
    GUI_VIDEO_EVENT_PAUSE,
    GUI_VIDEO_EVENT_STOP,
    GUI_VIDEO_EVENT_END

} GUI_VIDEO_EVENT;

typedef enum
{
    GUI_VIDEO_STATE_ERR,
    GUI_VIDEO_STATE_INIT,
    GUI_VIDEO_STATE_PLAYING,
    GUI_VIDEO_STATE_PAUSE,
    GUI_VIDEO_STATE_STOP,
} GUI_VIDEO_STATE;

typedef enum
{
    GUI_IMG_LIVE_SUCCESSED,
    // GUI_IMG_LIVE_s,

} GUI_VIDEO_RES;

typedef enum
{
    MJPEG_SCAN_INIT = 0,
    MJPEG_SCAN_START,
    MJPEG_SCAN_END,
    MJPEG_SCAN_EOF,

} VIDEO_MJPEG_SCAN_STATE; // Widget Internal use only

typedef enum
{
    VIDEO_TYPE_NULL,
    VIDEO_TYPE_MJPEG,
    VIDEO_TYPE_H264,
    VIDEO_TYPE_AVI,

} GUI_VIDEO_TYPE;

typedef enum
{
    CHUNK_VIDEO = 0,
    CHUNK_AUDIO,
    CHUNK_UNKNOWN,

} AVI_CHUNK_TYPE;
#pragma pack(1)
typedef struct
{
    char symbol[4]; // "H264"
    uint32_t w;
    uint32_t h;
    uint32_t frame_num;
    uint32_t frame_time;
    uint32_t size;
} gui_h264_header_t;

/* AVI RIFF header*/
typedef struct
{
    char symbol[4]; // "RIFF"
    uint32_t size;
    char format[4]; // "AVI_"
} gui_riff_header_t;

/* AVI main header */
typedef struct
{
    uint32_t usec_per_frame;
    uint32_t max_byte_rate;
    uint32_t reserved_0;
    uint32_t flags;
    uint32_t total_frame;
    uint32_t initial_frame;
    uint32_t streams;
    uint32_t buffer_size;
    uint32_t width;
    uint32_t height;
} MainAVIHeader_t;

typedef struct
{
    char type[4];               /* "vids", "auds" */
    uint32_t handler;           /* Optionally, contains a FOURCC for a specific data handler. */
    uint32_t flags;             /*  */
    uint32_t priority;          /*  */
    uint32_t initial_frames;    /*  */
    uint32_t scale;             /*  */
    uint32_t rate;              /*  */
    uint32_t start;             /*  */
    uint32_t length;            /*  */
    uint32_t buffer_size;       /* This should be larger than the largest chunk, zero if unknown. */
    uint32_t quality;           /* A value between 0 and 10,000. */
    uint32_t sample_size;       /* The size of a sample; zero if varying, block align for audio. */
    uint16_t frame[4];             /* The left, top, right, bottom coordinates in 16 bit values. */
    uint32_t stream_format;     /* The letters "strf" indicate that this is a stream format. */
    uint32_t length_format;     /* This size of the format. */
} AVIStreamHeader_t;

typedef struct
{
    uint32_t size;              /*  */
    uint32_t width;             /*  */
    uint32_t height;            /*  */
    uint16_t planes;            /*  */
    uint16_t bit_count;         /*  */
    uint32_t compression;       /*  */
    uint32_t image_size;        /*  */
    uint32_t x_pels_per_meter;  /*  */
    uint32_t y_pels_per_meter;  /*  */
    uint32_t colors_used;       /*  */
    uint32_t colors_important;  /*  */
} BitMapInfoHeader_t;

typedef struct
{
    uint16_t format_tag;        /*  85 (0x55), 'WAVE_FORMAT_MPEGLAYER3' */
    uint16_t channels;          /*  The number of channels of audio data. */
    uint32_t samples_per_sec;   /*  The number of samples per second. */
    uint32_t ave_bytes_per_sec; /*  (Samples Per Sec)*(Block Align) */
    uint16_t block_align;       /*  (Channels * BitsPerSample) / 8 */
    uint16_t bits_per_sample;   /*  This should be 8 or 16 for PCM. */
    uint16_t size_extra;        /*  The size of the extra information. */
    // uint8_t junk[4];            /* Begins with the label "JUNK", size 1722 */

} WaveFormateX_t;

/* AVI idx1 */
typedef struct
{
    uint32_t indexID;              /*  */
    uint32_t index_size;           /*  */
} IndexList_t;

typedef struct
{
    uint32_t chunk_ID;         /*  */
    uint32_t flags;            /*  */
    uint32_t offset;           /*  */
    uint32_t size;             /*  */
} IndexItem_t;

#pragma pack()

/* AVI chunk slice */
typedef struct
{
    uint32_t offset;          /*  */
    uint32_t len;
    uint32_t type;           /*  */
} AviMoviChunk_t;

/** @brief  stb img widget information structure */
typedef struct
{
    gui_obj_t base;
    gui_img_t *img;

    void *data;                  // mem address or filesystem path
    uint32_t num_frame;
    uint8_t **array;             // mjpeg: jpeg address array, need free when destory widget
    gui_rgb_data_head_t header;  // for image prepare

    uint8_t *frame_buff;
    uint8_t *frame_buff_raw;
    void *decoder;
    uint32_t frame_time;
    uint32_t frame_step;
    int32_t frame_cur;
    int32_t frame_last;        // for cache management
    int32_t repeat_cnt;
    uint8_t img_type;
    uint8_t storage_type;

    uint8_t state;             // video play status

    uint32_t frame_chunk_cur;
    uint32_t chunk_num;


    uint8_t rgb_type;           // to define RGB type of decoded img (not used yet)
} gui_video_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define IMG_LIVE_FPS (25)  // default 25 fps, 40 ms



/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @brief Retrieves the video file size from a memory address or FTL storage address.
 *
 * @param[in] addr         The starting address of the video data.
 *                         - For IMG_SRC_MEMADDR: Pointer to the data in RAM or Flash.
 *                         - For IMG_SRC_FTL: Storage address or offset in Flash.
 * @param[in] storage_type The storage source type.
 *                         Supported types: IMG_SRC_MEMADDR, IMG_SRC_FTL.
 *
 * @return int
 *         Returns the file size in bytes. Returns 0 or a negative value on failure.
 *
 * @note
 * 1. For file paths, please use the file system (FS) API to get the file size.
 * 2. Supported video formats: mjpeg, avi, h264.
 */
int gui_video_get_filesize_from_addr(void *addr, uint8_t storage_type);

void gui_video_set_frame_rate(gui_video_t *this, float fps);

uint32_t gui_video_get_frame_time(gui_video_t *this);

void gui_video_set_scale(gui_video_t *this, float scale_x, float scale_y);

/**
 * @brief Set the playback state of a video widget.
 *
 * @param this Pointer to the video widget (must be valid).
 * @param state Target state, as defined by GUI_VIDEO_STATE (e.g., Play, Pause, Stop).
 *
 * @note When set state Play at state Stop, widget will play from the beginning.
 */
void gui_video_set_state(gui_video_t *this, GUI_VIDEO_STATE state);


/**
 * @brief Set the repeat (loop) count for video playback.
 *
 * @param this Pointer to the video widget (must be valid).
 * @param cnt Number of times to repeat playback after the first run:
 *          - cnt > 0: Play the video cnt additional times (total plays = 1 + cnt).
 *          - cnt = 0: Do not repeat (play once).
 *          - GUI_VIDEO_REPEAT_INFINITE: Repeat indefinitely (infinite loop).
 */
void gui_video_set_repeat_count(gui_video_t *this, int32_t cnt);

GUI_VIDEO_STATE gui_video_get_state(gui_video_t *this);

void gui_video_set_frame_step(gui_video_t *this, uint32_t step);

uint32_t gui_video_get_frame_step(gui_video_t *this);

void gui_video_refresh_size(gui_video_t *this);

void gui_video_refresh_type(gui_video_t *this);



/**
 * @brief Create a video widget from an FTL source(cannot be accessed directly) and attach it to a parent container.
 *
 * @param parent Pointer to the parent GUI object (must be valid).
 * @param name Widget name/identifier.
 * @param addr Pointer to the FTL source handle or buffer.
 * @param x Top-left x position relative to the parent.
 * @param y Top-left y position relative to the parent.
 * @param w Width in pixels (will be overwritten by the video file).
 * @param h Height in pixels (will be overwritten by the video file).
 *
 * @return Pointer to gui_video_t on success; NULL on failure.
 *
 */
gui_video_t *gui_video_create_from_ftl(void           *parent,
                                       const char     *name,
                                       void           *addr,
                                       int16_t         x,
                                       int16_t         y,
                                       int16_t         w,
                                       int16_t         h);

/**
 * @brief Create a video widget from a file system and attach it to a parent container.
 *
 * @param parent Pointer to the parent GUI object (must be valid).
 * @param name Widget name/identifier.
 * @param addr Pointer to the file path string.
 * @param x Top-left x position relative to the parent.
 * @param y Top-left y position relative to the parent.
 * @param w Width in pixels (will be overwritten by video file).
 * @param h Height in pixels (will be overwritten by video file).
 *
 * @return Pointer to gui_video_t on success; NULL on failure.
 *
 */
gui_video_t *gui_video_create_from_fs(void           *parent,
                                      const char     *name,
                                      void           *addr,
                                      int16_t         x,
                                      int16_t         y,
                                      int16_t         w,
                                      int16_t         h);

/**
 * @brief Create a video widget from a memory buffer and attach it to a parent container.
 *
 * @param parent Pointer to the parent GUI object (must be valid).
 * @param name Widget name/identifier.
 * @param addr Pointer to the memory buffer containing video data.
 * @param x Top-left x position relative to the parent.
 * @param y Top-left y position relative to the parent.
 * @param w Width in pixels (will be overwritten by video file).
 * @param h Height in pixels (will be overwritten by video file).
 *
 * @return Pointer to gui_video_t on success; NULL on failure.
 *
 */
gui_video_t *gui_video_create_from_mem(void           *parent,
                                       const char     *name,
                                       void           *addr,
                                       int16_t         x,
                                       int16_t         y,
                                       int16_t         w,
                                       int16_t         h);



#ifdef __cplusplus
}
#endif

#endif
