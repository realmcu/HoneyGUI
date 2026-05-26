/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include <math.h>
#include "gui_msv1.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_api.h"
#include "gui_vfs.h"

/*============================================================================*
 *                         Types (AVI container)
 *============================================================================*/

/* Re-declare only what is needed; mirrors gui_video.h packed structs. */
#pragma pack(1)

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
} msv1_avi_main_hdr_t;

typedef struct
{
    char     type[4];            /* "vids" or "auds" */
    uint32_t handler;
    uint32_t flags;
    uint32_t priority;
    uint32_t initial_frames;
    uint32_t scale;
    uint32_t rate;
    uint32_t start;
    uint32_t length;
    uint32_t buffer_size;
    uint32_t quality;
    uint32_t sample_size;
    uint16_t frame[4];
    uint32_t stream_format;      /* "strf" as uint32 */
    uint32_t length_format;
} msv1_avi_stream_hdr_t;

typedef struct
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;          /* 8 or 16 for MSV1 */
    uint32_t compression;        /* "CRAM" or "MSVC" */
    uint32_t image_size;
    uint32_t x_pels_per_meter;
    uint32_t y_pels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} msv1_bmp_info_hdr_t;

typedef struct
{
    uint32_t chunk_ID;           /* "00dc" = 0x63643030, "01wb" = 0x62773130 */
    uint32_t flags;
    uint32_t offset;
    uint32_t size;
} msv1_idx1_item_t;

typedef struct
{
    uint8_t  b;
    uint8_t  g;
    uint8_t  r;
    uint8_t  reserved;
} msv1_rgbquad_t;

#pragma pack()

/* AVI chunk descriptor built at init time */
typedef struct
{
    uint32_t offset;             /* byte offset from start of AVI data */
    uint32_t len;                /* byte count of compressed data */
    uint32_t type;               /* CHUNK_VIDEO | CHUNK_AUDIO | CHUNK_UNKNOWN */
} msv1_chunk_t;

#define MSV1_CHUNK_VIDEO   0u
#define MSV1_CHUNK_AUDIO   1u
#define MSV1_CHUNK_UNKNOWN 2u

/* FourCC helpers */
#define MSV1_FOURCC_00DC 0x63643030u  /* "00dc" - video stream */
#define MSV1_FOURCC_01WB 0x62773130u  /* "01wb" - audio stream */

/*============================================================================*
 *                         Macros
 *============================================================================*/

#define MSV1_DEFAULT_FPS   24u         /* fallback if AVI header looks wrong */
#define MSV1_MAX_PALETTE   256u

/*============================================================================*
 *                           Private: Helpers
 *============================================================================*/

/** Convert a BMP-style RGBQUAD entry to RGB565. */
static inline uint16_t rgbquad_to_rgb565(const msv1_rgbquad_t *q)
{
    uint16_t r = (q->r >> 3) & 0x1Fu;
    uint16_t g = (q->g >> 2) & 0x3Fu;
    uint16_t b = (q->b >> 3) & 0x1Fu;
    return (uint16_t)((r << 11) | (g << 5) | b);
}

/** Check for MSV1 (CRAM / MSVC) FourCC. */
static bool is_msv1_fourcc(uint32_t fcc)
{
    /* "CRAM" = 0x4D415243, "MSVC" = 0x4356534D, "wmsv" = 0x76736D77 */
    return (fcc == 0x4D415243u || fcc == 0x4356534Du || fcc == 0x76736D77u);
}

static void gui_msv1_reset(gui_msv1_t *this)
{
    this->frame_cur       = -1;
    this->frame_last      = -1;
    this->frame_chunk_cur = 0;
}

/*============================================================================*
 *                       Private: AVI Index Builder
 *
 *  Builds the msv1_chunk_t[] that maps frame_cur -> (data_offset, data_len).
 *  Three almost-identical code paths handle: MEMADDR, FILESYS, FTL.
 *  A lightweight cursor + read abstraction keeps the logic DRY.
 *============================================================================*/

/* -- cursor helpers for in-memory parsing ----------------------------------- */

typedef struct
{
    const uint8_t *base;
    uint32_t       pos;
    uint32_t       size;
} mem_cursor_t;

static inline void mem_read(mem_cursor_t *c, void *dst, uint32_t n)
{
    if (c->pos + n <= c->size)
    {
        memcpy(dst, c->base + c->pos, n);
    }
    c->pos += n;
}

static inline void mem_skip(mem_cursor_t *c, uint32_t n)
{
    c->pos += n;
}

/* -- cursor helpers for FTL parsing ----------------------------------------- */

typedef struct
{
    uint32_t base;   /* FTL base address */
    uint32_t pos;
} ftl_cursor_t;

static inline void ftl_read(ftl_cursor_t *c, void *dst, uint32_t n)
{
    gui_ftl_read(c->base + c->pos, (uint8_t *)dst, n);
    c->pos += n;
}

static inline void ftl_skip(ftl_cursor_t *c, uint32_t n)
{
    c->pos += n;
}

/* -- shared post-parsing: build chunk array from idx1 ----------------------- */
/* Returns 0 on success, -1 on failure. */
static int build_chunk_array_from_idx1(gui_msv1_t   *this,
                                       gui_obj_t    *obj,
                                       uint32_t      movi_data_beacon,
                                       const void   *idx1_data,
                                       uint32_t      idx1_size)
{
    uint32_t chunk_num = idx1_size / sizeof(msv1_idx1_item_t);
    msv1_chunk_t *arr = (msv1_chunk_t *)gui_malloc(chunk_num * sizeof(msv1_chunk_t));
    if (!arr)
    {
        gui_log("gui_msv1: chunk array alloc failed\n");
        return -1;
    }

    this->chunks    = arr;
    this->chunk_num = chunk_num;

    const msv1_idx1_item_t *items = (const msv1_idx1_item_t *)idx1_data;
    uint32_t frame_cnt = 0;

    for (uint32_t i = 0; i < chunk_num; i++)
    {
        arr[i].offset = movi_data_beacon + items[i].offset + 8u;
        arr[i].len    = items[i].size;
        arr[i].type   = (items[i].chunk_ID == MSV1_FOURCC_00DC) ? MSV1_CHUNK_VIDEO
                        : (items[i].chunk_ID == MSV1_FOURCC_01WB) ? MSV1_CHUNK_AUDIO
                        : MSV1_CHUNK_UNKNOWN;

        if (arr[i].type == MSV1_CHUNK_VIDEO)
        {
            frame_cnt++;
        }
    }

    obj->w            = this->header.w;
    obj->h            = this->header.h;
    this->num_frame   = frame_cnt;

    gui_log("gui_msv1: %d frames, %dx%d, %u ms/frame\n",
            this->num_frame, obj->w, obj->h, this->frame_time);
    return 0;
}

/*============================================================================*
 *              Private: AVI Init -- MEMADDR path
 *============================================================================*/
static int msv1_src_init_mem(gui_msv1_t *this)
{
    gui_obj_t    *obj      = (gui_obj_t *)this;
    const uint8_t *img_data = (const uint8_t *)this->data;

    /* -- determine file size directly from raw bytes before cursor init -- */
    uint32_t riff_size;
    memcpy(&riff_size, img_data + 4, 4);
    uint32_t file_size = riff_size + 8u;

    mem_cursor_t c;
    c.base = img_data;
    c.pos  = 12;           /* skip "RIFF"(4) + riff_size(4) + "AVI "(4) */
    c.size = file_size;

    /* -- LIST hdrl -- */
    uint8_t  sig[5]       = {0};
    uint8_t  list_type[5] = {0};
    uint8_t  id[5]        = {0};
    uint32_t hdrl_size    = 0;
    uint32_t chunk_size   = 0;

    mem_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("msv1: no LIST\n"); return -1; }
    mem_read(&c, &hdrl_size, 4);
    uint32_t hdrl_data_beacon = c.pos;
    mem_read(&c, list_type, 4);
    if (memcmp(list_type, "hdrl", 4) != 0) { gui_log("msv1: no hdrl\n"); return -1; }

    /* -- avih -- */
    mem_read(&c, id, 4);
    if (memcmp(id, "avih", 4) != 0) { gui_log("msv1: no avih\n"); return -1; }
    mem_read(&c, &chunk_size, 4);
    msv1_avi_main_hdr_t main_hdr;
    mem_read(&c, &main_hdr, sizeof(main_hdr));
    mem_skip(&c, chunk_size - (uint32_t)sizeof(main_hdr));

    this->frame_time = (main_hdr.usec_per_frame > 0)
                       ? (main_hdr.usec_per_frame / 1000u)
                       : (1000u / MSV1_DEFAULT_FPS);

    /* -- LIST strl (video) -- */
    mem_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("msv1: no strl\n"); return -1; }
    uint32_t stream_list_size = 0;
    mem_read(&c, &stream_list_size, 4);

    mem_read(&c, list_type, 4);
    if (memcmp(list_type, "strl", 4) != 0) { gui_log("msv1: no strl type\n"); return -1; }

    mem_read(&c, id, 4);
    if (memcmp(id, "strh", 4) != 0) { gui_log("msv1: no strh\n"); return -1; }
    mem_read(&c, &chunk_size, 4);
    msv1_avi_stream_hdr_t video_strh;
    mem_read(&c, &video_strh, sizeof(video_strh));

    /* -- strf (BITMAPINFOHEADER + optional palette) -- */
    memcpy(id, &video_strh.stream_format, 4);
    if (memcmp(id, "strf", 4) != 0) { gui_log("msv1: no strf\n"); return -1; }
    chunk_size = video_strh.length_format;

    msv1_bmp_info_hdr_t bmp_hdr;
    mem_read(&c, &bmp_hdr, sizeof(bmp_hdr));

    if (!is_msv1_fourcc(bmp_hdr.compression))
    {
        gui_log("msv1: unsupported codec 0x%x\n", bmp_hdr.compression);
        return -1;
    }
    this->bits_per_pixel = (uint8_t)bmp_hdr.bit_count;

    /* Read palette for 8-bpp */
    if (this->bits_per_pixel == 8)
    {
        uint32_t pal_count = bmp_hdr.colors_used ? bmp_hdr.colors_used : 256u;
        if (pal_count > MSV1_MAX_PALETTE) { pal_count = MSV1_MAX_PALETTE; }
        for (uint32_t i = 0; i < pal_count; i++)
        {
            msv1_rgbquad_t q;
            mem_read(&c, &q, sizeof(q));
            this->palette565[i] = rgbquad_to_rgb565(&q);
        }
    }

    /* fill header fields now that we know w/h */
    this->header.w = (int16_t)bmp_hdr.width;
    this->header.h = (int16_t)bmp_hdr.height;
    obj->w         = (int16_t)bmp_hdr.width;
    obj->h         = (int16_t)bmp_hdr.height;

    /* -- find movi LIST -- */
    uint32_t movi_size = 0;
    uint32_t movi_data_beacon = 0;
    c.pos = hdrl_data_beacon + hdrl_size;

    while (c.pos < file_size)
    {
        uint32_t blk_size = 0;
        mem_read(&c, sig, 4);
        mem_read(&c, &blk_size, 4);
        if (memcmp(sig, "JUNK", 4) == 0)
        {
            mem_skip(&c, blk_size);
        }
        else if (memcmp(sig, "LIST", 4) == 0)
        {
            mem_read(&c, list_type, 4);
            if (memcmp(list_type, "movi", 4) == 0)
            {
                movi_size = blk_size;
                movi_data_beacon = c.pos - 4u;
                break;
            }
            mem_skip(&c, blk_size - 4u);
        }
        else
        {
            mem_skip(&c, blk_size);
        }
    }
    if (c.pos >= file_size) { gui_log("msv1: no movi\n"); return -1; }

    /* -- idx1 -- */
    c.pos = movi_data_beacon + movi_size;
    mem_read(&c, sig, 4);
    if (memcmp(sig, "idx1", 4) != 0) { gui_log("msv1: no idx1\n"); return -1; }
    uint32_t idx1_size = 0;
    mem_read(&c, &idx1_size, 4);

    return build_chunk_array_from_idx1(this, obj,
                                       movi_data_beacon,
                                       img_data + c.pos,
                                       idx1_size);
}

/*============================================================================*
 *              Private: AVI Init -- FTL path
 *============================================================================*/
static int msv1_src_init_ftl(gui_msv1_t *this)
{
    gui_obj_t  *obj = (gui_obj_t *)this;
    ftl_cursor_t c;
    c.base = (uint32_t)(uintptr_t)this->data;
    c.pos  = 0;

    uint8_t  sig[5]       = {0};
    uint8_t  list_type[5] = {0};
    uint8_t  id[5]        = {0};
    uint32_t chunk_size   = 0;

    /* file size from RIFF header */
    uint32_t riff_size = 0;
    c.pos = 4;
    ftl_read(&c, &riff_size, 4);
    uint32_t file_size = riff_size + 8u;
    c.pos = 12;

    /* LIST hdrl */
    ftl_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("msv1 ftl: no LIST\n"); return -1; }
    uint32_t hdrl_size = 0;
    ftl_read(&c, &hdrl_size, 4);
    uint32_t hdrl_data_beacon = c.pos;
    ftl_read(&c, list_type, 4);
    if (memcmp(list_type, "hdrl", 4) != 0) { gui_log("msv1 ftl: no hdrl\n"); return -1; }

    /* avih */
    ftl_read(&c, id, 4);
    if (memcmp(id, "avih", 4) != 0) { gui_log("msv1 ftl: no avih\n"); return -1; }
    ftl_read(&c, &chunk_size, 4);
    msv1_avi_main_hdr_t main_hdr;
    ftl_read(&c, &main_hdr, sizeof(main_hdr));
    ftl_skip(&c, chunk_size - (uint32_t)sizeof(main_hdr));

    this->frame_time = (main_hdr.usec_per_frame > 0)
                       ? (main_hdr.usec_per_frame / 1000u)
                       : (1000u / MSV1_DEFAULT_FPS);

    /* LIST strl (video) */
    ftl_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("msv1 ftl: no strl\n"); return -1; }
    uint32_t stream_list_size = 0;
    ftl_read(&c, &stream_list_size, 4);
    (void)stream_list_size;
    ftl_read(&c, list_type, 4);
    if (memcmp(list_type, "strl", 4) != 0) { gui_log("msv1 ftl: no strl type\n"); return -1; }

    ftl_read(&c, id, 4);
    if (memcmp(id, "strh", 4) != 0) { gui_log("msv1 ftl: no strh\n"); return -1; }
    ftl_read(&c, &chunk_size, 4);
    msv1_avi_stream_hdr_t video_strh;
    ftl_read(&c, &video_strh, sizeof(video_strh));

    memcpy(id, &video_strh.stream_format, 4);
    if (memcmp(id, "strf", 4) != 0) { gui_log("msv1 ftl: no strf\n"); return -1; }
    chunk_size = video_strh.length_format;

    msv1_bmp_info_hdr_t bmp_hdr;
    ftl_read(&c, &bmp_hdr, sizeof(bmp_hdr));

    if (!is_msv1_fourcc(bmp_hdr.compression))
    {
        gui_log("msv1 ftl: unsupported codec 0x%x\n", bmp_hdr.compression);
        return -1;
    }
    this->bits_per_pixel = (uint8_t)bmp_hdr.bit_count;

    if (this->bits_per_pixel == 8)
    {
        uint32_t pal_count = bmp_hdr.colors_used ? bmp_hdr.colors_used : 256u;
        if (pal_count > MSV1_MAX_PALETTE) { pal_count = MSV1_MAX_PALETTE; }
        for (uint32_t i = 0; i < pal_count; i++)
        {
            msv1_rgbquad_t q;
            ftl_read(&c, &q, sizeof(q));
            this->palette565[i] = rgbquad_to_rgb565(&q);
        }
    }

    this->header.w = (int16_t)bmp_hdr.width;
    this->header.h = (int16_t)bmp_hdr.height;
    obj->w         = (int16_t)bmp_hdr.width;
    obj->h         = (int16_t)bmp_hdr.height;

    /* find movi */
    uint32_t movi_size = 0;
    uint32_t movi_data_beacon = 0;
    c.pos = hdrl_data_beacon + hdrl_size;

    while (c.pos < file_size)
    {
        uint32_t blk_size = 0;
        ftl_read(&c, sig, 4);
        ftl_read(&c, &blk_size, 4);
        if (memcmp(sig, "JUNK", 4) == 0)
        {
            ftl_skip(&c, blk_size);
        }
        else if (memcmp(sig, "LIST", 4) == 0)
        {
            ftl_read(&c, list_type, 4);
            if (memcmp(list_type, "movi", 4) == 0)
            {
                movi_size = blk_size;
                movi_data_beacon = c.pos - 4u;
                break;
            }
            ftl_skip(&c, blk_size - 4u);
        }
        else
        {
            ftl_skip(&c, blk_size);
        }
    }
    if (c.pos >= file_size) { gui_log("msv1 ftl: no movi\n"); return -1; }

    /* idx1 */
    c.pos = movi_data_beacon + movi_size;
    ftl_read(&c, sig, 4);
    if (memcmp(sig, "idx1", 4) != 0) { gui_log("msv1 ftl: no idx1\n"); return -1; }
    uint32_t idx1_size = 0;
    ftl_read(&c, &idx1_size, 4);

    /* load idx1 into a temp buffer and build array */
    msv1_idx1_item_t *idx_buf = (msv1_idx1_item_t *)gui_malloc(idx1_size);
    if (!idx_buf) { gui_log("msv1 ftl: idx1 malloc failed\n"); return -1; }
    gui_ftl_read(c.base + c.pos, (uint8_t *)idx_buf, idx1_size);

    int ret = build_chunk_array_from_idx1(this, obj,
                                          movi_data_beacon,
                                          idx_buf, idx1_size);
    gui_free(idx_buf);
    return ret;
}

/*============================================================================*
 *              Private: AVI Init -- Filesystem path
 *============================================================================*/
static int msv1_src_init_fs(gui_msv1_t *this)
{
    /* Declare all locals at top of function for Keil AC5 (C89) compatibility.
     * Mixing declarations after goto targets triggers warning #546-D. */
    gui_obj_t             *obj              = (gui_obj_t *)this;
    const char            *fn               = (const char *)this->data;
    gui_vfs_file_t        *fp;
    int                    file_size;
    uint8_t                sig[5]           = {0};
    uint8_t                list_type[5]     = {0};
    uint8_t                id[5]            = {0};
    uint32_t               chunk_size       = 0;
    uint32_t               hdrl_size        = 0;
    int                    hdrl_data_beacon = 0;
    msv1_avi_main_hdr_t    main_hdr;
    uint32_t               stream_list_size = 0;
    msv1_avi_stream_hdr_t  video_strh;
    msv1_bmp_info_hdr_t    bmp_hdr;
    uint32_t               pal_count;
    uint32_t               i;
    uint32_t               movi_size        = 0;
    int                    movi_data_beacon = 0;
    int                    pos;
    uint32_t               idx1_size        = 0;
    msv1_idx1_item_t      *idx_buf          = NULL;
    int                    ret;

    fp = gui_vfs_open(fn, GUI_VFS_READ);
    if (!fp) { gui_log("msv1 fs: open failed %s\n", fn); return -1; }

    file_size = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_END);
    if (file_size <= 0) { gui_vfs_close(fp); return -1; }

    /* skip RIFF header (12 bytes) */
    gui_vfs_seek(fp, 12, GUI_VFS_SEEK_SET);

    /* LIST hdrl */
    gui_vfs_read(fp, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &hdrl_size, 4);
    hdrl_data_beacon = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);

    gui_vfs_read(fp, list_type, 4);
    if (memcmp(list_type, "hdrl", 4) != 0) { goto fail; }

    /* avih */
    gui_vfs_read(fp, id, 4);
    if (memcmp(id, "avih", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &chunk_size, 4);
    gui_vfs_read(fp, &main_hdr, sizeof(main_hdr));
    gui_vfs_seek(fp, (int)(chunk_size - sizeof(main_hdr)), GUI_VFS_SEEK_CUR);

    this->frame_time = (main_hdr.usec_per_frame > 0)
                       ? (main_hdr.usec_per_frame / 1000u)
                       : (1000u / MSV1_DEFAULT_FPS);

    /* LIST strl (video) */
    gui_vfs_read(fp, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &stream_list_size, 4);
    (void)stream_list_size;
    gui_vfs_read(fp, list_type, 4);
    if (memcmp(list_type, "strl", 4) != 0) { goto fail; }

    gui_vfs_read(fp, id, 4);
    if (memcmp(id, "strh", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &chunk_size, 4);
    gui_vfs_read(fp, &video_strh, sizeof(video_strh));

    memcpy(id, &video_strh.stream_format, 4);
    if (memcmp(id, "strf", 4) != 0) { goto fail; }
    chunk_size = video_strh.length_format;

    gui_vfs_read(fp, &bmp_hdr, sizeof(bmp_hdr));

    if (!is_msv1_fourcc(bmp_hdr.compression))
    {
        gui_log("msv1 fs: unsupported codec 0x%x\n", bmp_hdr.compression);
        gui_vfs_close(fp);
        return -1;
    }
    this->bits_per_pixel = (uint8_t)bmp_hdr.bit_count;

    if (this->bits_per_pixel == 8)
    {
        pal_count = bmp_hdr.colors_used ? bmp_hdr.colors_used : 256u;
        if (pal_count > MSV1_MAX_PALETTE) { pal_count = MSV1_MAX_PALETTE; }
        for (i = 0; i < pal_count; i++)
        {
            msv1_rgbquad_t q;
            gui_vfs_read(fp, &q, sizeof(q));
            this->palette565[i] = rgbquad_to_rgb565(&q);
        }
    }

    this->header.w = (int16_t)bmp_hdr.width;
    this->header.h = (int16_t)bmp_hdr.height;
    obj->w         = (int16_t)bmp_hdr.width;
    obj->h         = (int16_t)bmp_hdr.height;

    /* find movi */
    pos = hdrl_data_beacon + (int)hdrl_size;
    gui_vfs_seek(fp, pos, GUI_VFS_SEEK_SET);

    while (pos < file_size)
    {
        uint32_t blk_size = 0;
        gui_vfs_read(fp, sig, 4);
        gui_vfs_read(fp, &blk_size, 4);
        if (memcmp(sig, "JUNK", 4) == 0)
        {
            gui_vfs_seek(fp, (int)blk_size, GUI_VFS_SEEK_CUR);
        }
        else if (memcmp(sig, "LIST", 4) == 0)
        {
            gui_vfs_read(fp, list_type, 4);
            if (memcmp(list_type, "movi", 4) == 0)
            {
                movi_size = blk_size;
                pos = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);
                movi_data_beacon = pos - 4;
                break;
            }
            gui_vfs_seek(fp, (int)(blk_size - 4), GUI_VFS_SEEK_CUR);
        }
        else
        {
            gui_vfs_seek(fp, (int)blk_size, GUI_VFS_SEEK_CUR);
        }
        pos = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);
    }
    if (pos >= file_size) { gui_log("msv1 fs: no movi\n"); goto fail; }

    /* idx1 */
    gui_vfs_seek(fp, movi_data_beacon + (int)movi_size, GUI_VFS_SEEK_SET);
    gui_vfs_read(fp, sig, 4);
    if (memcmp(sig, "idx1", 4) != 0) { gui_log("msv1 fs: no idx1\n"); goto fail; }
    gui_vfs_read(fp, &idx1_size, 4);

    idx_buf = (msv1_idx1_item_t *)gui_malloc(idx1_size);
    if (!idx_buf) { gui_log("msv1 fs: idx1 malloc failed\n"); goto fail; }
    gui_vfs_read(fp, idx_buf, idx1_size);
    gui_vfs_close(fp);

    ret = build_chunk_array_from_idx1(this, obj,
                                      (uint32_t)movi_data_beacon,
                                      idx_buf, idx1_size);
    gui_free(idx_buf);
    return ret;

fail:
    gui_log("msv1 fs: parse error\n");
    gui_vfs_close(fp);
    return -1;
}

/*============================================================================*
 *              Private: Read One Compressed Frame Chunk
 *
 *  Reads the raw MSV1 compressed data for the logical video frame `frame_cur`
 *  and calls msv1_decode_frame_rgb565() into render_buf's pixel area.
 *  Returns 0 on success, -1 on error.
 *============================================================================*/
static int decode_frame(gui_msv1_t *this)
{
    msv1_chunk_t *chunks  = (msv1_chunk_t *)this->chunks;
    uint16_t     *dst_rgb = (uint16_t *)(this->render_buf + sizeof(gui_rgb_data_head_t));
    int           ret     = -1;

    /* Locate the AVI chunk index for frame_cur.
     * Scan forward from the last known position for efficiency. */
    int32_t frame_cnt = -1;
    for (uint32_t i = 0; i < this->chunk_num; i++)
    {
        if (chunks[i].type != MSV1_CHUNK_VIDEO) { continue; }
        frame_cnt++;
        if (frame_cnt < this->frame_cur)        { continue; }

        /* Found the video chunk for frame_cur. */
        if (this->frame_cur < 2)
        {
            gui_log("msv1_decode: frame=%d chunk[%u] offset=%u len=%u type=%u\n",
                    this->frame_cur, i, chunks[i].offset, chunks[i].len, chunks[i].type);
        }
        if (chunks[i].len == 0)
        {
            /* Zero-byte chunk = repeat previous frame; skip decode. */
            gui_log("msv1_decode: frame=%d is zero-length (repeat)\n", this->frame_cur);
            return 0;
        }
        this->frame_chunk_cur = i;

        if (this->storage_type == IMG_SRC_MEMADDR)
        {
            const uint8_t *src = (const uint8_t *)this->data + chunks[i].offset;
            if (this->frame_cur < 2)
            {
                gui_log("msv1_decode: src[0..3]=0x%02x%02x%02x%02x\n",
                        src[0], src[1], src[2], src[3]);
            }
            ret = msv1_decode_frame_rgb565(this->decoder,
                                           src, chunks[i].len,
                                           dst_rgb,
                                           this->palette565);
            if (this->frame_cur < 2)
            {
                gui_log("msv1_decode: rgb565_ret=%d dst[0]=0x%04x\n", ret, dst_rgb[0]);
            }
        }
        else if (this->storage_type == IMG_SRC_FILESYS)
        {
            uint8_t *buf = (uint8_t *)gui_malloc(chunks[i].len);
            if (!buf) { gui_log("msv1: frame malloc failed\n"); return -1; }

            gui_vfs_file_t *fp = gui_vfs_open((const char *)this->data, GUI_VFS_READ);
            if (fp)
            {
                gui_vfs_seek(fp, (int)chunks[i].offset, GUI_VFS_SEEK_SET);
                gui_vfs_read(fp, buf, chunks[i].len);
                gui_vfs_close(fp);
                ret = msv1_decode_frame_rgb565(this->decoder,
                                               buf, chunks[i].len,
                                               dst_rgb,
                                               this->palette565);
            }
            gui_free(buf);
        }
        else /* IMG_SRC_FTL */
        {
            uint8_t *buf = (uint8_t *)gui_malloc(chunks[i].len);
            if (!buf) { gui_log("msv1: frame malloc failed\n"); return -1; }

            uint32_t ftl_base = (uint32_t)(uintptr_t)this->data;
            gui_ftl_read(ftl_base + chunks[i].offset, buf, chunks[i].len);
            ret = msv1_decode_frame_rgb565(this->decoder,
                                           buf, chunks[i].len,
                                           dst_rgb,
                                           this->palette565);
            gui_free(buf);
        }
        return ret;
    }

    gui_log("msv1: frame %d not found\n", this->frame_cur);
    return -1;
}

/*============================================================================*
 *              Private: Widget Lifecycle Callbacks
 *============================================================================*/

static void gui_msv1_play_cb(void *p)
{
    gui_msv1_t *this = (gui_msv1_t *)p;

    if (this->state != GUI_VIDEO_STATE_PLAYING) { return; }

    this->frame_cur += (int32_t)this->frame_step;
    if ((uint32_t)this->frame_cur >= this->num_frame)
    {
        if (this->repeat_cnt > 0)
        {
            this->repeat_cnt--;
            this->frame_cur %= (int32_t)this->num_frame;
        }
        else if (this->repeat_cnt == 0)
        {
            this->frame_cur = (int32_t)(this->num_frame - 1u);
            this->state = GUI_VIDEO_STATE_STOP;
        }
        else /* infinite */
        {
            this->frame_cur %= (int32_t)this->num_frame;
        }
    }

    if (this->frame_cur != this->frame_last)
    {
        gui_fb_change();
    }
}

static void gui_msv1_prepare(gui_obj_t *obj)
{
    gui_msv1_t *this = (gui_msv1_t *)obj;
    gui_img_set_src(this->img, (const uint8_t *) & (this->header), IMG_SRC_MEMADDR);
}

static void gui_msv1_draw(gui_obj_t *obj)
{
    gui_msv1_t *this = (gui_msv1_t *)obj;

    this->frame_cur = (this->frame_cur < 0) ? 0 : this->frame_cur;

    if (this->frame_cur == this->frame_last && this->render_buf)
    {
        gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
        gui_img_refresh_draw_data(this->img);
        return;
    }

    if (this->frame_cur < 3)
    {
        gui_log("msv1_draw: cur=%d last=%d img=%p buf=%p\n",
                this->frame_cur, this->frame_last, (void *)this->img,
                (void *)this->render_buf);
    }

    if (decode_frame(this) != 0)
    {
        gui_log("msv1: decode frame %d failed\n", this->frame_cur);
        return;
    }

    if (this->frame_cur < 2)
    {
        uint16_t *px = (uint16_t *)(this->render_buf + sizeof(gui_rgb_data_head_t));
        gui_log("msv1_draw: frame%d px[0]=0x%04x px[1]=0x%04x\n",
                this->frame_cur, px[0], px[1]);
    }

    gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
    gui_img_refresh_draw_data(this->img);
    this->frame_last = this->frame_cur;
}

static void gui_msv1_draw_end(gui_obj_t *obj)
{
    (void)obj;
}

static void gui_msv1_destroy(gui_obj_t *obj)
{
    gui_msv1_t *this = (gui_msv1_t *)obj;

    if (this->decoder)
    {
        msv1_decoder_destroy(this->decoder);
        this->decoder = NULL;
    }
    if (this->render_buf)
    {
        gui_free(this->render_buf);
        this->render_buf = NULL;
    }
    if (this->chunks)
    {
        gui_free(this->chunks);
        this->chunks = NULL;
    }
}

static void gui_msv1_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (!obj) { return; }

    switch (cb_type)
    {
    case OBJ_PREPARE:  gui_msv1_prepare(obj);  break;
    case OBJ_DRAW:     gui_msv1_draw(obj);     break;
    case OBJ_END:      gui_msv1_draw_end(obj); break;
    case OBJ_DESTROY:  gui_msv1_destroy(obj);  break;
    default: break;
    }
}

/*============================================================================*
 *              Private: Constructor
 *============================================================================*/
static gui_msv1_t *gui_msv1_ctor(gui_obj_t   *parent,
                                 const char  *name,
                                 void        *addr,
                                 int16_t      x,
                                 int16_t      y,
                                 int16_t      w,
                                 int16_t      h,
                                 uint8_t      storage_type)
{
    gui_msv1_t *this = (gui_msv1_t *)gui_malloc(sizeof(gui_msv1_t));
    if (!this) { gui_log("msv1: alloc failed\n"); return NULL; }
    memset(this, 0, sizeof(gui_msv1_t));

    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);
    root->type              = IMAGE_FROM_MEM;
    root->obj_cb            = gui_msv1_cb;
    root->has_prepare_cb    = true;
    root->has_draw_cb       = true;
    root->has_end_cb        = true;
    root->has_destroy_cb    = true;

    this->data         = addr;
    this->storage_type = storage_type;
    this->frame_time   = 1000u / MSV1_DEFAULT_FPS;
    this->frame_step   = 1;
    this->frame_cur    = -1;
    this->frame_last   = -1;
    this->repeat_cnt   = -1; /* infinite by default */
    this->state        = GUI_VIDEO_STATE_PLAYING;

    /* Parse AVI container to get w/h/fps/chunks */
    int parse_ret = -1;
    if (storage_type == IMG_SRC_MEMADDR)
    {
        parse_ret = msv1_src_init_mem(this);
    }
    else if (storage_type == IMG_SRC_FILESYS)
    {
        parse_ret = msv1_src_init_fs(this);
    }
    else if (storage_type == IMG_SRC_FTL)
    {
        parse_ret = msv1_src_init_ftl(this);
    }

    if (parse_ret != 0)
    {
        gui_log("msv1: AVI parse failed\n");
        gui_free(this);
        return NULL;
    }

    /* Allocate persistent render buffer: header + RGB565 pixels */
    size_t pixel_bytes = (size_t)root->w * (size_t)root->h * 2u;
    this->render_buf   = (uint8_t *)gui_malloc(sizeof(gui_rgb_data_head_t) + pixel_bytes);
    if (!this->render_buf)
    {
        gui_log("msv1: render buf alloc failed\n");
        gui_free(this->chunks);
        gui_free(this);
        return NULL;
    }
    memset(this->render_buf, 0, sizeof(gui_rgb_data_head_t) + pixel_bytes);

    /* Fill the image header */
    memset(&this->header, 0, sizeof(gui_rgb_data_head_t));
    this->header.type = RGB565; /* 0x00 */
    this->header.w    = root->w;
    this->header.h    = root->h;
    memcpy(this->render_buf, &this->header, sizeof(gui_rgb_data_head_t));

    /* Create MSV1 decoder */
    this->decoder = msv1_decoder_create((uint16_t)root->w,
                                        (uint16_t)root->h,
                                        this->bits_per_pixel, this->render_buf + sizeof(gui_rgb_data_head_t));
    if (!this->decoder)
    {
        gui_log("msv1: decoder create failed\n");
        gui_free(this->render_buf);
        gui_free(this->chunks);
        gui_free(this);
        return NULL;
    }

    /* Register in parent widget tree */
    gui_list_init(&(root->child_list));
    if (root->parent)
    {
        gui_list_insert_before(&(root->parent->child_list), &(root->brother_list));
    }

    /* Child image widget for rendering */
    this->img = gui_img_create_from_mem(this, "MSV1_IMG",
                                        (void *)&this->header,
                                        0, 0,
                                        (int16_t)root->w,
                                        (int16_t)root->h);
    gui_img_set_mode(this->img, IMG_BYPASS_MODE);
    gui_img_set_src(this->img, (const uint8_t *)&this->header, IMG_SRC_MEMADDR);
    gui_img_refresh_size(this->img);

    /* Internal playback timer */
    gui_obj_create_timer(root, this->frame_time, true, gui_msv1_play_cb);

    return this;
}

/*============================================================================*
 *                        Public API
 *============================================================================*/

gui_msv1_t *gui_msv1_create_from_mem(void *parent, const char *name, void *addr,
                                     int16_t x, int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_LIVE_IMAGE";
    }



    gui_msv1_t *img = gui_msv1_ctor((gui_obj_t *)parent, name, addr,
                                    x, y, w, h, IMG_SRC_MEMADDR);
    GET_BASE(img)->create_done = true;

    return img;
}

gui_msv1_t *gui_msv1_create_from_fs(void *parent, const char *name, void *path,
                                    int16_t x, int16_t y, int16_t w, int16_t h)
{
    return gui_msv1_ctor((gui_obj_t *)parent, name, path,
                         x, y, w, h, IMG_SRC_FILESYS);
}

gui_msv1_t *gui_msv1_create_from_ftl(void *parent, const char *name, void *addr,
                                     int16_t x, int16_t y, int16_t w, int16_t h)
{
    return gui_msv1_ctor((gui_obj_t *)parent, name, addr,
                         x, y, w, h, IMG_SRC_FTL);
}

void gui_msv1_set_state(gui_msv1_t *this, GUI_VIDEO_STATE state)
{
    if (!this) { return; }
    if (this->state == GUI_VIDEO_STATE_STOP && state == GUI_VIDEO_STATE_PLAYING)
    {
        gui_msv1_reset(this);
    }
    this->state = state;
    if (state == GUI_VIDEO_STATE_INIT)
    {
        gui_msv1_reset(this);
    }
}

GUI_VIDEO_STATE gui_msv1_get_state(gui_msv1_t *this)
{
    if (!this) { return GUI_VIDEO_STATE_ERR; }
    return (GUI_VIDEO_STATE)this->state;
}

void gui_msv1_set_repeat_count(gui_msv1_t *this, int32_t cnt)
{
    if (!this) { return; }
    this->repeat_cnt = cnt;
}

void gui_msv1_set_frame_rate(gui_msv1_t *this, float fps)
{
    if (!this || fps <= 0.0f) { return; }
    this->frame_time = (uint32_t)roundf(1000.0f / fps);
    gui_obj_create_timer((gui_obj_t *)this, this->frame_time, true, gui_msv1_play_cb);
}

void gui_msv1_set_frame_step(gui_msv1_t *this, uint32_t step)
{
    if (!this) { return; }
    this->frame_step = step;
}

void gui_msv1_set_scale(gui_msv1_t *this, float scale_x, float scale_y)
{
    if (!this) { return; }
    gui_img_scale(this->img, scale_x, scale_y);
}
