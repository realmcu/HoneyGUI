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
#include "gui_lite_video.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_api.h"
#include "gui_vfs.h"

/*============================================================================*
 *                    Types: AVI container (unified, ltv_ prefix)
 *============================================================================*/

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
} ltv_avi_main_hdr_t;

typedef struct
{
    char     type[4];           /* "vids" or "auds" */
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
    uint32_t stream_format;     /* "strf" as uint32 */
    uint32_t length_format;     /* strf byte count  */
} ltv_avi_stream_hdr_t;

typedef struct
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pels_per_meter;
    uint32_t y_pels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} ltv_bmp_info_hdr_t;

typedef struct
{
    uint32_t chunk_ID;          /* "00dc" or "01wb" */
    uint32_t flags;
    uint32_t offset;
    uint32_t size;
} ltv_idx1_item_t;

typedef struct
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t reserved;
} ltv_rgbquad_t;

#pragma pack()

/* AVI chunk descriptor built at init time */
typedef struct
{
    uint32_t offset;            /* byte offset from start of AVI data */
    uint32_t len;               /* byte count of compressed data */
    uint32_t type;              /* ltv_CHUNK_* */
} ltv_chunk_t;

#define LTV_CHUNK_VIDEO   0u
#define LTV_CHUNK_AUDIO   1u
#define LTV_CHUNK_UNKNOWN 2u

/*============================================================================*
 *                         Macros / FourCC constants
 *============================================================================*/

#define LTV_DEFAULT_FPS      24u
#define LTV_MAX_PALETTE      256u

#define LTV_FOURCC_00DC      0x63643030u   /* "00dc" - video stream     */
#define LTV_FOURCC_01WB      0x62773130u   /* "01wb" - audio stream     */

/* MSV1 codec FourCCs */
#define LTV_FOURCC_CRAM      0x4D415243u   /* "CRAM" */
#define LTV_FOURCC_MSVC      0x4356534Du   /* "MSVC" */
#define LTV_FOURCC_wmsv      0x76736D77u   /* "wmsv" */

/* Cinepak codec FourCCs */
#define LTV_FOURCC_CVID      0x44495643u   /* "CVID" (uppercase) */
#define LTV_FOURCC_cvid      0x64697663u   /* "cvid" (lowercase, most AVI files) */

/*============================================================================*
 *                    Private: Cursor helpers
 *============================================================================*/

/* -- in-memory cursor ---------------------------------------------------- */

typedef struct
{
    const uint8_t *base;
    uint32_t       pos;
    uint32_t       size;
} ltv_mem_cur_t;

static inline void ltv_mem_read(ltv_mem_cur_t *c, void *dst, uint32_t n)
{
    if (c->pos + n <= c->size)
    {
        memcpy(dst, c->base + c->pos, n);
    }
    c->pos += n;
}

static inline void ltv_mem_skip(ltv_mem_cur_t *c, uint32_t n)
{
    c->pos += n;
}

/* -- FTL cursor ----------------------------------------------------------- */

typedef struct
{
    uint32_t base;
    uint32_t pos;
} ltv_ftl_cur_t;

static inline void ltv_ftl_read(ltv_ftl_cur_t *c, void *dst, uint32_t n)
{
    gui_ftl_read(c->base + c->pos, (uint8_t *)dst, n);
    c->pos += n;
}

static inline void ltv_ftl_skip(ltv_ftl_cur_t *c, uint32_t n)
{
    c->pos += n;
}

/*============================================================================*
 *                    Private: Helpers
 *============================================================================*/

static inline uint16_t ltv_rgbquad_to_rgb565(const ltv_rgbquad_t *q)
{
    uint16_t r = (q->r >> 3) & 0x1Fu;
    uint16_t g = (q->g >> 2) & 0x3Fu;
    uint16_t b = (q->b >> 3) & 0x1Fu;
    return (uint16_t)((r << 11) | (g << 5) | b);
}

static inline bool ltv_is_msv1_fourcc(uint32_t fcc)
{
    return (fcc == LTV_FOURCC_CRAM || fcc == LTV_FOURCC_MSVC || fcc == LTV_FOURCC_wmsv);
}

static inline bool ltv_is_cvid_fourcc(uint32_t fcc)
{
    return (fcc == LTV_FOURCC_CVID || fcc == LTV_FOURCC_cvid);
}

static void ltv_reset(gui_lite_video_t *this)
{
    this->frame_cur       = -1;
    this->frame_last      = -1;
    this->frame_chunk_cur = 0;
}

/*============================================================================*
 *             Private: Build AVI chunk array from idx1
 *============================================================================*/

static int ltv_build_chunk_array(gui_lite_video_t *this,
                                 gui_obj_t        *obj,
                                 uint32_t          movi_beacon,
                                 const void       *idx1_data,
                                 uint32_t          idx1_size)
{
    uint32_t n = idx1_size / (uint32_t)sizeof(ltv_idx1_item_t);
    ltv_chunk_t *arr = (ltv_chunk_t *)gui_malloc(n * (uint32_t)sizeof(ltv_chunk_t));
    if (!arr)
    {
        gui_log("lv: chunk array alloc failed\n");
        return -1;
    }

    this->chunks    = arr;
    this->chunk_num = n;

    const ltv_idx1_item_t *items = (const ltv_idx1_item_t *)idx1_data;
    uint32_t frame_cnt = 0;

    for (uint32_t i = 0; i < n; i++)
    {
        arr[i].offset = movi_beacon + items[i].offset + 8u;
        arr[i].len    = items[i].size;
        arr[i].type   = (items[i].chunk_ID == LTV_FOURCC_00DC) ? LTV_CHUNK_VIDEO
                        : (items[i].chunk_ID == LTV_FOURCC_01WB) ? LTV_CHUNK_AUDIO
                        : LTV_CHUNK_UNKNOWN;

        if (arr[i].type == LTV_CHUNK_VIDEO)
        {
            frame_cnt++;
        }
    }

    obj->w          = this->header.w;
    obj->h          = this->header.h;
    this->num_frame = frame_cnt;

    gui_log("lv: %u frames, %dx%d, %u ms/frame, codec=%d\n",
            this->num_frame, obj->w, obj->h, this->frame_time, (int)this->codec);
    return 0;
}

/*============================================================================*
 *             Private: Parse codec-specific strf fields
 *
 *  Sets this->codec; for MSV1 also fills this->u.msv1.bpp and palette565.
 *  Returns the number of strf bytes consumed (so caller can skip the rest).
 *  Returns 0 on unrecognised codec (caller should treat as error).
 *============================================================================*/

/* Shared logic after reading bmp -- used by all three storage paths via
 * separate helper to avoid code duplication in the parsing callbacks. */
static uint32_t ltv_strf_detect_msv1(gui_lite_video_t *this,
                                     const ltv_bmp_info_hdr_t *bmp)
{
    this->codec      = (uint8_t)LITE_VIDEO_CODEC_MSV1;
    this->u.msv1.bpp = (uint8_t)bmp->bit_count;
    return (uint32_t)sizeof(ltv_bmp_info_hdr_t);
}

static uint32_t ltv_strf_detect_cvid(gui_lite_video_t *this)
{
    this->codec = (uint8_t)LITE_VIDEO_CODEC_CINEPAK;
    return (uint32_t)sizeof(ltv_bmp_info_hdr_t);
}

/*============================================================================*
 *             Private: AVI Init -- MEMADDR path
 *============================================================================*/

static int ltv_src_init_mem(gui_lite_video_t *this)
{
    gui_obj_t      *obj      = (gui_obj_t *)this;
    const uint8_t  *img_data = (const uint8_t *)this->data;

    uint32_t riff_size;
    memcpy(&riff_size, img_data + 4, 4);
    uint32_t file_size = riff_size + 8u;

    ltv_mem_cur_t c;
    c.base = img_data;
    c.pos  = 12;    /* skip "RIFF"(4) + riff_size(4) + "AVI "(4) */
    c.size = file_size;

    uint8_t  sig[5]       = {0};
    uint8_t  list_type[5] = {0};
    uint8_t  id[5]        = {0};
    uint32_t hdrl_size    = 0;
    uint32_t chunk_size   = 0;

    ltv_mem_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("lv mem: no LIST\n"); return -1; }
    ltv_mem_read(&c, &hdrl_size, 4);
    uint32_t hdrl_beacon = c.pos;
    ltv_mem_read(&c, list_type, 4);
    if (memcmp(list_type, "hdrl", 4) != 0) { gui_log("lv mem: no hdrl\n"); return -1; }

    /* avih */
    ltv_mem_read(&c, id, 4);
    if (memcmp(id, "avih", 4) != 0) { gui_log("lv mem: no avih\n"); return -1; }
    ltv_mem_read(&c, &chunk_size, 4);
    ltv_avi_main_hdr_t main_hdr;
    ltv_mem_read(&c, &main_hdr, sizeof(main_hdr));
    ltv_mem_skip(&c, chunk_size - (uint32_t)sizeof(main_hdr));

    this->frame_time = (main_hdr.usec_per_frame > 0)
                       ? (main_hdr.usec_per_frame / 1000u)
                       : (1000u / LTV_DEFAULT_FPS);

    /* LIST strl (video) */
    ltv_mem_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("lv mem: no strl\n"); return -1; }
    uint32_t strl_size = 0;
    ltv_mem_read(&c, &strl_size, 4);
    (void)strl_size;
    ltv_mem_read(&c, list_type, 4);
    if (memcmp(list_type, "strl", 4) != 0) { gui_log("lv mem: no strl type\n"); return -1; }

    /* strh */
    ltv_mem_read(&c, id, 4);
    if (memcmp(id, "strh", 4) != 0) { gui_log("lv mem: no strh\n"); return -1; }
    ltv_mem_read(&c, &chunk_size, 4);
    ltv_avi_stream_hdr_t vstrh;
    ltv_mem_read(&c, &vstrh, sizeof(vstrh));

    /* strf */
    memcpy(id, &vstrh.stream_format, 4);
    if (memcmp(id, "strf", 4) != 0) { gui_log("lv mem: no strf\n"); return -1; }
    chunk_size = vstrh.length_format;

    ltv_bmp_info_hdr_t bmp;
    ltv_mem_read(&c, &bmp, sizeof(bmp));

    uint32_t consumed;
    if (ltv_is_msv1_fourcc(bmp.compression))
    {
        consumed = ltv_strf_detect_msv1(this, &bmp);
        if (this->u.msv1.bpp == 8)
        {
            uint32_t pal_n = bmp.colors_used ? bmp.colors_used : LTV_MAX_PALETTE;
            if (pal_n > LTV_MAX_PALETTE) { pal_n = LTV_MAX_PALETTE; }
            for (uint32_t pi = 0; pi < pal_n; pi++)
            {
                ltv_rgbquad_t q;
                ltv_mem_read(&c, &q, sizeof(q));
                this->u.msv1.palette565[pi] = ltv_rgbquad_to_rgb565(&q);
            }
            consumed += pal_n * (uint32_t)sizeof(ltv_rgbquad_t);
        }
    }
    else if (ltv_is_cvid_fourcc(bmp.compression))
    {
        consumed = ltv_strf_detect_cvid(this);
    }
    else
    {
        gui_log("lv mem: unsupported codec 0x%x\n", bmp.compression);
        return -1;
    }

    if (chunk_size > consumed) { ltv_mem_skip(&c, chunk_size - consumed); }

    this->header.w = (int16_t)bmp.width;
    this->header.h = (int16_t)bmp.height;
    obj->w         = (int16_t)bmp.width;
    obj->h         = (int16_t)bmp.height;

    /* Find movi LIST */
    uint32_t movi_size   = 0;
    uint32_t movi_beacon = 0;
    c.pos = hdrl_beacon + hdrl_size;

    while (c.pos < file_size)
    {
        uint32_t blk_size = 0;
        ltv_mem_read(&c, sig, 4);
        ltv_mem_read(&c, &blk_size, 4);
        if (memcmp(sig, "JUNK", 4) == 0)
        {
            ltv_mem_skip(&c, blk_size);
        }
        else if (memcmp(sig, "LIST", 4) == 0)
        {
            ltv_mem_read(&c, list_type, 4);
            if (memcmp(list_type, "movi", 4) == 0)
            {
                movi_size   = blk_size;
                movi_beacon = c.pos - 4u;
                break;
            }
            ltv_mem_skip(&c, blk_size - 4u);
        }
        else
        {
            ltv_mem_skip(&c, blk_size);
        }
    }
    if (c.pos >= file_size) { gui_log("lv mem: no movi\n"); return -1; }

    /* idx1 */
    c.pos = movi_beacon + movi_size;
    ltv_mem_read(&c, sig, 4);
    if (memcmp(sig, "idx1", 4) != 0) { gui_log("lv mem: no idx1\n"); return -1; }
    uint32_t idx1_size = 0;
    ltv_mem_read(&c, &idx1_size, 4);

    return ltv_build_chunk_array(this, obj, movi_beacon, img_data + c.pos, idx1_size);
}

/*============================================================================*
 *             Private: AVI Init -- FTL path
 *============================================================================*/

static int ltv_src_init_ftl(gui_lite_video_t *this)
{
    gui_obj_t    *obj = (gui_obj_t *)this;
    ltv_ftl_cur_t  c;
    c.base = (uint32_t)(uintptr_t)this->data;
    c.pos  = 0;

    uint8_t  sig[5]       = {0};
    uint8_t  list_type[5] = {0};
    uint8_t  id[5]        = {0};
    uint32_t chunk_size   = 0;

    uint32_t riff_size = 0;
    c.pos = 4;
    ltv_ftl_read(&c, &riff_size, 4);
    uint32_t file_size = riff_size + 8u;
    c.pos = 12;

    ltv_ftl_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("lv ftl: no LIST\n"); return -1; }
    uint32_t hdrl_size = 0;
    ltv_ftl_read(&c, &hdrl_size, 4);
    uint32_t hdrl_beacon = c.pos;
    ltv_ftl_read(&c, list_type, 4);
    if (memcmp(list_type, "hdrl", 4) != 0) { gui_log("lv ftl: no hdrl\n"); return -1; }

    ltv_ftl_read(&c, id, 4);
    if (memcmp(id, "avih", 4) != 0) { gui_log("lv ftl: no avih\n"); return -1; }
    ltv_ftl_read(&c, &chunk_size, 4);
    ltv_avi_main_hdr_t main_hdr;
    ltv_ftl_read(&c, &main_hdr, sizeof(main_hdr));
    ltv_ftl_skip(&c, chunk_size - (uint32_t)sizeof(main_hdr));

    this->frame_time = (main_hdr.usec_per_frame > 0)
                       ? (main_hdr.usec_per_frame / 1000u)
                       : (1000u / LTV_DEFAULT_FPS);

    ltv_ftl_read(&c, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { gui_log("lv ftl: no strl\n"); return -1; }
    uint32_t strl_size = 0;
    ltv_ftl_read(&c, &strl_size, 4);
    (void)strl_size;
    ltv_ftl_read(&c, list_type, 4);
    if (memcmp(list_type, "strl", 4) != 0) { gui_log("lv ftl: no strl type\n"); return -1; }

    ltv_ftl_read(&c, id, 4);
    if (memcmp(id, "strh", 4) != 0) { gui_log("lv ftl: no strh\n"); return -1; }
    ltv_ftl_read(&c, &chunk_size, 4);
    ltv_avi_stream_hdr_t vstrh;
    ltv_ftl_read(&c, &vstrh, sizeof(vstrh));

    memcpy(id, &vstrh.stream_format, 4);
    if (memcmp(id, "strf", 4) != 0) { gui_log("lv ftl: no strf\n"); return -1; }
    chunk_size = vstrh.length_format;

    ltv_bmp_info_hdr_t bmp;
    ltv_ftl_read(&c, &bmp, sizeof(bmp));

    uint32_t consumed;
    if (ltv_is_msv1_fourcc(bmp.compression))
    {
        consumed = ltv_strf_detect_msv1(this, &bmp);
        if (this->u.msv1.bpp == 8)
        {
            uint32_t pal_n = bmp.colors_used ? bmp.colors_used : LTV_MAX_PALETTE;
            if (pal_n > LTV_MAX_PALETTE) { pal_n = LTV_MAX_PALETTE; }
            for (uint32_t pi = 0; pi < pal_n; pi++)
            {
                ltv_rgbquad_t q;
                ltv_ftl_read(&c, &q, sizeof(q));
                this->u.msv1.palette565[pi] = ltv_rgbquad_to_rgb565(&q);
            }
            consumed += pal_n * (uint32_t)sizeof(ltv_rgbquad_t);
        }
    }
    else if (ltv_is_cvid_fourcc(bmp.compression))
    {
        consumed = ltv_strf_detect_cvid(this);
    }
    else
    {
        gui_log("lv ftl: unsupported codec 0x%x\n", bmp.compression);
        return -1;
    }

    if (chunk_size > consumed) { ltv_ftl_skip(&c, chunk_size - consumed); }

    this->header.w = (int16_t)bmp.width;
    this->header.h = (int16_t)bmp.height;
    obj->w         = (int16_t)bmp.width;
    obj->h         = (int16_t)bmp.height;

    /* Find movi */
    uint32_t movi_size   = 0;
    uint32_t movi_beacon = 0;
    c.pos = hdrl_beacon + hdrl_size;

    while (c.pos < file_size)
    {
        uint32_t blk_size = 0;
        ltv_ftl_read(&c, sig, 4);
        ltv_ftl_read(&c, &blk_size, 4);
        if (memcmp(sig, "JUNK", 4) == 0)
        {
            ltv_ftl_skip(&c, blk_size);
        }
        else if (memcmp(sig, "LIST", 4) == 0)
        {
            ltv_ftl_read(&c, list_type, 4);
            if (memcmp(list_type, "movi", 4) == 0)
            {
                movi_size   = blk_size;
                movi_beacon = c.pos - 4u;
                break;
            }
            ltv_ftl_skip(&c, blk_size - 4u);
        }
        else
        {
            ltv_ftl_skip(&c, blk_size);
        }
    }
    if (c.pos >= file_size) { gui_log("lv ftl: no movi\n"); return -1; }

    /* idx1 */
    c.pos = movi_beacon + movi_size;
    ltv_ftl_read(&c, sig, 4);
    if (memcmp(sig, "idx1", 4) != 0) { gui_log("lv ftl: no idx1\n"); return -1; }
    uint32_t idx1_size = 0;
    ltv_ftl_read(&c, &idx1_size, 4);

    ltv_idx1_item_t *idx_buf = (ltv_idx1_item_t *)gui_malloc(idx1_size);
    if (!idx_buf) { gui_log("lv ftl: idx1 malloc failed\n"); return -1; }
    gui_ftl_read(c.base + c.pos, (uint8_t *)idx_buf, idx1_size);

    int ret = ltv_build_chunk_array(this, obj, movi_beacon, idx_buf, idx1_size);
    gui_free(idx_buf);
    return ret;
}

/*============================================================================*
 *             Private: AVI Init -- Filesystem path
 *============================================================================*/

static int ltv_src_init_fs(gui_lite_video_t *this)
{
    /* Declare all locals at top for Keil AC5 (C89) compatibility. */
    gui_obj_t            *obj              = (gui_obj_t *)this;
    const char           *fn               = (const char *)this->data;
    gui_vfs_file_t       *fp;
    int                   file_size;
    uint8_t               sig[5]           = {0};
    uint8_t               list_type[5]     = {0};
    uint8_t               id[5]            = {0};
    uint32_t              chunk_size       = 0;
    uint32_t              hdrl_size        = 0;
    int                   hdrl_beacon      = 0;
    ltv_avi_main_hdr_t     main_hdr;
    uint32_t              strl_size        = 0;
    ltv_avi_stream_hdr_t   vstrh;
    ltv_bmp_info_hdr_t     bmp;
    uint32_t              consumed;
    uint32_t              pal_n;
    uint32_t              pi;
    uint32_t              movi_size        = 0;
    int                   movi_beacon      = 0;
    int                   pos;
    uint32_t              idx1_size        = 0;
    ltv_idx1_item_t       *idx_buf          = NULL;
    int                   ret;

    fp = gui_vfs_open(fn, GUI_VFS_READ);
    if (!fp) { gui_log("lv fs: open failed: %s\n", fn); return -1; }

    file_size = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_END);
    if (file_size <= 0) { gui_vfs_close(fp); return -1; }

    gui_vfs_seek(fp, 12, GUI_VFS_SEEK_SET);

    gui_vfs_read(fp, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &hdrl_size, 4);
    hdrl_beacon = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);
    gui_vfs_read(fp, list_type, 4);
    if (memcmp(list_type, "hdrl", 4) != 0) { goto fail; }

    gui_vfs_read(fp, id, 4);
    if (memcmp(id, "avih", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &chunk_size, 4);
    gui_vfs_read(fp, &main_hdr, sizeof(main_hdr));
    gui_vfs_seek(fp, (int)(chunk_size - sizeof(main_hdr)), GUI_VFS_SEEK_CUR);

    this->frame_time = (main_hdr.usec_per_frame > 0)
                       ? (main_hdr.usec_per_frame / 1000u)
                       : (1000u / LTV_DEFAULT_FPS);

    gui_vfs_read(fp, sig, 4);
    if (memcmp(sig, "LIST", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &strl_size, 4);
    (void)strl_size;
    gui_vfs_read(fp, list_type, 4);
    if (memcmp(list_type, "strl", 4) != 0) { goto fail; }

    gui_vfs_read(fp, id, 4);
    if (memcmp(id, "strh", 4) != 0) { goto fail; }
    gui_vfs_read(fp, &chunk_size, 4);
    gui_vfs_read(fp, &vstrh, sizeof(vstrh));

    memcpy(id, &vstrh.stream_format, 4);
    if (memcmp(id, "strf", 4) != 0) { goto fail; }
    chunk_size = vstrh.length_format;

    gui_vfs_read(fp, &bmp, sizeof(bmp));

    if (ltv_is_msv1_fourcc(bmp.compression))
    {
        consumed = ltv_strf_detect_msv1(this, &bmp);
        if (this->u.msv1.bpp == 8)
        {
            pal_n = bmp.colors_used ? bmp.colors_used : LTV_MAX_PALETTE;
            if (pal_n > LTV_MAX_PALETTE) { pal_n = LTV_MAX_PALETTE; }
            for (pi = 0; pi < pal_n; pi++)
            {
                ltv_rgbquad_t q;
                gui_vfs_read(fp, &q, sizeof(q));
                this->u.msv1.palette565[pi] = ltv_rgbquad_to_rgb565(&q);
            }
            consumed += pal_n * (uint32_t)sizeof(ltv_rgbquad_t);
        }
    }
    else if (ltv_is_cvid_fourcc(bmp.compression))
    {
        consumed = ltv_strf_detect_cvid(this);
    }
    else
    {
        gui_log("lv fs: unsupported codec 0x%x\n", bmp.compression);
        gui_vfs_close(fp);
        return -1;
    }

    if (chunk_size > consumed)
    {
        gui_vfs_seek(fp, (int)(chunk_size - consumed), GUI_VFS_SEEK_CUR);
    }

    this->header.w = (int16_t)bmp.width;
    this->header.h = (int16_t)bmp.height;
    obj->w         = (int16_t)bmp.width;
    obj->h         = (int16_t)bmp.height;

    /* Find movi */
    pos = hdrl_beacon + (int)hdrl_size;
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
                movi_size   = blk_size;
                pos         = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);
                movi_beacon = pos - 4;
                break;
            }
            gui_vfs_seek(fp, (int)(blk_size - 4u), GUI_VFS_SEEK_CUR);
        }
        else
        {
            gui_vfs_seek(fp, (int)blk_size, GUI_VFS_SEEK_CUR);
        }
        pos = gui_vfs_seek(fp, 0, GUI_VFS_SEEK_CUR);
    }
    if (pos >= file_size) { gui_log("lv fs: no movi\n"); goto fail; }

    gui_vfs_seek(fp, movi_beacon + (int)movi_size, GUI_VFS_SEEK_SET);
    gui_vfs_read(fp, sig, 4);
    if (memcmp(sig, "idx1", 4) != 0) { gui_log("lv fs: no idx1\n"); goto fail; }
    gui_vfs_read(fp, &idx1_size, 4);

    idx_buf = (ltv_idx1_item_t *)gui_malloc(idx1_size);
    if (!idx_buf) { gui_log("lv fs: idx1 malloc failed\n"); goto fail; }
    gui_vfs_read(fp, idx_buf, idx1_size);
    gui_vfs_close(fp);

    ret = ltv_build_chunk_array(this, obj, (uint32_t)movi_beacon,
                                idx_buf, idx1_size);
    gui_free(idx_buf);
    return ret;

fail:
    gui_log("lv fs: parse error\n");
    gui_vfs_close(fp);
    return -1;
}

/*============================================================================*
 *             Private: Decode One Video Frame
 *============================================================================*/

static int ltv_decode_frame(gui_lite_video_t *this)
{
    ltv_chunk_t   *chunks    = (ltv_chunk_t *)this->chunks;
    uint16_t     *dst_rgb   = (uint16_t *)(this->render_buf + sizeof(gui_rgb_data_head_t));
    int32_t       frame_cnt = -1;

    for (uint32_t i = 0; i < this->chunk_num; i++)
    {
        if (chunks[i].type != LTV_CHUNK_VIDEO) { continue; }
        frame_cnt++;
        if (frame_cnt < this->frame_cur) { continue; }

        if (chunks[i].len == 0) { return 0; }  /* repeat previous frame */
        this->frame_chunk_cur = i;

        if (this->storage_type == IMG_SRC_MEMADDR)
        {
            const uint8_t *src = (const uint8_t *)this->data + chunks[i].offset;
            if (this->codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
            {
                uint16_t *pal = (this->u.msv1.bpp == 8) ? this->u.msv1.palette565 : NULL;
                return msv1_decode_frame_rgb565(this->u.msv1.dec,
                                                src, chunks[i].len,
                                                dst_rgb, pal);
            }
            else /* CINEPAK */
            {
                const void *px = cinepak_decode_frame(this->u.cinepak.dec,
                                                      src, chunks[i].len);
                return px ? 0 : -1;
            }
        }
        else
        {
            uint8_t *buf = (uint8_t *)gui_malloc(chunks[i].len);
            if (!buf) { gui_log("lv: frame malloc failed\n"); return -1; }

            if (this->storage_type == IMG_SRC_FILESYS)
            {
                gui_vfs_file_t *fp = gui_vfs_open((const char *)this->data,
                                                  GUI_VFS_READ);
                if (!fp)
                {
                    gui_free(buf);
                    gui_log("lv: file open failed\n");
                    return -1;
                }
                gui_vfs_seek(fp, (int)chunks[i].offset, GUI_VFS_SEEK_SET);
                gui_vfs_read(fp, buf, chunks[i].len);
                gui_vfs_close(fp);
            }
            else /* IMG_SRC_FTL */
            {
                uint32_t ftl_base = (uint32_t)(uintptr_t)this->data;
                gui_ftl_read(ftl_base + chunks[i].offset, buf, chunks[i].len);
            }

            int ret;
            if (this->codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
            {
                uint16_t *pal = (this->u.msv1.bpp == 8) ? this->u.msv1.palette565 : NULL;
                ret = msv1_decode_frame_rgb565(this->u.msv1.dec,
                                               buf, chunks[i].len,
                                               dst_rgb, pal);
            }
            else /* CINEPAK */
            {
                const void *px = cinepak_decode_frame(this->u.cinepak.dec,
                                                      buf, chunks[i].len);
                ret = px ? 0 : -1;
            }

            gui_free(buf);
            return ret;
        }
    }

    gui_log("lv: frame %d not found\n", this->frame_cur);
    return -1;
}

/*============================================================================*
 *             Private: Widget Lifecycle Callbacks
 *============================================================================*/

static void ltv_play_cb(void *p)
{
    gui_lite_video_t *this = (gui_lite_video_t *)p;

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
            this->state     = GUI_VIDEO_STATE_STOP;
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

static void ltv_prepare(gui_obj_t *obj)
{
    gui_lite_video_t *this = (gui_lite_video_t *)obj;
    gui_img_set_src(this->img, (const uint8_t *)&this->header, IMG_SRC_MEMADDR);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_DOUBLE_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
}

static void ltv_draw(gui_obj_t *obj)
{
    gui_lite_video_t *this = (gui_lite_video_t *)obj;

    this->frame_cur = (this->frame_cur < 0) ? 0 : this->frame_cur;

    if (this->frame_cur == this->frame_last && this->render_buf)
    {
        gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
        gui_img_refresh_draw_data(this->img);
        return;
    }

    /*
     * Frame-skip protection: if the play timer fired more than once between
     * two render calls, decode (but do not display) every skipped intermediate
     * frame so the codec's internal state (MSV1 skip-block reference,
     * Cinepak codebook) stays correct.
     */
    if (this->frame_last >= 0 && this->frame_cur > this->frame_last + 1)
    {
        int32_t saved_cur = this->frame_cur;
        int32_t f;
        for (f = this->frame_last + 1; f < saved_cur; f++)
        {
            this->frame_cur = f;
            (void)ltv_decode_frame(this);
        }
        this->frame_cur = saved_cur;
    }

    if (ltv_decode_frame(this) != 0)
    {
        gui_log("lv: decode frame %d failed\n", this->frame_cur);
        return;
    }

    gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
    gui_img_refresh_draw_data(this->img);
    this->frame_last = this->frame_cur;
}

static void ltv_draw_end(gui_obj_t *obj)
{
    (void)obj;
}

static void ltv_destroy(gui_obj_t *obj)
{
    gui_lite_video_t *this = (gui_lite_video_t *)obj;

    if (this->codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
    {
        if (this->u.msv1.dec)
        {
            msv1_decoder_destroy(this->u.msv1.dec);
            this->u.msv1.dec = NULL;
        }
    }
    else if (this->codec == (uint8_t)LITE_VIDEO_CODEC_CINEPAK)
    {
        if (this->u.cinepak.dec)
        {
            cinepak_decoder_destroy(this->u.cinepak.dec);
            this->u.cinepak.dec = NULL;
        }
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

static void ltv_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (!obj) { return; }

    switch (cb_type)
    {
    case OBJ_PREPARE:  ltv_prepare(obj);  break;
    case OBJ_DRAW:     ltv_draw(obj);     break;
    case OBJ_END:      ltv_draw_end(obj); break;
    case OBJ_DESTROY:  ltv_destroy(obj);  break;
    default: break;
    }
}

/*============================================================================*
 *             Private: Constructor
 *============================================================================*/

static gui_lite_video_t *ltv_ctor(gui_obj_t   *parent,
                                  const char  *name,
                                  void        *addr,
                                  int16_t      x,
                                  int16_t      y,
                                  int16_t      w,
                                  int16_t      h,
                                  uint8_t      storage_type)
{
    gui_lite_video_t *this = (gui_lite_video_t *)gui_malloc(sizeof(gui_lite_video_t));
    if (!this) { gui_log("lv: alloc failed\n"); return NULL; }
    memset(this, 0, sizeof(gui_lite_video_t));

    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);
    root->type           = IMAGE_FROM_MEM;
    root->obj_cb         = ltv_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb    = true;
    root->has_end_cb     = true;
    root->has_destroy_cb = true;

    this->data         = addr;
    this->storage_type = storage_type;
    this->codec        = (uint8_t)LITE_VIDEO_CODEC_UNKNOWN;
    this->frame_time   = 1000u / LTV_DEFAULT_FPS;
    this->frame_step   = 1;
    this->frame_cur    = -1;
    this->frame_last   = -1;
    this->repeat_cnt   = -1;   /* infinite by default */
    this->state        = GUI_VIDEO_STATE_PLAYING;

    /* Parse AVI container -- sets codec, w/h, frame_time, chunks */
    int parse_ret = -1;
    if (storage_type == IMG_SRC_MEMADDR) { parse_ret = ltv_src_init_mem(this); }
    else if (storage_type == IMG_SRC_FILESYS) { parse_ret = ltv_src_init_fs(this);  }
    else if (storage_type == IMG_SRC_FTL)     { parse_ret = ltv_src_init_ftl(this); }

    if (parse_ret != 0)
    {
        gui_log("lv: AVI parse failed\n");
        gui_free(this);
        return NULL;
    }

    /* Allocate render buffer: image header + RGB565 pixel area */
    size_t pixel_bytes = (size_t)root->w * (size_t)root->h * 2u;
    this->render_buf = (uint8_t *)gui_malloc(sizeof(gui_rgb_data_head_t) + pixel_bytes);
    if (!this->render_buf)
    {
        gui_log("lv: render buf alloc failed\n");
        gui_free(this->chunks);
        gui_free(this);
        return NULL;
    }
    memset(this->render_buf, 0, sizeof(gui_rgb_data_head_t) + pixel_bytes);

    /* Write image header prefix */
    memset(&this->header, 0, sizeof(gui_rgb_data_head_t));
    this->header.type = RGB565;
    this->header.w    = root->w;
    this->header.h    = root->h;
    memcpy(this->render_buf, &this->header, sizeof(gui_rgb_data_head_t));

    /* Create the codec-specific decoder; it reads/writes the pixel area */
    uint8_t *pixel_buf = this->render_buf + sizeof(gui_rgb_data_head_t);
    if (this->codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
    {
        this->u.msv1.dec = msv1_decoder_create((uint16_t)root->w, (uint16_t)root->h,
                                               this->u.msv1.bpp, pixel_buf);
        if (!this->u.msv1.dec)
        {
            gui_log("lv: MSV1 decoder create failed\n");
            gui_free(this->render_buf);
            gui_free(this->chunks);
            gui_free(this);
            return NULL;
        }
    }
    else /* CINEPAK */
    {
        this->u.cinepak.dec = cinepak_decoder_create((uint16_t)root->w, (uint16_t)root->h,
                                                     pixel_buf);
        if (!this->u.cinepak.dec)
        {
            gui_log("lv: Cinepak decoder create failed\n");
            gui_free(this->render_buf);
            gui_free(this->chunks);
            gui_free(this);
            return NULL;
        }
    }

    /* Register in parent widget tree */
    gui_list_init(&root->child_list);
    if (root->parent)
    {
        gui_list_insert_before(&root->parent->child_list, &root->brother_list);
    }

    /* Child image widget */
    this->img = gui_img_create_from_mem(this, "LTV_IMG",
                                        (void *)&this->header,
                                        0, 0,
                                        (int16_t)root->w,
                                        (int16_t)root->h);
    gui_img_set_mode(this->img, IMG_BYPASS_MODE);
    gui_img_set_src(this->img, (const uint8_t *)&this->header, IMG_SRC_MEMADDR);
    gui_img_refresh_size(this->img);

    /* Playback timer */
    gui_obj_create_timer(root, this->frame_time, true, ltv_play_cb);

    return this;
}

/*============================================================================*
 *                        Public API
 *============================================================================*/

gui_lite_video_t *gui_lite_video_create_from_mem(void *parent, const char *name,
                                                 void *addr,
                                                 int16_t x, int16_t y,
                                                 int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (!name) { name = "DEFAULT_LV"; }

    gui_lite_video_t *widget = ltv_ctor((gui_obj_t *)parent, name, addr,
                                        x, y, w, h, IMG_SRC_MEMADDR);
    if (widget) { GET_BASE(widget)->create_done = true; }
    return widget;
}

gui_lite_video_t *gui_lite_video_create_from_fs(void *parent, const char *name,
                                                void *path,
                                                int16_t x, int16_t y,
                                                int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (!name) { name = "DEFAULT_LV"; }

    gui_lite_video_t *widget = ltv_ctor((gui_obj_t *)parent, name, path,
                                        x, y, w, h, IMG_SRC_FILESYS);
    if (widget) { GET_BASE(widget)->create_done = true; }
    return widget;
}

gui_lite_video_t *gui_lite_video_create_from_ftl(void *parent, const char *name,
                                                 void *addr,
                                                 int16_t x, int16_t y,
                                                 int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (!name) { name = "DEFAULT_LV"; }

    gui_lite_video_t *widget = ltv_ctor((gui_obj_t *)parent, name, addr,
                                        x, y, w, h, IMG_SRC_FTL);
    if (widget) { GET_BASE(widget)->create_done = true; }
    return widget;
}

void gui_lite_video_set_state(gui_lite_video_t *this, GUI_VIDEO_STATE state)
{
    if (!this) { return; }
    if (this->state == GUI_VIDEO_STATE_STOP && state == GUI_VIDEO_STATE_PLAYING)
    {
        ltv_reset(this);
    }
    this->state = state;
    if (state == GUI_VIDEO_STATE_INIT)
    {
        ltv_reset(this);
    }
}

GUI_VIDEO_STATE gui_lite_video_get_state(gui_lite_video_t *this)
{
    if (!this) { return GUI_VIDEO_STATE_ERR; }
    return (GUI_VIDEO_STATE)this->state;
}

void gui_lite_video_set_repeat_count(gui_lite_video_t *this, int32_t cnt)
{
    if (!this) { return; }
    this->repeat_cnt = cnt;
}

void gui_lite_video_set_frame_rate(gui_lite_video_t *this, float fps)
{
    if (!this || fps <= 0.0f) { return; }
    this->frame_time = (uint32_t)roundf(1000.0f / fps);
    gui_obj_create_timer((gui_obj_t *)this, this->frame_time, true, ltv_play_cb);
}

void gui_lite_video_set_frame_step(gui_lite_video_t *this, uint32_t step)
{
    if (!this) { return; }
    this->frame_step = step;
}

void gui_lite_video_set_scale(gui_lite_video_t *this, float scale_x, float scale_y)
{
    if (!this) { return; }
    gui_img_scale(this->img, scale_x, scale_y);
}

void gui_lite_video_set_src(gui_lite_video_t *this, void *src, uint8_t storage_type)
{
    /* Declare all locals at top for Keil AC5 (C89) compatibility. */
    gui_obj_t   *obj;
    void        *old_data;
    uint8_t      old_storage_type;
    void        *old_chunks;
    uint32_t     old_chunk_num;
    uint32_t     old_num_frame;
    int16_t      old_w;
    int16_t      old_h;
    uint32_t     old_frame_time;
    uint8_t      old_codec;
    void        *old_dec_ptr;         /* saved decoder pointer (any codec)  */
    uint8_t      old_bpp;
    uint16_t     old_palette[256];
    int          parse_ret;
    int16_t      new_w;
    int16_t      new_h;
    int          size_changed;
    uint8_t     *new_render_buf;
    size_t       new_total;
    uint8_t     *pixel_buf;
    int          codec_changed;
    int          dec_ok;

    if (!this || !src) { return; }
    if (storage_type != IMG_SRC_MEMADDR &&
        storage_type != IMG_SRC_FILESYS &&
        storage_type != IMG_SRC_FTL) { return; }

    obj = (gui_obj_t *)this;

    /* ------------------------------------------------------------------ *
     * Save all fields that parse helpers may mutate; enables full rollback.
     * ------------------------------------------------------------------ */
    old_data         = this->data;
    old_storage_type = this->storage_type;
    old_chunks       = this->chunks;
    old_chunk_num    = this->chunk_num;
    old_num_frame    = this->num_frame;
    old_w            = this->header.w;
    old_h            = this->header.h;
    old_frame_time   = this->frame_time;
    old_codec        = this->codec;

    /* Save codec-specific decoder pointer and MSV1 state. */
    old_dec_ptr = NULL;
    old_bpp     = 0;
    if (old_codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
    {
        old_dec_ptr = (void *)this->u.msv1.dec;
        old_bpp     = this->u.msv1.bpp;
        memcpy(old_palette, this->u.msv1.palette565, sizeof(this->u.msv1.palette565));
    }
    else if (old_codec == (uint8_t)LITE_VIDEO_CODEC_CINEPAK)
    {
        old_dec_ptr = (void *)this->u.cinepak.dec;
    }

    /* Null out chunks so parse helpers always build a fresh array. */
    this->chunks    = NULL;
    this->chunk_num = 0;
    this->codec     = (uint8_t)LITE_VIDEO_CODEC_UNKNOWN;

    this->data         = src;
    this->storage_type = storage_type;

    /* ------------------------------------------------------------------ *
     * Parse new AVI container.  On success: codec, header.w/h, frame_time,
     * num_frame, chunks, chunk_num, and MSV1 fields are updated.
     * ------------------------------------------------------------------ */
    parse_ret = -1;
    if (storage_type == IMG_SRC_MEMADDR) { parse_ret = ltv_src_init_mem(this); }
    else if (storage_type == IMG_SRC_FILESYS) { parse_ret = ltv_src_init_fs(this);  }
    else if (storage_type == IMG_SRC_FTL)     { parse_ret = ltv_src_init_ftl(this); }

    if (parse_ret != 0)
    {
        gui_log("lv set_src: parse failed -- keeping old source\n");
        goto rollback;
    }

    new_w        = this->header.w;
    new_h        = this->header.h;
    size_changed = (new_w != old_w || new_h != old_h);
    new_total    = sizeof(gui_rgb_data_head_t) + (size_t)new_w * (size_t)new_h * 2u;
    codec_changed = (this->codec != old_codec);

    /* ------------------------------------------------------------------ *
     * Allocate new render buffer when frame dimensions changed.
     * ------------------------------------------------------------------ */
    if (size_changed)
    {
        new_render_buf = (uint8_t *)gui_malloc(new_total);
        if (!new_render_buf)
        {
            gui_log("lv set_src: render buf alloc failed\n");
            gui_free(this->chunks);
            goto rollback;
        }
    }
    else
    {
        new_render_buf = this->render_buf;   /* reuse; zeroed below */
    }

    memset(new_render_buf, 0, new_total);
    this->header.type = RGB565;
    this->header.w    = new_w;
    this->header.h    = new_h;
    memcpy(new_render_buf, &this->header, sizeof(gui_rgb_data_head_t));

    pixel_buf = new_render_buf + sizeof(gui_rgb_data_head_t);
    dec_ok    = 0;

    /* ------------------------------------------------------------------ *
     * Create or reset the decoder.
     *
     * Same codec: reset in-place (avoids reallocating codec internals).
     * Cross-codec: create a fresh decoder of the new type.
     * Old decoder is destroyed only at COMMIT (after success is assured).
     * ------------------------------------------------------------------ */
    if (!codec_changed)
    {
        /* Same codec: reset the existing decoder struct. */
        if (this->codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
        {
            dec_ok = (msv1_decoder_reset(this->u.msv1.dec,
                                         (uint16_t)new_w, (uint16_t)new_h,
                                         this->u.msv1.bpp, pixel_buf) == 0);
        }
        else /* CINEPAK */
        {
            dec_ok = (cinepak_decoder_reset(this->u.cinepak.dec,
                                            (uint16_t)new_w, (uint16_t)new_h,
                                            pixel_buf) == 0);
        }
    }
    else
    {
        /* Cross-codec: create a new decoder; old one destroyed at commit. */
        if (this->codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
        {
            this->u.msv1.dec = msv1_decoder_create((uint16_t)new_w, (uint16_t)new_h,
                                                   this->u.msv1.bpp, pixel_buf);
            dec_ok = (this->u.msv1.dec != NULL);
        }
        else /* CINEPAK */
        {
            this->u.cinepak.dec = cinepak_decoder_create((uint16_t)new_w, (uint16_t)new_h,
                                                         pixel_buf);
            dec_ok = (this->u.cinepak.dec != NULL);
        }
    }

    if (!dec_ok)
    {
        gui_log("lv set_src: decoder create/reset failed\n");
        if (codec_changed)
        {
            /* New decoder was not created; null out the pointer so
             * rollback can restore the old one cleanly. */
            if (this->codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
            {
                this->u.msv1.dec = NULL;
            }
            else
            {
                this->u.cinepak.dec = NULL;
            }
        }
        if (size_changed) { gui_free(new_render_buf); }
        gui_free(this->chunks);
        goto rollback;
    }

    /* ================================================================== *
     * COMMIT POINT -- all resources are ready; no more failure paths.
     * ================================================================== */

    /* Destroy old decoder if the codec type changed. */
    if (codec_changed)
    {
        if (old_codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
        {
            msv1_decoder_destroy((msv1_decoder_t *)old_dec_ptr);
        }
        else if (old_codec == (uint8_t)LITE_VIDEO_CODEC_CINEPAK)
        {
            cinepak_decoder_destroy((cinepak_decoder_t *)old_dec_ptr);
        }
    }

    if (size_changed)
    {
        gui_free(this->render_buf);
        this->render_buf = new_render_buf;
    }

    gui_free(old_chunks);

    obj->w = new_w;
    obj->h = new_h;
    if (this->img)
    {
        ((gui_obj_t *)this->img)->w = new_w;
        ((gui_obj_t *)this->img)->h = new_h;
        gui_img_set_src(this->img, this->render_buf, IMG_SRC_MEMADDR);
        gui_img_refresh_size(this->img);
    }

    gui_obj_create_timer(obj, this->frame_time, true, ltv_play_cb);

    ltv_reset(this);
    this->repeat_cnt = -1;
    this->state      = GUI_VIDEO_STATE_PLAYING;
    return;

rollback:
    this->data         = old_data;
    this->storage_type = old_storage_type;
    this->chunks       = old_chunks;
    this->chunk_num    = old_chunk_num;
    this->num_frame    = old_num_frame;
    this->header.w     = old_w;
    this->header.h     = old_h;
    this->frame_time   = old_frame_time;
    this->codec        = old_codec;

    /* Restore decoder pointer into the correct union arm. */
    if (old_codec == (uint8_t)LITE_VIDEO_CODEC_MSV1)
    {
        this->u.msv1.dec = (msv1_decoder_t *)old_dec_ptr;
        this->u.msv1.bpp = old_bpp;
        memcpy(this->u.msv1.palette565, old_palette, sizeof(this->u.msv1.palette565));
    }
    else if (old_codec == (uint8_t)LITE_VIDEO_CODEC_CINEPAK)
    {
        this->u.cinepak.dec = (cinepak_decoder_t *)old_dec_ptr;
    }

    obj->w = old_w;
    obj->h = old_h;
}
