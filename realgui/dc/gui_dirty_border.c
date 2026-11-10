/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "guidef.h"
#include "gui_dirty_border.h"

/* Storage for previous frame's dirty regions (for cleanup in next frame) */
static gui_rect_t s_prev_regions[GUI_MAX_DIRTY_REGIONS];
static uint16_t   s_prev_count = 0;

/* Border enable/disable control */
static bool s_border_enabled = false;
static bool s_prev_border_enabled = false;

static bool rect_intersection_border(const gui_rect_t *a, const gui_rect_t *b, gui_rect_t *result)
{
    result->x1 = _UI_MAX(a->x1, b->x1);
    result->y1 = _UI_MAX(a->y1, b->y1);
    result->x2 = _UI_MIN(a->x2, b->x2);
    result->y2 = _UI_MIN(a->y2, b->y2);

    return (result->x1 <= result->x2) && (result->y1 <= result->y2);
}

static inline void write_pixel(uint8_t *pixel, uint32_t color, uint8_t bpp)
{
    if (bpp == 2)
    {
        *(uint16_t *)pixel = (uint16_t)color;
    }
    else if (bpp == 3)
    {
        pixel[0] = (color >> 0) & 0xFF;
        pixel[1] = (color >> 8) & 0xFF;
        pixel[2] = (color >> 16) & 0xFF;
    }
    else if (bpp == 4)
    {
        *(uint32_t *)pixel = color;
    }
}

void draw_dirty_border(gui_dispdev_t *dc, gui_rect_t *rect)
{
    if (dc->frame_buf == NULL || rect == NULL)
    {
        return;
    }

    gui_rect_t visible;
    if (!rect_intersection_border(rect, &dc->section, &visible))
    {
        return;
    }

    int16_t local_x1 = visible.x1 - dc->section.x1;
    int16_t local_y1 = visible.y1 - dc->section.y1;
    int16_t local_x2 = visible.x2 - dc->section.x1;
    int16_t local_y2 = visible.y2 - dc->section.y1;

    if (local_x1 < 0 || local_y1 < 0 || local_x2 >= dc->fb_width || local_y2 >= dc->fb_height)
    {
        return;
    }

    uint8_t bpp = 0;
    uint32_t red_color = 0;

    switch (dc->bit_depth)
    {
    case 16:
        bpp = 2;
        red_color = 0xF800;
        break;
    case 24:
        bpp = 3;
        red_color = 0x0000FF;
        break;
    case 32:
        bpp = 4;
        red_color = 0xFFFF0000;
        break;
    default:
        return;
    }

    uint8_t *fb = dc->frame_buf;
    uint32_t stride = (dc->section.x2 - dc->section.x1 + 1) * bpp;

    if (visible.y1 == rect->y1)
    {
        for (int16_t x = local_x1; x <= local_x2; x++)
        {
            write_pixel(fb + local_y1 * stride + x * bpp, red_color, bpp);
        }
    }

    if (visible.y2 == rect->y2)
    {
        for (int16_t x = local_x1; x <= local_x2; x++)
        {
            write_pixel(fb + local_y2 * stride + x * bpp, red_color, bpp);
        }
    }

    if (visible.x1 == rect->x1)
    {
        for (int16_t y = local_y1; y <= local_y2; y++)
        {
            write_pixel(fb + y * stride + local_x1 * bpp, red_color, bpp);
        }
    }

    if (visible.x2 == rect->x2)
    {
        for (int16_t y = local_y1; y <= local_y2; y++)
        {
            write_pixel(fb + y * stride + local_x2 * bpp, red_color, bpp);
        }
    }
}

void gui_dirty_border_enable(bool enable)
{
    s_border_enabled = enable;
}

bool gui_dirty_border_is_enabled(void)
{
    return s_border_enabled;
}

uint16_t dirty_border_prepare(gui_dirty_region_manager_t *mgr)
{
    /* Both disabled now and previously: nothing to do */
    if (!s_border_enabled && !s_prev_border_enabled)
    {
        s_prev_count = 0;
        return mgr->count;
    }

    /* In full_refresh mode or no dirty regions, no cleanup needed */
    if (mgr->full_refresh || mgr->count == 0)
    {
        /* full_refresh will overwrite everything, no cleanup needed next frame */
        s_prev_count = 0;
        s_prev_border_enabled = s_border_enabled;
        return mgr->count;
    }

    uint16_t real_count = mgr->count;

    /* Inject cleanup rects if previous frame drew borders */
    if (s_prev_border_enabled && s_prev_count > 0)
    {
        for (uint16_t i = 0; i < s_prev_count; i++)
        {
            gui_rect_t *r = &s_prev_regions[i];
            gui_rect_t edges[4] =
            {
                {r->x1, r->y1, r->x2, r->y1},  /* top    (1px tall) */
                {r->x1, r->y2, r->x2, r->y2},  /* bottom (1px tall) */
                {r->x1, r->y1, r->x1, r->y2},  /* left   (1px wide) */
                {r->x2, r->y1, r->x2, r->y2},  /* right  (1px wide) */
            };
            for (uint16_t e = 0; e < 4 && mgr->count < GUI_MAX_DIRTY_REGIONS; e++)
            {
                mgr->regions[mgr->count++] = edges[e];
            }
        }
    }

    /* Save current frame's regions only if borders will be drawn */
    if (s_border_enabled)
    {
        s_prev_count = real_count;
        memcpy(s_prev_regions, mgr->regions, real_count * sizeof(gui_rect_t));
    }
    else
    {
        s_prev_count = 0;
    }

    s_prev_border_enabled = s_border_enabled;

    return real_count;
}
