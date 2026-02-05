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
#include "guidef.h"
#include "gui_fb.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "gui_rect.h"
#include "lite_geometry.h"

/*============================================================================*
 *                           Configuration
 *============================================================================*/
// 1 = Enable Dither by default (Better quality on 565/666 screens)
// 0 = Disable Dither (Maximum speed, banding may appear on gradients)
#define GUI_RECT_ENABLE_DITHER  1

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/** Safely free a draw_img_t and its data */
static void free_draw_img(draw_img_t **img)
{
    if (img == NULL || *img == NULL) { return; }

    if ((*img)->data != NULL)
    {
        gui_free((void *)(*img)->data);
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
static uint8_t *create_solid_color_buffer(uint16_t w, uint16_t h, gui_color_t color)
{
    uint32_t buffer_size = w * h * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = gui_malloc(buffer_size);
    if (buffer == NULL) { return NULL; }

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
    memset(img, 0x00, sizeof(draw_img_t));

    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f);

    if (has_transform)
    {
        img->blend_mode = IMG_SRC_OVER_MODE;
        img->high_quality = 1;
        img->data = create_solid_color_buffer(w, h, this->color);
        if (img->data == NULL)
        {
            gui_free(img);
            *input_img = NULL;
            return;
        }
    }
    else
    {
        img->blend_mode = IMG_RECT;
        gui_rect_file_head_t *rect_data = gui_malloc(sizeof(gui_rect_file_head_t));
        set_rect_header((gui_rgb_data_head_t *)rect_data, w, h, this->color);
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

/** Prepare arc image data for a specific corner (Legacy/Fallback) */
static void prepare_arc_img(gui_rounded_rect_t *this, uint8_t *circle_data, int corner_type)
{
    if (this->radius == 0) { return; }
    uint32_t *data = (uint32_t *)(circle_data + sizeof(gui_rgb_data_head_t));
    uint16_t img_size = this->radius + 1;
    memset(data, 0, img_size * img_size * 4);

    // Calculate boundary points for anti-aliasing (including row 0)
    float boundary[img_size];
    memset(boundary, 0, sizeof(boundary));
    for (int i = 0; i < img_size; i++)
    {
        float y = i + 0.5 - this->radius;
        float x_boundary = this->radius - sqrtf(this->radius * this->radius - y * y);
        boundary[i] = (x_boundary < 0) ? 0 : x_boundary;
    }

    // Fill the corner based on corner type
    for (int i = 0; i < img_size; i++)
    {
        uint16_t right = (int)boundary[i];

        // Fill solid pixels
        if (right < img_size)
        {
            for (int j = right + 2; j < img_size; j++)
            {
                int fill_i, fill_j;
                switch (corner_type)
                {
                case 0: // Top-left
                    fill_i = i;
                    fill_j = j;
                    break;
                case 1: // Top-right
                    fill_i = i;
                    fill_j = img_size - 1 - j;
                    break;
                case 2: // Bottom-right
                    fill_i = img_size - 1 - i;
                    fill_j = img_size - 1 - j;
                    break;
                case 3: // Bottom-left
                    fill_i = img_size - 1 - i;
                    fill_j = j;
                    break;
                default:
                    fill_i = i;
                    fill_j = j;
                    break;
                }
                if (fill_i >= 0 && fill_i < img_size && fill_j >= 0 && fill_j < img_size)
                {
                    data[fill_i * img_size + fill_j] = this->color.color.argb_full;
                }
            }
        }

        // Anti-aliasing edge pixel
        float portion = ceilf(boundary[i]) - boundary[i];
        gui_color_t color = this->color;
        color.color.rgba.a = (uint8_t)(portion * color.color.rgba.a);

        int edge_i, edge_j;
        switch (corner_type)
        {
        case 0: // Top-left
            edge_i = i;
            edge_j = right + 1;
            break;
        case 1: // Top-right
            edge_i = i;
            edge_j = img_size - 1 - (right + 1);
            break;
        case 2: // Bottom-right
            edge_i = img_size - 1 - i;
            edge_j = img_size - 1 - (right + 1);
            break;
        case 3: // Bottom-left
            edge_i = img_size - 1 - i;
            edge_j = right + 1;
            break;
        default:
            edge_i = i;
            edge_j = right + 1;
            break;
        }

        if (edge_i >= 0 && edge_i < img_size && edge_j >= 0 && edge_j < img_size)
        {
            data[edge_i * img_size + edge_j] = color.color.argb_full;
        }
    }
}

// ============================================================================
//  HIGH PERFORMANCE RENDERER (ZONE BASED + PRE-CALC)
// ============================================================================

/**
 * Allocate buffer for single-buffer rendering
 */
static draw_img_t *alloc_rect_img_buffer(gui_rounded_rect_t *this, gui_obj_t *obj,
                                         uint32_t **out_pixels)
{
    int w = this->base.w;
    int h = this->base.h;

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    uint32_t buffer_size = w * h * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = gui_malloc(buffer_size);
    if (buffer == NULL)
    {
        gui_free(img);
        return NULL;
    }
    memset(buffer, 0x00, buffer_size);

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)buffer;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = w;
    head->h = h;

    img->data = buffer;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = this->opacity_value;
    img->high_quality = 1;

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

    *out_pixels = (uint32_t *)(buffer + sizeof(gui_rgb_data_head_t));
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

static draw_img_t *create_rounded_rect_buffer(gui_rounded_rect_t *this, gui_obj_t *obj,
                                              draw_img_t **old_img)
{
    // Free old buffer first to prevent memory leak
    free_draw_img(old_img);

    uint32_t *pixels = NULL;
    draw_img_t *img = alloc_rect_img_buffer(this, obj, &pixels);
    if (img == NULL) { return NULL; }

    int w = this->base.w;
    int h = this->base.h;
    int r = this->radius;

    bool use_gradient = (this->use_gradient && this->gradient != NULL &&
                         this->gradient->stop_count >= 2);

    if (use_gradient)
    {
        fill_gradient_rounded_rect(this, pixels, w, h, r);
    }
    else
    {
        fill_solid_rounded_rect(this, pixels, w, h, r);
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
    uint32_t buffer_size = size * size * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *circle_data = gui_malloc(buffer_size);
    if (circle_data == NULL)
    {
        gui_free(img);
        return NULL;
    }
    memset(circle_data, 0x00, buffer_size);

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)circle_data;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = size;
    head->h = size;

    prepare_arc_img(this, circle_data, corner_idx);

    img->data = circle_data;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = this->opacity_value;
    img->high_quality = 1;

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
    uint8_t last = this->checksum;

    // obj->matrix is already initialized by gui_obj_ctor
    // Don't reinitialize it - it may contain parent transformations (e.g., list scrolling)

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

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, NULL);
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, NULL);

    // Calculate checksum only for key properties (exclude pointers)
    uint8_t new_checksum = 0;
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->opacity_value,
                                    sizeof(this->opacity_value));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->radius, sizeof(this->radius));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->color, sizeof(this->color));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->degrees, sizeof(this->degrees));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->scale_x, sizeof(this->scale_x));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->scale_y, sizeof(this->scale_y));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->offset_x, sizeof(this->offset_x));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->offset_y, sizeof(this->offset_y));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->use_gradient,
                                    sizeof(this->use_gradient));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->enable_dither,
                                    sizeof(this->enable_dither));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->gradient_dir,
                                    sizeof(this->gradient_dir));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->base.w, sizeof(this->base.w));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->base.h, sizeof(this->base.h));
    // Handle bit-field not_show with temporary variable
    uint32_t not_show_val = obj->not_show;
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&not_show_val, sizeof(not_show_val));

    if (this->gradient != NULL)
    {
        new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)this->gradient, sizeof(Gradient));
    }

    bool need_regenerate = (last != new_checksum);

    // FORCE single buffer for gradient or alpha for consistency
    bool need_single_buffer = (this->color.color.rgba.a < 255) ||
                              (this->base.w * this->base.h <= 10000) ||
                              (this->use_gradient && this->gradient != NULL);

    if (this->radius == 0 && !this->use_gradient)
    {
        if (need_regenerate || this->rect_0 == NULL)
        {
            set_rect_img(this, &this->rect_0, 0, 0, this->base.w, this->base.h);
        }
    }
    else if (need_single_buffer)
    {
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

    if (this->rect_0 != NULL) { gui_acc_blit_to_dc(this->rect_0, dc, NULL); }
    if (this->rect_1 != NULL) { gui_acc_blit_to_dc(this->rect_1, dc, NULL); }
    if (this->rect_2 != NULL) { gui_acc_blit_to_dc(this->rect_2, dc, NULL); }
    if (this->circle_00 != NULL) { gui_acc_blit_to_dc(this->circle_00, dc, NULL); }
    if (this->circle_01 != NULL) { gui_acc_blit_to_dc(this->circle_01, dc, NULL); }
    if (this->circle_10 != NULL) { gui_acc_blit_to_dc(this->circle_10, dc, NULL); }
    if (this->circle_11 != NULL) { gui_acc_blit_to_dc(this->circle_11, dc, NULL); }
}
static void gui_rect_end(gui_obj_t *obj)
{
    // DO NOT free buffers here - they are cached for reuse
    // Buffers will be freed in gui_rect_destory() when widget is destroyed
    (void)obj; // Suppress unused parameter warning
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

    if (this->circle_data != NULL)
    {
        gui_free(this->circle_data);
        this->circle_data = NULL;
    }
    if (this->rect_data != NULL)
    {
        gui_free(this->rect_data);
        this->rect_data = NULL;
    }
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
    rect->opacity_value = color.color.rgba.a;
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
    rect->opacity_value = color.color.rgba.a;
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
