/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "gui_fb.h"
#include "acc_api.h"
#include "gui_geometry_cache.h"
#include "gui_geometry_image.h"

void gui_geometry_data_head_set(gui_rgb_data_head_t *head, int16_t w, int16_t h,
                                GUI_FormatType type)
{
    GUI_ASSERT(head != NULL);

    memset(head, 0x00, sizeof(*head));
    head->type = (char)type;
    head->w = w;
    head->h = h;
}

bool gui_geometry_image_buffer_size(int32_t w, int32_t h, uint32_t pixel_bytes,
                                    uint32_t *buffer_size)
{
    if (buffer_size == NULL || w <= 0 || h <= 0 ||
        w > INT16_MAX || h > INT16_MAX || pixel_bytes == 0u)
    {
        return false;
    }

    uint64_t size = (uint64_t)(uint32_t)w * (uint32_t)h * pixel_bytes +
                    sizeof(gui_rgb_data_head_t);
    if (size > UINT32_MAX)
    {
        return false;
    }

    *buffer_size = (uint32_t)size;
    return true;
}

void gui_geometry_img_bind(draw_img_t *img, uint8_t *payload, uint8_t opacity,
                           bool is_a8, gui_color_t color)
{
    GUI_ASSERT(img != NULL);

    img->data = payload;
    img->opacity_value = opacity;
    img->high_quality = 1;

    if (is_a8)
    {
        img->blend_mode = IMG_2D_SW_FIX_A8_FG;
        img->fg_color_set = 0xFF000000u | (color.color.argb_full & 0x00FFFFFFu);
    }
    else
    {
        img->blend_mode = IMG_SRC_OVER_MODE;
    }
}

void gui_geometry_draw_img_release(draw_img_t **img)
{
    if (img == NULL || *img == NULL)
    {
        return;
    }

    if ((*img)->acc_user != NULL)
    {
        gui_free((*img)->acc_user);
        (*img)->acc_user = NULL;
    }

    if ((*img)->data != NULL)
    {
        if ((*img)->blend_mode == IMG_RECT)
        {
            gui_free((*img)->data);
        }
        else
        {
            gui_geometry_cache_release((*img)->data);
        }
    }

    gui_free(*img);
    *img = NULL;
}

uint32_t gui_geometry_dither_argb8888(uint32_t color, int x, int y)
{
    return gui_geometry_dither_argb8888_inline(color, x, y);
}

bool gui_geometry_use_a8(bool enabled, bool has_gradient, bool is_stroke)
{
    return enabled && (is_stroke || !has_gradient);
}

bool gui_geometry_use_split_a8(bool use_a8, uint8_t opacity, bool parent_opaque)
{
    return use_a8 && opacity == UINT8_MAX && parent_opaque;
}

/**
 * One pixel of a filled shape with an inset stroke, both already merged.
 *
 * @p c_out is the coverage of the outer edge and @p c_in that of the stroke's
 * inner edge (c_in <= c_out).  The stroke owns c_out - c_in of the pixel and
 * the fill owns c_in.  Those areas are disjoint, so they are summed rather
 * than layered: a pixel the shared edge crosses comes out fully covered and no
 * background shows through, whatever the alphas are.  Without a stroke, pass
 * c_in == c_out.
 */
uint32_t gui_geometry_mix_fill_stroke(uint32_t fill, uint32_t stroke, float c_out, float c_in)
{
    /* Nearly every pixel is pure fill or pure stroke; only the one-pixel seam
     * between them needs the weighted mix below. */
    if (c_in >= c_out || c_in <= 0.0f)
    {
        uint32_t color = (c_in > 0.0f) ? fill : stroke;
        if (c_out >= 1.0f) { return color; }

        uint32_t a = (uint32_t)((float)(color >> 24) * c_out + 0.5f);
        return (a << 24) | (color & 0x00FFFFFFu);
    }

    float wf = (float)(fill >> 24) * c_in;
    float ws = (float)(stroke >> 24) * (c_out - c_in);
    float a = wf + ws;
    if (a < 0.5f) { return 0u; }

    float inv = 1.0f / a;
    uint32_t r = (uint32_t)(((float)((fill >> 16) & 0xFFu) * wf +
                             (float)((stroke >> 16) & 0xFFu) * ws) * inv + 0.5f);
    uint32_t g = (uint32_t)(((float)((fill >> 8) & 0xFFu) * wf +
                             (float)((stroke >> 8) & 0xFFu) * ws) * inv + 0.5f);
    uint32_t b = (uint32_t)(((float)(fill & 0xFFu) * wf +
                             (float)(stroke & 0xFFu) * ws) * inv + 0.5f);

    return ((uint32_t)(a + 0.5f) << 24) | (r << 16) | (g << 8) | b;
}

/**
 * Take a w x h payload from the geometry cache, header already set when new.
 *
 * @param is_new Set true when the caller still has to rasterise the body.
 */
uint8_t *gui_geometry_payload_acquire(const void *desc, uint16_t desc_len,
                                      int w, int h, bool is_a8, bool *is_new)
{
    uint32_t buffer_size;
    if (!gui_geometry_image_buffer_size(w, h, is_a8 ? 1u : 4u, &buffer_size))
    {
        return NULL;
    }

    uint8_t *buffer = gui_geometry_cache_acquire(desc, desc_len, buffer_size, is_new);
    if (buffer != NULL && *is_new)
    {
        memset(buffer, 0x00, buffer_size);
        gui_geometry_data_head_set((gui_rgb_data_head_t *)buffer, (int16_t)w, (int16_t)h,
                                   is_a8 ? A8 : ARGB8888);
    }
    return buffer;
}

static void parts_place_one(gui_geometry_part_t *p, const gui_matrix_t *matrix)
{
    gui_matrix_t local =
    {
        {
            {(float)p->xx, (float)p->xy, (float)p->x},
            {(float)p->yx, (float)p->yy, (float)p->y},
            {0.0f, 0.0f, 1.0f}
        }
    };

    memcpy(&p->img->matrix, matrix, sizeof(gui_matrix_t));
    matrix_multiply(&p->img->matrix, &local);
    memcpy(&p->img->inverse, &p->img->matrix, sizeof(gui_matrix_t));
    matrix_inverse(&p->img->inverse);
    draw_img_load_scale(p->img, IMG_SRC_MEMADDR);
    draw_img_new_area(p->img, NULL);
}

/**
 * Append a piece; on failure @p img is released, so the caller never has to.
 *
 * (x, y) is where the piece's pixel (0, 0) lands, and (xx, xy, yx, yy) the
 * signed permutation applied around it -- see gui_geometry_part_t.
 */
bool gui_geometry_parts_add(gui_geometry_parts_t *parts, const gui_matrix_t *matrix,
                            draw_img_t *img, int x, int y,
                            int xx, int xy, int yx, int yy)
{
    if (img == NULL) { return false; }
    if (parts->count >= GUI_GEOMETRY_MAX_PARTS)
    {
        gui_geometry_draw_img_release(&img);
        return false;
    }

    gui_geometry_part_t *p = &parts->part[parts->count++];
    p->img = img;
    p->x = (int16_t)x;
    p->y = (int16_t)y;
    p->xx = (int8_t)xx;
    p->xy = (int8_t)xy;
    p->yx = (int8_t)yx;
    p->yy = (int8_t)yy;
    parts_place_one(p, matrix);
    return true;
}

/**
 * Append a piece drawing a cached payload.  Takes over the caller's reference
 * on @p payload, including on failure.
 */
bool gui_geometry_parts_add_image(gui_geometry_parts_t *parts, const gui_matrix_t *matrix,
                                  uint8_t *payload, uint8_t opacity, bool is_a8,
                                  gui_color_t color, int x, int y,
                                  int xx, int xy, int yx, int yy)
{
    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL)
    {
        gui_geometry_cache_release(payload);
        return false;
    }
    memset(img, 0x00, sizeof(draw_img_t));
    gui_geometry_img_bind(img, payload, opacity, is_a8, color);
    return gui_geometry_parts_add(parts, matrix, img, x, y, xx, xy, yx, yy);
}

/**
 * Append a solid rectangle piece.
 *
 * The payload is a bare header plus the colour -- no pixels, so there is no
 * rasterisation cost and no cached allocation.  The software rasteriser reads
 * the colour from the header and applies widget opacity itself, and a hardware
 * accelerator gets a solid rectangle primitive; neither needs a pixel buffer.
 * Empty or fully transparent rectangles add nothing and succeed.
 */
bool gui_geometry_parts_add_solid(gui_geometry_parts_t *parts, const gui_matrix_t *matrix,
                                  int x, int y, int w, int h, uint32_t argb, uint8_t opacity)
{
    if (w <= 0 || h <= 0 || (argb >> 24) == 0u)
    {
        return true;
    }

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return false; }
    memset(img, 0x00, sizeof(draw_img_t));

    gui_rect_file_head_t *head = gui_malloc(sizeof(gui_rect_file_head_t));
    if (head == NULL)
    {
        gui_free(img);
        return false;
    }

    gui_geometry_data_head_set((gui_rgb_data_head_t *)head, (int16_t)w, (int16_t)h, ARGB8888);
    head->color.color.argb_full = argb;

    img->blend_mode = IMG_RECT;
    img->data = head;
    img->opacity_value = opacity;
    return gui_geometry_parts_add(parts, matrix, img, x, y, 1, 0, 0, 1);
}

/** Re-place every piece after the widget moved; pixels are kept. */
void gui_geometry_parts_place(gui_geometry_parts_t *parts, const gui_matrix_t *matrix)
{
    for (uint8_t i = 0; i < parts->count; i++)
    {
        parts_place_one(&parts->part[i], matrix);
    }
}

void gui_geometry_parts_draw(gui_geometry_parts_t *parts, uint8_t opacity)
{
    gui_dispdev_t *dc = gui_get_dc();

    for (uint8_t i = 0; i < parts->count; i++)
    {
        parts->part[i].img->opacity_value = opacity;
        gui_acc_blit_to_dc(parts->part[i].img, dc, NULL);
    }
}

/**
 * Pixel buffers are cached across frames and must not be freed here, but
 * HW-acceleration user data (acc_user) is allocated every frame inside
 * draw_img_new_area, so it is released now or the next frame orphans it.
 */
void gui_geometry_parts_end(gui_geometry_parts_t *parts)
{
    if (draw_img_acc_end_cb == NULL) { return; }

    for (uint8_t i = 0; i < parts->count; i++)
    {
        draw_img_acc_end_cb(parts->part[i].img);
    }
}

void gui_geometry_parts_release(gui_geometry_parts_t *parts)
{
    for (uint8_t i = 0; i < parts->count; i++)
    {
        gui_geometry_draw_img_release(&parts->part[i].img);
    }
    parts->count = 0;
}
