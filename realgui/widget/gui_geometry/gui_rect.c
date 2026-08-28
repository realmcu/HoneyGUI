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
#include <stddef.h>
#include "guidef.h"
#include "gui_fb.h"
#include "acc_api.h"
#include "gui_rect.h"
#include "lite_geometry.h"
#include "gui_shape_cache.h"

/*============================================================================*
 *                           Configuration
 *============================================================================*/
// 1 = Enable Dither by default (Better quality on 565/666 screens)
// 0 = Disable Dither (Maximum speed, banding may appear on gradients)
#define GUI_RECT_ENABLE_DITHER  1

/**
 * 1 = store solid rects as A8 coverage masks, 0 = always ARGB8888.
 *
 * A8 costs a quarter of the memory, drops colour from the cache key so rects
 * differing only in colour share one payload, and reaches acc_sw's dedicated A8
 * blit instead of the generic rasteriser.
 *
 * Set to 0 when bringing up a hardware accelerator whose blit may not handle A8.
 * An unsupported source format usually shows up as a garbled or missing shape
 * rather than a clean fallback, so flipping this is the quickest way to rule it
 * out.  The same switch isolates what A8 alone contributes to frame time -- the
 * shape cache stays active either way.
 *
 * Overridable from the build, so a bring-up run needs no source edit:
 *   scons GUI_RECT_ENABLE_A8=0
 */
#ifndef GUI_RECT_ENABLE_A8
#define GUI_RECT_ENABLE_A8  1
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/

/** Which rasterised part a cached payload holds. */
typedef enum
{
    RECT_PART_ROUNDED = 1,      /**< Whole rounded rect in one ARGB8888 buffer. */
    RECT_PART_SOLID,            /**< Solid sub-rectangle, used when transformed. */
    RECT_PART_CORNER,           /**< One rounded corner, keyed by corner index. */
} gui_rect_part_t;

/**
 * Identity of a cached rect payload.
 *
 * Everything that changes a pixel goes in here and nothing else does; position
 * stays out, since it only ever reaches the draw_img matrix.  Gradient parts
 * pass the whole struct, solid ones stop short of the gradient -- see
 * rect_desc_len().
 */
typedef struct
{
    uint32_t part;              /**< gui_rect_part_t. */
    int32_t size_a;             /**< Width, or radius for RECT_PART_CORNER. */
    int32_t size_b;             /**< Height, or corner index for RECT_PART_CORNER. */
    int32_t radius;             /**< Corner radius. */
    uint32_t color;             /**< ARGB baked into the pixels; 0 when A8. */
    uint32_t is_a8;             /**< Non-zero when the payload is a coverage mask. */
    uint32_t flags;             /**< Dither on/off, and gradient direction plus 1. */
    Gradient gradient;          /**< Only present when flags say a gradient is used. */
} rect_desc_t;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/** Descriptor bytes to compare: solid parts stop before the gradient. */
static uint16_t rect_desc_len(const rect_desc_t *desc)
{
    if ((desc->flags & 0xFFFF0000u) == 0u)
    {
        return (uint16_t)offsetof(rect_desc_t, gradient);
    }
    return (uint16_t)sizeof(rect_desc_t);
}

/** FNV-1a accumulator used for change detection. */
static uint32_t rect_checksum(uint32_t seed, const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t *)data;

    for (size_t i = 0; i < len; i++)
    {
        seed ^= p[i];
        seed *= 16777619u;
    }

    return seed;
}

/**
 * Whether this rect can be stored as an A8 coverage mask.
 *
 * A mask carries no RGB, so one payload serves every colour of the same shape --
 * which is the point, since "same size, different colour" is otherwise a full
 * cache miss.  Only a gradient rules it out, since that genuinely needs per-pixel
 * RGB.
 *
 * A translucent colour is fine: its alpha is folded into the mask values, not
 * carried in fg_color_set.  That matters because the two blit paths disagree
 * about fg_color_set's alpha -- acc_sw_raster multiplies it in, a8_2_rgb565
 * discards it -- so it is pinned at 255 and the mask carries everything.
 */
static bool rect_use_a8(gui_rounded_rect_t *this)
{
#if GUI_RECT_ENABLE_A8
    return !(this->use_gradient && this->gradient != NULL && this->gradient->stop_count >= 2);
#else
    GUI_UNUSED(this);
    return false;
#endif
}

/** Set the image header for an A8 coverage mask */
static void set_a8_header(gui_rgb_data_head_t *head, uint16_t w, uint16_t h)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = A8;
    head->w = w;
    head->h = h;
    head->version = 0;
    head->rsvd2 = 0;
}

/**
 * Point a draw_img at a payload, in whichever format it holds.
 *
 * An A8 payload only draws under IMG_2D_SW_FIX_A8_FG -- the blit routines ignore
 * an A8 image in any other blend mode -- and takes its colour from fg_color_set.
 */
static void set_img_payload(gui_rounded_rect_t *this, draw_img_t *img, uint8_t *payload,
                            bool is_a8)
{
    img->data = payload;
    img->opacity_value = this->opacity_value;
    img->high_quality = 1;

    if (is_a8)
    {
        img->blend_mode = IMG_2D_SW_FIX_A8_FG;
        /* Alpha pinned at 255: the mask already carries this->color's alpha, and
         * the two blit paths disagree about fg_color_set's alpha anyway. */
        img->fg_color_set = 0xFF000000u | (this->color.color.argb_full & 0x00FFFFFFu);
    }
    else
    {
        img->blend_mode = IMG_SRC_OVER_MODE;
    }
}

/**
 * Fill in a descriptor for one part of this rect.
 *
 * Zeroes first: gui_shape_cache_acquire() compares descriptors byte for byte, so
 * a padding hole left uninitialised would make two identical rects miss.
 */
static void rect_desc_init(rect_desc_t *desc, gui_rounded_rect_t *this,
                           gui_rect_part_t part, int size_a, int size_b)
{
    memset(desc, 0x00, sizeof(*desc));

    desc->part = (uint32_t)part;
    desc->size_a = size_a;
    desc->size_b = size_b;
    desc->radius = this->radius;
    desc->flags = this->enable_dither ? 1u : 0u;

    /* RGB is deliberately left out of an A8 key -- that is what lets rects
     * differing only in colour share one mask.  Alpha stays in, because it is
     * folded into the mask values themselves. */
    if (rect_use_a8(this))
    {
        desc->is_a8 = 1u;
        desc->color = this->color.color.rgba.a;
    }
    else
    {
        desc->color = this->color.color.argb_full;
    }

    /* Only the whole-rect part reads the gradient; sub-rectangles and corners
     * are always solid.  The direction lives in the high half so that
     * rect_desc_len() can tell from flags alone whether to compare it. */
    if (part == RECT_PART_ROUNDED && this->use_gradient && this->gradient != NULL &&
        this->gradient->stop_count >= 2)
    {
        desc->flags |= ((uint32_t)this->gradient_dir + 1u) << 16;
        memcpy(&desc->gradient, this->gradient, sizeof(Gradient));
    }
}

/** Safely free a draw_img_t, dropping its reference on the shared pixel data */
static void free_draw_img(draw_img_t **img)
{
    if (img == NULL || *img == NULL) { return; }

    /* Free HW-acceleration user data (e.g. boundary-line cache from hw_acc_prepare_cb). */
    if ((*img)->acc_user != NULL)
    {
        gui_free((*img)->acc_user);
        (*img)->acc_user = NULL;
    }

    if ((*img)->data != NULL)
    {
        /* An IMG_RECT payload is a bare header with no pixels, so it is owned
         * outright rather than shared -- see set_rect_img(). */
        if ((*img)->blend_mode == IMG_RECT)
        {
            gui_free((void *)(*img)->data);
        }
        else
        {
            gui_shape_cache_release((*img)->data);
        }
        (*img)->data = NULL;
    }
    gui_free(*img);
    *img = NULL;
}

/** Set image data header for rectangle */
static void set_rect_header(gui_rgb_data_head_t *head, uint16_t w, uint16_t h, gui_color_t color)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = w;
    head->h = h;
    head->version = 0;
    head->rsvd2 = 0;
    gui_rect_file_head_t *rect_head = (gui_rect_file_head_t *)head;
    rect_head->color = color;
}

/** Create a solid color image buffer */
static uint8_t *create_solid_color_buffer(gui_rounded_rect_t *this, uint16_t w, uint16_t h,
                                          gui_color_t color)
{
    bool is_a8 = rect_use_a8(this);
    rect_desc_t desc;
    bool is_new = false;
    rect_desc_init(&desc, this, RECT_PART_SOLID, w, h);

    uint32_t pixel_bytes = is_a8 ? 1u : 4u;
    uint32_t buffer_size = w * h * pixel_bytes + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = gui_shape_cache_acquire(&desc, rect_desc_len(&desc), buffer_size, &is_new);
    if (buffer == NULL) { return NULL; }
    if (!is_new) { return buffer; }

    if (is_a8)
    {
        /* Fully covered everywhere, so the mask is just the colour's alpha and
         * every sub-rectangle of this size and alpha shares it. */
        set_a8_header((gui_rgb_data_head_t *)buffer, w, h);
        memset(buffer + sizeof(gui_rgb_data_head_t), color.color.rgba.a, (size_t)w * h);
        return buffer;
    }

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)buffer;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = w;
    head->h = h;
    head->version = 0;
    head->rsvd2 = 0;

    uint32_t *pixels = (uint32_t *)(buffer + sizeof(gui_rgb_data_head_t));
    uint32_t argb = color.color.argb_full;
    for (int i = 0; i < w * h; i++)
    {
        pixels[i] = argb;
    }
    return buffer;
}

/** Create a rectangle image object (Legacy/Fallback) */
static void set_rect_img(gui_rounded_rect_t *this, draw_img_t **input_img, int16_t x,
                         int16_t y, uint16_t w, uint16_t h)
{
    // Free old buffer first to prevent memory leak
    free_draw_img(input_img);

    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(draw_img_t));

    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f);

    if (has_transform)
    {
        uint8_t *payload = create_solid_color_buffer(this, w, h, this->color);
        if (payload == NULL)
        {
            gui_free(img);
            *input_img = NULL;
            return;
        }
        set_img_payload(this, img, payload, rect_use_a8(this));
    }
    else
    {
        /* Deliberately not cached: this payload is 12 bytes of header with no
         * pixels, and its width and height make it near-unique anyway, so the
         * per-node bookkeeping would cost several times the payload. */
        gui_rect_file_head_t *rect_data = gui_malloc(sizeof(gui_rect_file_head_t));
        GUI_ASSERT(rect_data != NULL);
        set_rect_header((gui_rgb_data_head_t *)rect_data, w, h, this->color);

        img->blend_mode = IMG_RECT;
        img->data = rect_data;
    }

    img->opacity_value = this->opacity_value;

    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    matrix_translate(x, y, &img->matrix);
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);
    *input_img = img;
}

/**
 * Prepare arc image data for a specific corner with supersampling AA
 *
 * @param is_a8 Write one coverage byte per pixel instead of an ARGB8888 pixel.
 *              The geometry and supersampling are identical either way.
 */
static void prepare_arc_img(gui_rounded_rect_t *this, uint8_t *circle_data, int corner_type,
                            bool is_a8)
{
    if (this->radius == 0) { return; }
    uint32_t *data = (uint32_t *)(circle_data + sizeof(gui_rgb_data_head_t));
    uint8_t *mask = circle_data + sizeof(gui_rgb_data_head_t);
    uint16_t img_size = this->radius + 1;
    memset(circle_data + sizeof(gui_rgb_data_head_t), 0,
           (size_t)img_size * img_size * (is_a8 ? 1u : 4u));

    float center = (float)this->radius;
    float radius_sq = this->radius * this->radius;
    float inner_sq = (this->radius - 0.5f) * (this->radius - 0.5f);
    float outer_sq = (this->radius + 0.5f) * (this->radius + 0.5f);
    uint32_t color_full = this->color.color.argb_full;

    int is_right = (corner_type == 1 || corner_type == 2);
    int is_bottom = (corner_type == 2 || corner_type == 3);

    for (int i = 0; i < img_size; i++)
    {
        float py = is_bottom ? (img_size - 1 - i) : i;
        py += 0.5f;
        float dy = py - center;
        float dy_sq = dy * dy;
        float wy0 = dy - 0.5f;

        for (int j = 0; j < img_size; j++)
        {
            float px = is_right ? (img_size - 1 - j) : j;
            px += 0.5f;
            float dx = px - center;
            float dist_sq = dx * dx + dy_sq;

            if (dist_sq <= inner_sq)
            {
                if (is_a8) { mask[i * img_size + j] = this->color.color.rgba.a; }
                else { data[i * img_size + j] = color_full; }
            }
            else if (dist_sq < outer_sq)
            {
                int count = 0;
                float step = 0.25f;
                for (int sx = 0; sx < 4; sx++)
                {
                    float wx = dx - 0.5f + (sx + 0.5f) * step;
                    float wx_sq = wx * wx;
                    for (int sy = 0; sy < 4; sy++)
                    {
                        float wy = wy0 + (sy + 0.5f) * step;
                        if (wx_sq + wy * wy <= radius_sq)
                        {
                            count++;
                        }
                    }
                }

                /* Coverage becomes the alpha directly, without scaling by the
                 * colour's own alpha the way the other rasterisers do.  That is
                 * exact here rather than an oversight: this function is only
                 * reached via create_corner_img(), which only runs on the
                 * split-rendering path, which gui_rect_prepare() only takes when
                 * !need_single_buffer -- and that condition rules out a
                 * translucent colour.  So the colour's alpha is always 255 here
                 * and scaling by it would only cost precision.
                 *
                 * Should need_single_buffer ever stop excluding translucent
                 * colours, this has to scale by it the way
                 * fill_solid_rounded_rect() does, or corner edges will come out
                 * less transparent than the body. */
                uint8_t alpha = (count * 255) >> 4;
                if (alpha > 0)
                {
                    if (is_a8) { mask[i * img_size + j] = alpha; }
                    else
                    {
                        data[i * img_size + j] = (color_full & 0x00FFFFFF) | ((uint32_t)alpha << 24);
                    }
                }
            }
        }
    }
}

// ============================================================================
//  HIGH PERFORMANCE RENDERER (ZONE BASED + PRE-CALC)
// ============================================================================

/**
 * Allocate buffer for single-buffer rendering
 *
 * @param out_body   Receives the payload body, past the image header.  Points at
 *                   ARGB8888 pixels, or coverage bytes when the rect is A8.
 * @param out_is_new Set false when the payload was already rasterised by an
 *                   identical rect, in which case out_body must not be written.
 */
static draw_img_t *alloc_rect_img_buffer(gui_rounded_rect_t *this, gui_obj_t *obj,
                                         uint8_t **out_body, bool *out_is_new)
{
    int w = this->base.w;
    int h = this->base.h;

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    /* Every rect with the same size, radius and gradient draws from one payload;
     * only this draw_img_t and its matrix are per widget.  An opaque solid rect
     * stores a colourless mask, so colour does not split the cache either. */
    bool is_a8 = rect_use_a8(this);
    rect_desc_t desc;
    bool is_new = false;
    rect_desc_init(&desc, this, RECT_PART_ROUNDED, w, h);

    uint32_t pixel_bytes = is_a8 ? 1u : 4u;
    uint32_t buffer_size = w * h * pixel_bytes + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = gui_shape_cache_acquire(&desc, rect_desc_len(&desc), buffer_size, &is_new);
    if (buffer == NULL)
    {
        gui_free(img);
        return NULL;
    }

    if (is_new)
    {
        memset(buffer, 0x00, buffer_size);

        if (is_a8)
        {
            set_a8_header((gui_rgb_data_head_t *)buffer, (uint16_t)w, (uint16_t)h);
        }
        else
        {
            gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)buffer;
            head->scan = 0;
            head->align = 0;
            head->resize = 0;
            head->compress = 0;
            head->rsvd = 0;
            head->type = ARGB8888;
            head->w = w;
            head->h = h;
        }
    }

    set_img_payload(this, img, buffer, is_a8);

    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    *out_body = buffer + sizeof(gui_rgb_data_head_t);
    *out_is_new = is_new;
    return img;
}

/**
 * Pre-calculate Corner Alpha Mask
 * Returns an r*r array. 255 = solid, 0 = transparent.
 */
static uint8_t *generate_corner_mask(int r)
{
    if (r <= 0) { return NULL; }
    uint8_t *mask = gui_malloc(r * r);
    if (!mask) { return NULL; }

    float r_sq = (float)((r - 0.5f) * (r - 0.5f));
    float r_out_sq = (float)((r + 0.5f) * (r + 0.5f));

    for (int y = 0; y < r; y++)
    {
        for (int x = 0; x < r; x++)
        {
            // Logic: Calculate for Top-Left corner conceptually
            // Distance from the circle center which is at (r, r)
            float dist_sq = (r - x - 0.5f) * (r - x - 0.5f) + (r - y - 0.5f) * (r - y - 0.5f);

            if (dist_sq <= r_sq)
            {
                mask[y * r + x] = 255;
            }
            else if (dist_sq < r_out_sq)
            {
                float alpha_f = r + 0.5f - sqrtf(dist_sq);
                mask[y * r + x] = (uint8_t)(alpha_f * 255.0f);
            }
            else
            {
                mask[y * r + x] = 0;
            }
        }
    }
    return mask;
}

/**
 * Fast Integer Dither (4x4 Bayer)
 */
static inline uint32_t fast_dither(uint32_t color, int x, int y)
{
    static const int8_t dither_table[16] =
    {
        -8,  0, -6,  2,
        4, -4,  6, -2,
        -5,  3, -7,  1,
        7, -1,  5, -3
    };

    int d = dither_table[(y & 3) * 4 + (x & 3)];
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;
    int a = (color >> 24) & 0xFF;

    r += d;
    if (r < 0) { r = 0; }
    else if (r > 255) { r = 255; }
    g += d;
    if (g < 0) { g = 0; }
    else if (g > 255) { g = 255; }
    b += d;
    if (b < 0) { b = 0; }
    else if (b > 255) { b = 255; }

    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/**
 * Vertical Gradient Renderer (Zone Optimized)
 */
static void fill_vertical_gradient_opt(uint32_t *pixels, int w, int h, int r, uint32_t *lut,
                                       uint8_t *mask, bool use_dither)
{
    // Zone 1: Top (0 to r-1)
    for (int y = 0; y < r; y++)
    {
        uint32_t base = lut[y];
        uint32_t *line = pixels + y * w;
        uint8_t *mask_row = mask + y * r;

        // TL Corner
        for (int x = 0; x < r; x++)
        {
            uint32_t c = use_dither ? fast_dither(base, x, y) : base;
            uint8_t a = mask_row[x];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
        // Top Middle
        for (int x = r; x < w - r; x++)
        {
            line[x] = use_dither ? fast_dither(base, x, y) : base;
        }
        // TR Corner (Mirror X of mask)
        for (int x = w - r, mx = r - 1; x < w; x++, mx--)
        {
            uint32_t c = use_dither ? fast_dither(base, x, y) : base;
            uint8_t a = mask_row[mx];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
    }

    // Zone 2: Middle (r to h-r) - FASTEST
    for (int y = r; y < h - r; y++)
    {
        uint32_t base = lut[y];
        uint32_t *line = pixels + y * w;

        if (use_dither)
        {
            for (int x = 0; x < w; x++) { line[x] = fast_dither(base, x, y); }
        }
        else
        {
            // No dither: Solid color line fill
            for (int x = 0; x < w; x++) { line[x] = base; }
        }
    }

    // Zone 3: Bottom (h-r to h-1)
    for (int y = h - r, my = r - 1; y < h; y++, my--)
    {
        uint32_t base = lut[y];
        uint32_t *line = pixels + y * w;
        uint8_t *mask_row = mask + my * r; // Mirror Y of mask

        // BL Corner
        for (int x = 0; x < r; x++)
        {
            uint32_t c = use_dither ? fast_dither(base, x, y) : base;
            uint8_t a = mask_row[x];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
        // Bottom Middle
        for (int x = r; x < w - r; x++)
        {
            line[x] = use_dither ? fast_dither(base, x, y) : base;
        }
        // BR Corner
        for (int x = w - r, mx = r - 1; x < w; x++, mx--)
        {
            uint32_t c = use_dither ? fast_dither(base, x, y) : base;
            uint8_t a = mask_row[mx];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
    }
}

/**
 * Horizontal Gradient Renderer (Zone Optimized)
 */
static void fill_horizontal_gradient_opt(uint32_t *pixels, int w, int h, int r, uint32_t *lut,
                                         uint8_t *mask, bool use_dither)
{
    // Zone 1: Top (0 to r-1)
    for (int y = 0; y < r; y++)
    {
        uint32_t *line = pixels + y * w;
        uint8_t *mask_row = mask + y * r;

        // TL
        for (int x = 0; x < r; x++)
        {
            uint32_t c = lut[x];
            if (use_dither) { c = fast_dither(c, x, y); }
            uint8_t a = mask_row[x];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
        // Top Middle
        for (int x = r; x < w - r; x++)
        {
            uint32_t c = lut[x];
            line[x] = use_dither ? fast_dither(c, x, y) : c;
        }
        // TR
        for (int x = w - r, mx = r - 1; x < w; x++, mx--)
        {
            uint32_t c = lut[x];
            if (use_dither) { c = fast_dither(c, x, y); }
            uint8_t a = mask_row[mx];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
    }

    // Zone 2: Middle (r to h-r)
    for (int y = r; y < h - r; y++)
    {
        uint32_t *line = pixels + y * w;
        for (int x = 0; x < w; x++)
        {
            uint32_t c = lut[x];
            line[x] = use_dither ? fast_dither(c, x, y) : c;
        }
    }

    // Zone 3: Bottom (h-r to h-1)
    for (int y = h - r, my = r - 1; y < h; y++, my--)
    {
        uint32_t *line = pixels + y * w;
        uint8_t *mask_row = mask + my * r;

        // BL
        for (int x = 0; x < r; x++)
        {
            uint32_t c = lut[x];
            if (use_dither) { c = fast_dither(c, x, y); }
            uint8_t a = mask_row[x];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
        // Bottom Middle
        for (int x = r; x < w - r; x++)
        {
            uint32_t c = lut[x];
            line[x] = use_dither ? fast_dither(c, x, y) : c;
        }
        // BR
        for (int x = w - r, mx = r - 1; x < w; x++, mx--)
        {
            uint32_t c = lut[x];
            if (use_dither) { c = fast_dither(c, x, y); }
            uint8_t a = mask_row[mx];
            if (a == 255) { line[x] = c; }
            else if (a > 0)
            {
                uint8_t ca = (c >> 24) & 0xFF;
                line[x] = (c & 0x00FFFFFF) | ((uint32_t)((ca * a) >> 8) << 24);
            }
            else { line[x] = 0; }
        }
    }
}

static void fill_gradient_rounded_rect(gui_rounded_rect_t *this, uint32_t *pixels, int w, int h,
                                       int r)
{
    // Clamp radius
    if (r * 2 > w) { r = w / 2; }
    if (r * 2 > h) { r = h / 2; }

    int lut_size = (this->gradient_dir == RECT_GRADIENT_VERTICAL) ? h : w;
    uint32_t *lut = gui_malloc(lut_size * sizeof(uint32_t));
    if (!lut) { return; }

    // Generate LUT
    float step = (lut_size > 1) ? 1.0f / (float)(lut_size - 1) : 0.0f;
    for (int i = 0; i < lut_size; i++)
    {
        lut[i] = gradient_get_color(this->gradient, (float)i * step);
    }

    // Generate Corner Mask
    uint8_t *mask = NULL;
    if (r > 0)
    {
        mask = generate_corner_mask(r);
        if (!mask) { gui_free(lut); return; }
    }

    bool use_dither = GUI_RECT_ENABLE_DITHER;

    // Dispatch
    if (this->gradient_dir == RECT_GRADIENT_VERTICAL)
    {
        fill_vertical_gradient_opt(pixels, w, h, r, lut, mask, use_dither);
    }
    else
    {
        // Horizontal and fallback for others
        fill_horizontal_gradient_opt(pixels, w, h, r, lut, mask, use_dither);
    }

    if (mask) { gui_free(mask); }
    if (lut) { gui_free(lut); }
}

/**
 * Fill solid color with optimization
 * Reuse the mask logic to avoid sqrt loops
 */
static void fill_solid_rounded_rect(gui_rounded_rect_t *this, uint32_t *pixels, int w, int h, int r)
{
    if (r * 2 > w) { r = w / 2; }
    if (r * 2 > h) { r = h / 2; }

    uint32_t solid_color = this->color.color.argb_full;
    uint8_t *mask = NULL;
    if (r > 0)
    {
        mask = generate_corner_mask(r);
        if (!mask) { return; }
    }

    // Zone 1 & 3: Corners (Top/Bottom)
    // We can reuse a simplified logic or just run loops
    // For brevity and speed, let's just do Top/Bottom loops
    if (r > 0)
    {
        // Top
        for (int y = 0; y < r; y++)
        {
            uint32_t *line = pixels + y * w;
            uint8_t *m = mask + y * r;
            for (int x = 0; x < r; x++) // TL
            {
                if (m[x] == 255) { line[x] = solid_color; }
                else if (m[x] > 0)
                {
                    uint8_t ca = (solid_color >> 24) & 0xFF;
                    line[x] = (solid_color & 0x00FFFFFF) | ((uint32_t)((ca * m[x]) >> 8) << 24);
                }
            }
            for (int x = r; x < w - r; x++) { line[x] = solid_color; } // TM
            for (int x = w - r, mx = r - 1; x < w; x++, mx--) // TR
            {
                if (m[mx] == 255) { line[x] = solid_color; }
                else if (m[mx] > 0)
                {
                    uint8_t ca = (solid_color >> 24) & 0xFF;
                    line[x] = (solid_color & 0x00FFFFFF) | ((uint32_t)((ca * m[mx]) >> 8) << 24);
                }
            }
        }
        // Bottom
        for (int y = h - r, my = r - 1; y < h; y++, my--)
        {
            uint32_t *line = pixels + y * w;
            uint8_t *m = mask + my * r;
            for (int x = 0; x < r; x++) // BL
            {
                if (m[x] == 255) { line[x] = solid_color; }
                else if (m[x] > 0)
                {
                    uint8_t ca = (solid_color >> 24) & 0xFF;
                    line[x] = (solid_color & 0x00FFFFFF) | ((uint32_t)((ca * m[x]) >> 8) << 24);
                }
            }
            for (int x = r; x < w - r; x++) { line[x] = solid_color; } // BM
            for (int x = w - r, mx = r - 1; x < w; x++, mx--) // BR
            {
                if (m[mx] == 255) { line[x] = solid_color; }
                else if (m[mx] > 0)
                {
                    uint8_t ca = (solid_color >> 24) & 0xFF;
                    line[x] = (solid_color & 0x00FFFFFF) | ((uint32_t)((ca * m[mx]) >> 8) << 24);
                }
            }
        }
    }

    // Zone 2: Middle
    int start_y = r;
    int end_y = h - r;
    for (int y = start_y; y < end_y; y++)
    {
        uint32_t *line = pixels + y * w;
        for (int x = 0; x < w; x++) { line[x] = solid_color; }
    }

    if (mask) { gui_free(mask); }
}

/**
 * Fill a rounded-rect coverage mask, one byte per pixel.
 *
 * Simpler than the ARGB8888 twin, since generate_corner_mask() already yields
 * the coverage and there is no colour to blend it into -- but the colour's alpha
 * is folded in with that twin's exact arithmetic so the two agree byte for byte:
 * fully covered pixels take colour alpha, corner pixels scale it by coverage.
 */
static void fill_solid_rounded_mask_a8(gui_rounded_rect_t *this, uint8_t *mask_out,
                                       int w, int h, int r)
{
    if (r * 2 > w) { r = w / 2; }
    if (r * 2 > h) { r = h / 2; }

    uint8_t color_a = this->color.color.rgba.a;

    uint8_t *corner = NULL;
    if (r > 0)
    {
        corner = generate_corner_mask(r);
        if (corner == NULL) { return; }
    }

    /* One corner row: coverage 255 keeps colour alpha, 0 stays clear (the buffer
     * is already zeroed), anything between scales colour alpha. */
#define MASK_CORNER_ROW(line, m)                                                    \
    do {                                                                            \
        for (int x = 0; x < r; x++)                                                 \
        {                                                                           \
            uint8_t cov = (m)[x];                                                   \
            if (cov == 255)  { (line)[x] = color_a; }                               \
            else if (cov > 0) { (line)[x] = (uint8_t)(((uint32_t)color_a * cov) >> 8); } \
        }                                                                           \
        memset((line) + r, color_a, (size_t)(w - 2 * r));                           \
        for (int x = w - r, mx = r - 1; x < w; x++, mx--)                           \
        {                                                                           \
            uint8_t cov = (m)[mx];                                                  \
            if (cov == 255)  { (line)[x] = color_a; }                               \
            else if (cov > 0) { (line)[x] = (uint8_t)(((uint32_t)color_a * cov) >> 8); } \
        }                                                                           \
    } while (0)

    /* Top corners plus the strip between them. */
    for (int y = 0; y < r; y++)
    {
        MASK_CORNER_ROW(mask_out + y * w, corner + y * r);
    }

    /* Bottom corners, walking the corner mask back up. */
    for (int y = h - r, my = r - 1; y < h; y++, my--)
    {
        MASK_CORNER_ROW(mask_out + y * w, corner + my * r);
    }
#undef MASK_CORNER_ROW

    /* Everything between the corner bands is fully covered. */
    for (int y = r; y < h - r; y++)
    {
        memset(mask_out + y * w, color_a, (size_t)w);
    }

    if (corner != NULL) { gui_free(corner); }
}

static draw_img_t *create_rounded_rect_buffer(gui_rounded_rect_t *this, gui_obj_t *obj,
                                              draw_img_t **old_img)
{
    // Free old buffer first to prevent memory leak
    free_draw_img(old_img);

    uint8_t *body = NULL;
    bool is_new = false;
    draw_img_t *img = alloc_rect_img_buffer(this, obj, &body, &is_new);
    if (img == NULL) { return NULL; }

    if (!is_new)
    {
        // An identical rect already rasterised these pixels
        return img;
    }

    int w = this->base.w;
    int h = this->base.h;
    int r = this->radius;

    bool use_gradient = (this->use_gradient && this->gradient != NULL &&
                         this->gradient->stop_count >= 2);

    if (rect_use_a8(this))
    {
        fill_solid_rounded_mask_a8(this, body, w, h, r);
    }
    else if (use_gradient)
    {
        fill_gradient_rounded_rect(this, (uint32_t *)body, w, h, r);
    }
    else
    {
        fill_solid_rounded_rect(this, (uint32_t *)body, w, h, r);
    }
    return img;
}

/** Create corner image for specific corner (Legacy/Fallback) */
static draw_img_t *create_corner_img(gui_rounded_rect_t *this, gui_obj_t *obj,
                                     int corner_idx, int x, int y, draw_img_t **old_img)
{
    // Free old buffer first to prevent memory leak
    free_draw_img(old_img);

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    int size = this->radius + 1;

    /* Keyed by corner index as well as radius, so all four corners of every rect
     * with this radius come from just four payloads -- and as a mask, regardless
     * of colour. */
    bool is_a8 = rect_use_a8(this);
    rect_desc_t desc;
    bool is_new = false;
    rect_desc_init(&desc, this, RECT_PART_CORNER, this->radius, corner_idx);

    uint32_t pixel_bytes = is_a8 ? 1u : 4u;
    uint32_t buffer_size = size * size * pixel_bytes + sizeof(gui_rgb_data_head_t);
    uint8_t *circle_data = gui_shape_cache_acquire(&desc, rect_desc_len(&desc), buffer_size,
                                                   &is_new);
    if (circle_data == NULL)
    {
        gui_free(img);
        return NULL;
    }

    if (is_new)
    {
        memset(circle_data, 0x00, buffer_size);

        if (is_a8)
        {
            set_a8_header((gui_rgb_data_head_t *)circle_data, (uint16_t)size, (uint16_t)size);
        }
        else
        {
            gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)circle_data;
            head->scan = 0;
            head->align = 0;
            head->resize = 0;
            head->compress = 0;
            head->rsvd = 0;
            head->type = ARGB8888;
            head->w = size;
            head->h = size;
        }

        prepare_arc_img(this, circle_data, corner_idx, is_a8);
    }

    set_img_payload(this, img, circle_data, is_a8);

    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    matrix_translate(x, y, &img->matrix);
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

static void gui_rect_prepare(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
    uint32_t last = this->checksum;

    // obj->matrix is already initialized by gui_obj_ctor
    // Don't reinitialize it - it may contain parent transformations (e.g., list scrolling)

    // Detect non-translate parent transform (rotation/scale/skew)
    bool parent_has_non_translate = !matrix_only_translate(obj->matrix);

    // Apply transformations if needed
    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f ||
                          this->offset_x != 0.0f || this->offset_y != 0.0f);

    if (has_transform)
    {
        float center_x = this->base.w / 2.0f;
        float center_y = this->base.h / 2.0f;

        matrix_translate(this->offset_x, this->offset_y, obj->matrix);
        matrix_translate(center_x, center_y, obj->matrix);
        matrix_scale(this->scale_x, this->scale_y, obj->matrix);
        matrix_rotate(this->degrees, obj->matrix);
        matrix_translate(-center_x, -center_y, obj->matrix);
    }

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, "touch");

    // Calculate checksum only for key properties (exclude pointers)
    uint32_t new_checksum = 2166136261u;
    new_checksum = rect_checksum(new_checksum, &this->opacity_value, sizeof(this->opacity_value));
    new_checksum = rect_checksum(new_checksum, &this->radius, sizeof(this->radius));
    new_checksum = rect_checksum(new_checksum, &this->color, sizeof(this->color));
    new_checksum = rect_checksum(new_checksum, &this->degrees, sizeof(this->degrees));
    new_checksum = rect_checksum(new_checksum, &this->scale_x, sizeof(this->scale_x));
    new_checksum = rect_checksum(new_checksum, &this->scale_y, sizeof(this->scale_y));
    new_checksum = rect_checksum(new_checksum, &this->offset_x, sizeof(this->offset_x));
    new_checksum = rect_checksum(new_checksum, &this->offset_y, sizeof(this->offset_y));
    new_checksum = rect_checksum(new_checksum, &this->use_gradient, sizeof(this->use_gradient));
    new_checksum = rect_checksum(new_checksum, &this->enable_dither, sizeof(this->enable_dither));
    new_checksum = rect_checksum(new_checksum, &this->gradient_dir, sizeof(this->gradient_dir));
    new_checksum = rect_checksum(new_checksum, &this->base.w, sizeof(this->base.w));
    new_checksum = rect_checksum(new_checksum, &this->base.h, sizeof(this->base.h));
    // Handle bit-field hidden with temporary variable
    uint32_t hidden_val = obj->hidden;
    new_checksum = rect_checksum(new_checksum, &hidden_val, sizeof(hidden_val));

    if (this->gradient != NULL)
    {
        new_checksum = rect_checksum(new_checksum, this->gradient, sizeof(Gradient));
    }

    bool need_regenerate = (last != new_checksum);

    // FORCE single buffer for gradient or alpha for consistency
    bool need_single_buffer = (this->color.color.rgba.a < 255) ||
                              (this->base.w * this->base.h <= 10000) ||
                              (this->use_gradient && this->gradient != NULL) ||
                              has_transform || parent_has_non_translate;

    if (this->radius == 0 && !this->use_gradient)
    {
        if (need_regenerate || this->rect_0 == NULL)
        {
            set_rect_img(this, &this->rect_0, 0, 0, this->base.w, this->base.h);
        }
    }
    else if (need_single_buffer)
    {
        // Free split-rendering buffers in case we switched from split to single-buffer path.
        // matrix_changed uses rect_1 == NULL to detect single-buffer mode, so these must be NULL.
        free_draw_img(&this->rect_1);
        free_draw_img(&this->rect_2);
        free_draw_img(&this->circle_00);
        free_draw_img(&this->circle_01);
        free_draw_img(&this->circle_10);
        free_draw_img(&this->circle_11);
        if (need_regenerate || this->rect_0 == NULL)
        {
            this->rect_0 = create_rounded_rect_buffer(this, obj, &this->rect_0);
        }
    }
    else
    {
        // Legacy split rendering for large solid rounded rects
        if (need_regenerate || this->rect_0 == NULL)
        {
            set_rect_img(this, &this->rect_0, \
                         this->radius + 1,  \
                         0,
                         this->base.w - 2 * (this->radius + 1), \
                         this->radius + 1);

            set_rect_img(this, &this->rect_1, \
                         0, \
                         this->radius + 1, \
                         this->base.w, \
                         this->base.h - 2 * (this->radius + 1));

            set_rect_img(this, &this->rect_2, \
                         this->radius + 1,  \
                         this->base.h - this->radius - 1,
                         this->base.w - 2 * (this->radius + 1), \
                         this->radius + 1);

            this->circle_00 = create_corner_img(this, obj, 0, 0, 0, &this->circle_00);
            this->circle_01 = create_corner_img(this, obj, 1, this->base.w - this->radius - 1, 0,
                                                &this->circle_01);
            this->circle_10 = create_corner_img(this, obj, 3, 0, this->base.h - this->radius - 1,
                                                &this->circle_10);
            this->circle_11 = create_corner_img(this, obj, 2, this->base.w - this->radius - 1,
                                                this->base.h - this->radius - 1, &this->circle_11);
        }
    }

    this->checksum = new_checksum;

    // Check if matrix changed (important for list scrolling optimization)
    bool matrix_changed = (memcmp(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t)) != 0);

    // Only update draw_img matrix and area when matrix actually changed
    if (matrix_changed)
    {
        memcpy(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t));

        if (this->radius == 0 && !this->use_gradient)
        {
            // Simple rect case - rect_0 covers the whole area
            if (this->rect_0 != NULL)
            {
                memcpy(&this->rect_0->matrix, obj->matrix, sizeof(struct gui_matrix));
                memcpy(&this->rect_0->inverse, obj->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->rect_0->inverse);
                draw_img_new_area(this->rect_0, NULL);
            }
        }
        else if (this->rect_1 == NULL)
        {
            // Single buffer case (gradient or small rect)
            if (this->rect_0 != NULL)
            {
                memcpy(&this->rect_0->matrix, obj->matrix, sizeof(struct gui_matrix));
                memcpy(&this->rect_0->inverse, obj->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->rect_0->inverse);
                draw_img_new_area(this->rect_0, NULL);
            }
        }
        else
        {
            // Split rendering case - need to apply offsets
            if (this->rect_0 != NULL)
            {
                memcpy(&this->rect_0->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate(this->radius + 1, 0, &this->rect_0->matrix);
                memcpy(&this->rect_0->inverse, &this->rect_0->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->rect_0->inverse);
                draw_img_new_area(this->rect_0, NULL);
            }
            if (this->rect_1 != NULL)
            {
                memcpy(&this->rect_1->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate(0, this->radius + 1, &this->rect_1->matrix);
                memcpy(&this->rect_1->inverse, &this->rect_1->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->rect_1->inverse);
                draw_img_new_area(this->rect_1, NULL);
            }
            if (this->rect_2 != NULL)
            {
                memcpy(&this->rect_2->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate(this->radius + 1, this->base.h - this->radius - 1, &this->rect_2->matrix);
                memcpy(&this->rect_2->inverse, &this->rect_2->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->rect_2->inverse);
                draw_img_new_area(this->rect_2, NULL);
            }
            if (this->circle_00 != NULL)
            {
                memcpy(&this->circle_00->matrix, obj->matrix, sizeof(struct gui_matrix));
                memcpy(&this->circle_00->inverse, obj->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->circle_00->inverse);
                draw_img_new_area(this->circle_00, NULL);
            }
            if (this->circle_01 != NULL)
            {
                memcpy(&this->circle_01->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate(this->base.w - this->radius - 1, 0, &this->circle_01->matrix);
                memcpy(&this->circle_01->inverse, &this->circle_01->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->circle_01->inverse);
                draw_img_new_area(this->circle_01, NULL);
            }
            if (this->circle_10 != NULL)
            {
                memcpy(&this->circle_10->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate(0, this->base.h - this->radius - 1, &this->circle_10->matrix);
                memcpy(&this->circle_10->inverse, &this->circle_10->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->circle_10->inverse);
                draw_img_new_area(this->circle_10, NULL);
            }
            if (this->circle_11 != NULL)
            {
                memcpy(&this->circle_11->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate(this->base.w - this->radius - 1, this->base.h - this->radius - 1,
                                 &this->circle_11->matrix);
                memcpy(&this->circle_11->inverse, &this->circle_11->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->circle_11->inverse);
                draw_img_new_area(this->circle_11, NULL);
            }
        }
    }

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

/** Drawing phase processing */
static void gui_rect_draw(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    // Update opacity value to consider parent's opacity (like gui_img does)
    uint8_t final_opacity = obj->parent->opacity_value * this->opacity_value / UINT8_MAX;

    if (this->rect_0 != NULL)
    {
        this->rect_0->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->rect_0, dc, NULL);
    }
    if (this->rect_1 != NULL)
    {
        this->rect_1->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->rect_1, dc, NULL);
    }
    if (this->rect_2 != NULL)
    {
        this->rect_2->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->rect_2, dc, NULL);
    }
    if (this->circle_00 != NULL)
    {
        this->circle_00->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->circle_00, dc, NULL);
    }
    if (this->circle_01 != NULL)
    {
        this->circle_01->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->circle_01, dc, NULL);
    }
    if (this->circle_10 != NULL)
    {
        this->circle_10->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->circle_10, dc, NULL);
    }
    if (this->circle_11 != NULL)
    {
        this->circle_11->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->circle_11, dc, NULL);
    }
}
static void gui_rect_end(gui_obj_t *obj)
{
    /* Pixel buffers are cached across frames and must NOT be freed here.
     * However HW-acceleration user data (acc_user, e.g. the boundary-line
     * cache allocated by hw_acc_prepare_cb) is allocated every frame inside
     * draw_img_new_area -> draw_img_acc_prepare_cb.  We must release it now
     * so the next frame's prepare call does not orphan the old pointer. */
    if (draw_img_acc_end_cb != NULL)
    {
        gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
        if (this->rect_0   != NULL) { draw_img_acc_end_cb(this->rect_0);   }
        if (this->rect_1   != NULL) { draw_img_acc_end_cb(this->rect_1);   }
        if (this->rect_2   != NULL) { draw_img_acc_end_cb(this->rect_2);   }
        if (this->circle_00 != NULL) { draw_img_acc_end_cb(this->circle_00); }
        if (this->circle_01 != NULL) { draw_img_acc_end_cb(this->circle_01); }
        if (this->circle_10 != NULL) { draw_img_acc_end_cb(this->circle_10); }
        if (this->circle_11 != NULL) { draw_img_acc_end_cb(this->circle_11); }
    }
}

static void gui_rect_destroy(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;

    // Free gradient data
    if (this->gradient != NULL)
    {
        gui_free(this->gradient);
        this->gradient = NULL;
    }

    // Free cached buffers using the helper function
    free_draw_img(&this->rect_0);
    free_draw_img(&this->rect_1);
    free_draw_img(&this->rect_2);
    free_draw_img(&this->circle_00);
    free_draw_img(&this->circle_01);
    free_draw_img(&this->circle_10);
    free_draw_img(&this->circle_11);
}

static void gui_rect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_rect_prepare(obj);
            break;
        case OBJ_DRAW:
            gui_rect_draw(obj);
            break;
        case OBJ_END:
            gui_rect_end(obj);
            break;
        case OBJ_DESTROY:
            gui_rect_destroy(obj);
            break;
        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_rounded_rect_t *gui_rect_create(void *parent, const char *name, int x, int y,
                                    int w, int h,
                                    int radius, gui_color_t color)
{
    GUI_ASSERT(parent != NULL);
    gui_rounded_rect_t *round_rect = gui_malloc(sizeof(gui_rounded_rect_t));
    GUI_ASSERT(round_rect != NULL);
    memset(round_rect, 0x00, sizeof(gui_rounded_rect_t));

    round_rect->opacity_value = UINT8_MAX;
    gui_obj_ctor((gui_obj_t *)round_rect, parent, name, x, y, w, h);
    GET_BASE(round_rect)->obj_cb = gui_rect_cb;
    GET_BASE(round_rect)->has_input_prepare_cb = true;
    GET_BASE(round_rect)->has_prepare_cb = true;
    GET_BASE(round_rect)->has_draw_cb = true;
    GET_BASE(round_rect)->has_end_cb = true;
    GET_BASE(round_rect)->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(round_rect)->child_list));
    if ((GET_BASE(round_rect)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(round_rect)->parent)->child_list),
                               &(GET_BASE(round_rect)->brother_list));
    }
    GET_BASE(round_rect)->create_done = true;
    round_rect->radius = radius;
    round_rect->color = color;
    round_rect->degrees = 0.0f;
    round_rect->scale_x = 1.0f;
    round_rect->scale_y = 1.0f;
    round_rect->offset_x = 0.0f;
    round_rect->offset_y = 0.0f;
    round_rect->gradient = NULL;
    round_rect->use_gradient = false;
    round_rect->gradient_dir = RECT_GRADIENT_HORIZONTAL;

    return round_rect;
}

void gui_rect_set_style(gui_rounded_rect_t *rect,
                        int x, int y, int w, int h,
                        int radius, gui_color_t color)
{
    GUI_ASSERT(rect != NULL);
    rect->base.x = x;
    rect->base.y = y;
    rect->base.w = w;
    rect->base.h = h;
    rect->radius = radius;
    rect->color = color;
}
void gui_rect_set_opacity(gui_rounded_rect_t *rect, uint8_t opacity)
{
    GUI_ASSERT(rect != NULL);
    rect->opacity_value = opacity;
}
void gui_rect_set_position(gui_rounded_rect_t *rect, int x, int y)
{
    GUI_ASSERT(rect != NULL);
    rect->base.x = x;
    rect->base.y = y;
}

void gui_rect_set_size(gui_rounded_rect_t *rect, int w, int h)
{
    GUI_ASSERT(rect != NULL);
    rect->base.w = w;
    rect->base.h = h;
}

void gui_rect_set_radius(gui_rounded_rect_t *rect, int radius)
{
    GUI_ASSERT(rect != NULL);
    rect->radius = radius;
}

void gui_rect_set_color(gui_rounded_rect_t *rect, gui_color_t color)
{
    GUI_ASSERT(rect != NULL);
    rect->color = color;
}

void gui_rect_on_click(gui_rounded_rect_t *rect, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)rect, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}

void gui_rect_rotate(gui_rounded_rect_t *rect, float degrees)
{
    GUI_ASSERT(rect != NULL);
    rect->degrees = degrees;
}

void gui_rect_scale(gui_rounded_rect_t *rect, float scale_x, float scale_y)
{
    GUI_ASSERT(rect != NULL);
    rect->scale_x = scale_x;
    rect->scale_y = scale_y;
}

void gui_rect_translate(gui_rounded_rect_t *rect, float tx, float ty)
{
    GUI_ASSERT(rect != NULL);
    rect->offset_x = tx;
    rect->offset_y = ty;
}

void gui_rect_set_linear_gradient(gui_rounded_rect_t *rect, gui_rect_gradient_dir_t direction)
{
    GUI_ASSERT(rect != NULL);
    if (rect->gradient == NULL)
    {
        rect->gradient = gui_malloc(sizeof(Gradient));
        if (rect->gradient == NULL) { return; }
    }
    gradient_init(rect->gradient, GRADIENT_LINEAR);
    rect->gradient_dir = direction;
    rect->use_gradient = true;
}

void gui_rect_add_gradient_stop(gui_rounded_rect_t *rect, float position, gui_color_t color)
{
    GUI_ASSERT(rect != NULL);
    if (rect->gradient == NULL)
    {
        gui_rect_set_linear_gradient(rect, RECT_GRADIENT_HORIZONTAL);
    }
    if (rect->gradient != NULL)
    {
        gradient_add_stop(rect->gradient, position, color.color.argb_full);
    }
}

void gui_rect_clear_gradient(gui_rounded_rect_t *rect)
{
    GUI_ASSERT(rect != NULL);
    if (rect->gradient != NULL)
    {
        gui_free(rect->gradient);
        rect->gradient = NULL;
    }
    rect->use_gradient = false;
}
