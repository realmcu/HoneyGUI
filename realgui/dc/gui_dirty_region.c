/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_dirty_region.h"
#include "def_fb.h"
#include "gui_api.h"
#include "gui_fb.h"
#include <string.h>

/*============================================================================*
 *                         Private Variables
 *============================================================================*/

/** Global dirty region manager instance */
static gui_dirty_region_manager_t g_dirty_mgr =
{
    .count = 0,
    .full_refresh = false,
};

/** Dirty region optimization enable flag */
static bool s_dirty_region_enabled = true;

/*============================================================================*
 *                         Private Functions
 *============================================================================*/

/**
 * @brief Check if two rectangles overlap or are adjacent
 *
 * @param a Rectangle A
 * @param b Rectangle B
 * @return true if overlapping or adjacent, false if separated
 */
static bool rects_overlap_or_adjacent(const gui_rect_t *a, const gui_rect_t *b)
{
    // Return false if rectangles are completely separated (with gap)
    // Otherwise considered overlapping or adjacent
    return !(a->x2 < b->x1 - 1 ||
             a->x1 > b->x2 + 1 ||
             a->y2 < b->y1 - 1 ||
             a->y1 > b->y2 + 1);
}

/**
 * @brief Merge two rectangles (compute bounding box)
 *
 * @param a Rectangle A (result stored here)
 * @param b Rectangle B
 */
static void merge_rects(gui_rect_t *a, const gui_rect_t *b)
{
    a->x1 = _UI_MIN(a->x1, b->x1);
    a->y1 = _UI_MIN(a->y1, b->y1);
    a->x2 = _UI_MAX(a->x2, b->x2);
    a->y2 = _UI_MAX(a->y2, b->y2);
}

/**
 * @brief Clip rectangle to screen bounds
 *
 * @param rect Input/output rectangle
 * @param screen_width Screen width
 * @param screen_height Screen height
 * @return true if rectangle is valid, false if outside screen
 */
static bool clip_rect_to_screen(gui_rect_t *rect, int16_t screen_width, int16_t screen_height)
{
    rect->x1 = _UI_MAX(0, rect->x1);
    rect->y1 = _UI_MAX(0, rect->y1);
    rect->x2 = _UI_MIN(rect->x2, screen_width - 1);
    rect->y2 = _UI_MIN(rect->y2, screen_height - 1);

    // Check if still valid after clipping
    return (rect->x1 <= rect->x2) && (rect->y1 <= rect->y2);
}

/*============================================================================*
 *                         Public Functions
 *============================================================================*/

void gui_dirty_region_enable(bool enable)
{
    s_dirty_region_enabled = enable;
    if (!enable)
    {
        /* When disabling, clear all dirty regions and force full refresh */
        g_dirty_mgr.count = 0;
        g_dirty_mgr.full_refresh = true;
    }
}

bool gui_dirty_region_is_enabled(void)
{
    return s_dirty_region_enabled;
}

void gui_dirty_set_region(gui_rect_t *rect)
{
    if (rect == NULL)
    {
        return;
    }

    /* When dirty region optimization is disabled, force full refresh */
    if (!s_dirty_region_enabled)
    {
        g_dirty_mgr.full_refresh = true;
        gui_fb_change();
        return;
    }

    // Get screen dimensions
    gui_dispdev_t *dc = gui_get_dc();
    if (dc == NULL)
    {
        return;
    }

    // 1. Clip to screen bounds
    if (!clip_rect_to_screen(rect, dc->screen_width, dc->screen_height))
    {
        // Rectangle is completely outside screen, ignore
        return;
    }

    // 2. Try to merge with existing dirty regions
    for (uint16_t i = 0; i < g_dirty_mgr.count; i++)
    {
        if (rects_overlap_or_adjacent(rect, &g_dirty_mgr.regions[i]))
        {
            // Merge with existing dirty region
            merge_rects(&g_dirty_mgr.regions[i], rect);

            // Set refresh flag
            gui_fb_change();
            return;
        }
    }

    // 3. Cannot merge, check for overflow
    if (g_dirty_mgr.count >= GUI_MAX_DIRTY_REGIONS)
    {
        // Exceeds maximum count, fall back to full screen refresh
        g_dirty_mgr.full_refresh = true;
        gui_fb_change();
        return;
    }

    // 4. Add as new dirty region
    g_dirty_mgr.regions[g_dirty_mgr.count] = *rect;
    g_dirty_mgr.count++;

    // Set refresh flag
    gui_fb_change();
}

gui_dirty_region_manager_t *gui_dirty_region_get_manager(void)
{
    return &g_dirty_mgr;
}

void gui_dirty_region_clear(void)
{
    g_dirty_mgr.count = 0;
    g_dirty_mgr.full_refresh = false;
}
