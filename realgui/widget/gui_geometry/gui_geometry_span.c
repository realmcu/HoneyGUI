/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                             Header Files
 *============================================================================*/
#include <math.h>
#include <stddef.h>
#include <string.h>
#include "gui_api.h"
#include "gui_geometry_span.h"
#include "gui_geometry_cache.h"
#include "gui_geometry_raster.h"
#include "lite_geometry.h"

/*============================================================================*
 *                         Constants
 *============================================================================*/

#ifndef GUI_GEOMETRY_SPAN_PROFILE
#define GUI_GEOMETRY_SPAN_PROFILE 0
#endif

GUI_GEOMETRY_DESC_SIZE_CHECK(gui_geometry_path_t);

/*============================================================================*
 *                         Functions
 *============================================================================*/

static uint32_t path_estimated_runs(const gui_geometry_path_t *path)
{
    uint32_t runs_per_row = path->type == GUI_GEOMETRY_PRIMITIVE_CIRCLE ? 10u : 8u;
    uint64_t estimate = (uint64_t)path->height * runs_per_row;

    return estimate > UINT32_MAX ? UINT32_MAX : (uint32_t)estimate;
}

static gui_geometry_span_t *path_acquire_exact(const gui_geometry_path_t *path)
{
    uint32_t run_count = gui_geometry_raster_run_count(path);
    if (run_count == 0u)
    {
        return NULL;
    }

    uint64_t size = gui_geometry_span_bytes(run_count, path->height);
    if (size > UINT32_MAX)
    {
        return NULL;
    }

    bool is_new = false;
    uint8_t *payload = gui_geometry_cache_acquire(path, sizeof(*path), (uint32_t)size, &is_new);
    if (payload == NULL)
    {
        return NULL;
    }

    gui_geometry_span_t *spans = (gui_geometry_span_t *)payload;
    if (is_new)
    {
        memset(spans, 0x00, (size_t)size);
        spans->run_count = run_count;
        if (!gui_geometry_raster_build(path, spans))
        {
            gui_geometry_cache_release(payload);
            return NULL;
        }
    }

    return spans;
}

static bool path_finish_resizable(const gui_geometry_path_t *path,
                                  gui_geometry_span_t *spans)
{
    uint64_t size = gui_geometry_span_bytes(spans->run_count, path->height);

    return size <= UINT32_MAX &&
           gui_geometry_cache_rekey(spans, path, sizeof(*path), (uint32_t)size);
}

static gui_geometry_span_t *path_acquire_resizable(const gui_geometry_path_t *path,
                                                   uint32_t estimated_runs,
                                                   bool *is_new)
{
    uint64_t capacity = gui_geometry_span_bytes(estimated_runs, path->height);
    uint64_t initial_size = gui_geometry_span_bytes(0u, path->height);

    if (estimated_runs == 0u || capacity > UINT32_MAX || initial_size > UINT32_MAX)
    {
        return NULL;
    }

    uint8_t *payload = gui_geometry_cache_acquire_resizable(path, sizeof(*path),
                                                            (uint32_t)initial_size,
                                                            (uint32_t)capacity, is_new);
    if (payload == NULL)
    {
        return NULL;
    }

    gui_geometry_span_t *spans = (gui_geometry_span_t *)payload;
    if (*is_new)
    {
        memset(spans, 0x00, (size_t)initial_size);
        spans->run_count = estimated_runs;
    }
    return spans;
}

gui_geometry_span_t *gui_geometry_span_acquire(const gui_geometry_path_t *path)
{
    if (path == NULL || path->magic != GUI_GEOMETRY_PATH_MAGIC ||
        path->width == 0u || path->height == 0u)
    {
        return NULL;
    }

#if GUI_GEOMETRY_SPAN_PROFILE
    uint32_t acquire_start_us = gui_us_get();
#endif
    bool is_new = false;
    uint32_t estimate_runs = path_estimated_runs(path);
    gui_geometry_span_t *spans = path_acquire_resizable(path, estimate_runs, &is_new);
    if (spans == NULL) { return NULL; }
    if (is_new)
    {
#if GUI_GEOMETRY_SPAN_PROFILE
        uint32_t build_start_us = gui_us_get();
#endif
        if (!gui_geometry_raster_build(path, spans))
        {
            gui_geometry_cache_release(spans);
            return path_acquire_exact(path);
        }

        if (!path_finish_resizable(path, spans))
        {
            gui_geometry_cache_release(spans);
            return path_acquire_exact(path);
        }

#if GUI_GEOMETRY_SPAN_PROFILE
        uint32_t end_us = gui_us_get();
        uint64_t actual_size = gui_geometry_span_bytes(spans->run_count, path->height);
        gui_log("GEOMETRY_SPAN_ACQUIRE new %ux%u type=%u runs=%u payload=%u B:"
                " reserve=%u runs build=%u us total=%u us\n",
                (unsigned int)path->width, (unsigned int)path->height,
                (unsigned int)path->type, (unsigned int)spans->run_count,
                (unsigned int)actual_size, (unsigned int)estimate_runs,
                (unsigned int)(end_us - build_start_us),
                (unsigned int)(end_us - acquire_start_us));
#endif
    }

    return spans;
}

void gui_geometry_span_release(const gui_geometry_span_t *spans)
{
    gui_geometry_cache_release(spans);
}

bool gui_geometry_span_can_draw(const gui_matrix_t *matrix)
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

bool gui_geometry_span_draw(const gui_geometry_span_t *spans,
                            const gui_matrix_t *matrix,
                            gui_color_t color, uint8_t opacity,
                            gui_dispdev_t *dc)
{
    if (spans == NULL || matrix == NULL || dc == NULL || dc->frame_buf == NULL ||
        !gui_geometry_span_can_draw(matrix))
    {
        return false;
    }

    int origin_x = (int)roundf(matrix->m[0][2]);
    int origin_y = (int)roundf(matrix->m[1][2]);
    int section_width = dc->section.x2 - dc->section.x1 + 1;
    const gui_geometry_span_row_t *rows = gui_geometry_span_rows_const(spans);
    const gui_geometry_span_run_t *runs = gui_geometry_span_runs_const(spans);

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

        const gui_geometry_span_row_t *row = &rows[local_y];
        for (uint16_t r = 0; r < row->run_count; r++)
        {
            const gui_geometry_span_run_t *run = &runs[row->first_run + r];
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
