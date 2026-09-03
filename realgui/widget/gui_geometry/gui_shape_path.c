/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <math.h>
#include <stddef.h>
#include <string.h>
#include "gui_api.h"
#include "gui_shape_path.h"
#include "gui_shape_cache.h"
#include "lite_geometry.h"

#define GUI_SHAPE_PATH_MAGIC 0x50544853u
#define GUI_SHAPE_PATH_SAMPLES 4u

typedef struct
{
    uint32_t first_run;
    uint16_t run_count;
    uint16_t reserved;
} gui_shape_span_row_t;

typedef struct
{
    uint16_t x;
    uint16_t length;
    uint8_t alpha;
    uint8_t reserved;
} gui_shape_span_run_t;

struct gui_shape_span_data
{
    uint16_t width;
    uint16_t height;
    uint32_t run_count;
    uint8_t data[];
};

GUI_SHAPE_DESC_SIZE_CHECK(gui_shape_path_t);

static gui_shape_span_row_t *span_rows(gui_shape_span_data_t *spans)
{
    return (gui_shape_span_row_t *)spans->data;
}

static const gui_shape_span_row_t *span_rows_const(const gui_shape_span_data_t *spans)
{
    return (const gui_shape_span_row_t *)spans->data;
}

static gui_shape_span_run_t *span_runs(gui_shape_span_data_t *spans)
{
    return (gui_shape_span_run_t *)(spans->data +
                                    (size_t)spans->height * sizeof(gui_shape_span_row_t));
}

static const gui_shape_span_run_t *span_runs_const(const gui_shape_span_data_t *spans)
{
    return (const gui_shape_span_run_t *)(spans->data +
                                          (size_t)spans->height *
                                          sizeof(gui_shape_span_row_t));
}

static float rounded_rect_sdf(float px, float py,
                              float x, float y, float width, float height,
                              float radius)
{
    float half_w = width * 0.5f;
    float half_h = height * 0.5f;
    float r = LG_CLAMP(radius, 0.0f, LG_MIN(half_w, half_h));
    float qx = fabsf(px - (x + half_w)) - (half_w - r);
    float qy = fabsf(py - (y + half_h)) - (half_h - r);
    float ox = LG_MAX(qx, 0.0f);
    float oy = LG_MAX(qy, 0.0f);

    return LG_MIN(LG_MAX(qx, qy), 0.0f) + sqrtf(ox * ox + oy * oy) - r;
}

static bool path_contains(const gui_shape_path_t *path, float px, float py, bool inner)
{
    float inset = inner ? path->inset : 0.0f;
    float width = (float)path->width - inset * 2.0f;
    float height = (float)path->height - inset * 2.0f;
    float radius = LG_MAX(path->radius - inset, 0.0f);

    if (width <= 0.0f || height <= 0.0f)
    {
        return false;
    }

    if (path->type == GUI_SHAPE_PATH_CIRCLE)
    {
        float cx = (float)path->width * 0.5f;
        float cy = (float)path->height * 0.5f;
        float dx = px - cx;
        float dy = py - cy;
        return dx * dx + dy * dy <= radius * radius;
    }

    return rounded_rect_sdf(px, py, inset, inset, width, height, radius) <= 0.0f;
}

static uint8_t path_pixel_coverage(const gui_shape_path_t *path, int x, int y)
{
    uint32_t covered = 0u;

    for (uint32_t sy = 0; sy < GUI_SHAPE_PATH_SAMPLES; sy++)
    {
        float py = y + (sy + 0.5f) / GUI_SHAPE_PATH_SAMPLES;
        for (uint32_t sx = 0; sx < GUI_SHAPE_PATH_SAMPLES; sx++)
        {
            float px = x + (sx + 0.5f) / GUI_SHAPE_PATH_SAMPLES;
            bool outer = path_contains(path, px, py, false);
            bool inner = path_contains(path, px, py, true);
            if (outer && !inner)
            {
                covered++;
            }
        }
    }

    return (uint8_t)((covered * 255u + 8u) /
                     (GUI_SHAPE_PATH_SAMPLES * GUI_SHAPE_PATH_SAMPLES));
}

static uint32_t count_path_runs(const gui_shape_path_t *path)
{
    uint32_t run_count = 0u;

    for (uint16_t y = 0; y < path->height; y++)
    {
        uint8_t previous = 0u;
        bool active = false;

        for (uint16_t x = 0; x < path->width; x++)
        {
            uint8_t alpha = path_pixel_coverage(path, x, y);
            if (alpha == 0u)
            {
                active = false;
            }
            else if (!active || alpha != previous)
            {
                run_count++;
                active = true;
                previous = alpha;
            }
        }
    }

    return run_count;
}

static bool build_path_runs(const gui_shape_path_t *path, gui_shape_span_data_t *spans)
{
    spans->width = path->width;
    spans->height = path->height;

    gui_shape_span_row_t *rows = span_rows(spans);
    gui_shape_span_run_t *runs = span_runs(spans);
    uint32_t run_index = 0u;

    for (uint16_t y = 0; y < path->height; y++)
    {
        uint32_t row_first = run_index;
        uint8_t previous = 0u;
        bool active = false;

        for (uint16_t x = 0; x < path->width; x++)
        {
            uint8_t alpha = path_pixel_coverage(path, x, y);
            if (alpha == 0u)
            {
                active = false;
                continue;
            }

            if (active && alpha == previous)
            {
                if (runs[run_index - 1u].length == UINT16_MAX)
                {
                    return false;
                }
                runs[run_index - 1u].length++;
                continue;
            }

            if (run_index >= spans->run_count)
            {
                return false;
            }

            runs[run_index].x = x;
            runs[run_index].length = 1u;
            runs[run_index].alpha = alpha;
            runs[run_index].reserved = 0u;
            run_index++;
            previous = alpha;
            active = true;
        }

        uint32_t row_count = run_index - row_first;
        if (row_count > UINT16_MAX)
        {
            return false;
        }
        rows[y].first_run = row_first;
        rows[y].run_count = (uint16_t)row_count;
        rows[y].reserved = 0u;
    }

    return run_index == spans->run_count;
}

void gui_shape_path_init_circle(gui_shape_path_t *path, int radius, float inset)
{
    GUI_ASSERT(path != NULL);
    memset(path, 0x00, sizeof(*path));
    path->magic = GUI_SHAPE_PATH_MAGIC;
    path->type = GUI_SHAPE_PATH_CIRCLE;
    path->width = (uint16_t)(radius * 2);
    path->height = (uint16_t)(radius * 2);
    path->radius = (float)radius;
    path->inset = LG_CLAMP(inset, 0.0f, (float)radius);
    path->samples = GUI_SHAPE_PATH_SAMPLES;
}

void gui_shape_path_init_rounded_rect(gui_shape_path_t *path, int width, int height,
                                      int radius, float inset)
{
    GUI_ASSERT(path != NULL);
    memset(path, 0x00, sizeof(*path));
    path->magic = GUI_SHAPE_PATH_MAGIC;
    path->type = GUI_SHAPE_PATH_ROUNDED_RECT;
    path->width = (uint16_t)width;
    path->height = (uint16_t)height;
    path->radius = (float)radius;
    path->inset = LG_CLAMP(inset, 0.0f, LG_MIN((float)width, (float)height) * 0.5f);
    path->samples = GUI_SHAPE_PATH_SAMPLES;
}

gui_shape_span_data_t *gui_shape_path_acquire(const gui_shape_path_t *path)
{
    if (path == NULL || path->magic != GUI_SHAPE_PATH_MAGIC ||
        path->width == 0u || path->height == 0u || path->inset <= 0.0f)
    {
        return NULL;
    }

    uint32_t run_count = count_path_runs(path);
    uint64_t size = sizeof(gui_shape_span_data_t) +
                    (uint64_t)path->height * sizeof(gui_shape_span_row_t) +
                    (uint64_t)run_count * sizeof(gui_shape_span_run_t);
    if (run_count == 0u || size > UINT32_MAX)
    {
        return NULL;
    }

    bool is_new = false;
    uint8_t *payload = gui_shape_cache_acquire(path, sizeof(*path), (uint32_t)size, &is_new);
    if (payload == NULL)
    {
        return NULL;
    }

    gui_shape_span_data_t *spans = (gui_shape_span_data_t *)payload;
    if (is_new)
    {
        memset(spans, 0x00, (size_t)size);
        spans->run_count = run_count;
        if (!build_path_runs(path, spans))
        {
            gui_shape_cache_release(payload);
            return NULL;
        }
    }

    return spans;
}

void gui_shape_path_release(const gui_shape_span_data_t *spans)
{
    gui_shape_cache_release(spans);
}

bool gui_shape_path_can_draw(const gui_matrix_t *matrix)
{
    if (!matrix_only_translate((gui_matrix_t *)matrix))
    {
        return false;
    }

    float tx = matrix->m[0][2];
    float ty = matrix->m[1][2];
    return fabsf(tx - roundf(tx)) < 0.001f &&
           fabsf(ty - roundf(ty)) < 0.001f;
}

static uint8_t combined_alpha(uint8_t coverage, uint8_t color_alpha, uint8_t opacity)
{
    uint32_t alpha = (uint32_t)coverage * color_alpha;
    alpha = (alpha + 127u) / 255u;
    alpha = alpha * opacity;
    return (uint8_t)((alpha + 127u) / 255u);
}

bool gui_shape_path_draw(const gui_shape_span_data_t *spans,
                         const gui_matrix_t *matrix,
                         gui_color_t color, uint8_t opacity,
                         gui_dispdev_t *dc)
{
    if (spans == NULL || matrix == NULL || dc == NULL || dc->frame_buf == NULL ||
        !gui_shape_path_can_draw(matrix))
    {
        return false;
    }

    int origin_x = (int)roundf(matrix->m[0][2]);
    int origin_y = (int)roundf(matrix->m[1][2]);
    int section_width = dc->section.x2 - dc->section.x1 + 1;
    const gui_shape_span_row_t *rows = span_rows_const(spans);
    const gui_shape_span_run_t *runs = span_runs_const(spans);

    uint16_t fg565 = (uint16_t)create_color_by_format(PIXEL_FORMAT_RGB565,
                                                      color.color.rgba.r,
                                                      color.color.rgba.g,
                                                      color.color.rgba.b, 255u);
    uint32_t fg8888 = 0xFF000000u | (color.color.argb_full & 0x00FFFFFFu);

    for (uint16_t local_y = 0; local_y < spans->height; local_y++)
    {
        int screen_y = origin_y + local_y;
        if (screen_y < dc->section.y1 || screen_y > dc->section.y2)
        {
            continue;
        }

        const gui_shape_span_row_t *row = &rows[local_y];
        for (uint16_t r = 0; r < row->run_count; r++)
        {
            const gui_shape_span_run_t *run = &runs[row->first_run + r];
            int x1 = origin_x + run->x;
            int x2 = x1 + run->length - 1;
            x1 = LG_MAX(x1, dc->section.x1);
            x2 = LG_MIN(x2, dc->section.x2);
            if (x1 > x2) { continue; }

            uint8_t alpha = combined_alpha(run->alpha, color.color.rgba.a, opacity);
            if (alpha == 0u) { continue; }

            int offset = (screen_y - dc->section.y1) * section_width +
                         x1 - dc->section.x1;
            if (dc->bit_depth == 16)
            {
                uint16_t *dst = (uint16_t *)dc->frame_buf + offset;
                for (int x = x1; x <= x2; x++, dst++)
                {
                    *dst = alpha == 255u ? fg565 :
                           (uint16_t)blend_colors_rgb565(*dst, fg565, alpha);
                }
            }
            else if (dc->bit_depth == 32)
            {
                uint32_t *dst = (uint32_t *)dc->frame_buf + offset;
                for (int x = x1; x <= x2; x++, dst++)
                {
                    *dst = alpha == 255u ? fg8888 :
                           blend_colors_argb8888(*dst, fg8888, alpha);
                }
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}
