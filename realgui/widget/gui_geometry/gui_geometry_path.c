/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "gui_geometry_path.h"

void gui_geometry_path_init_circle(gui_geometry_path_t *path, int radius, float inset)
{
    if (path == NULL || radius <= 0)
    {
        return;
    }

    memset(path, 0x00, sizeof(*path));
    path->magic = GUI_GEOMETRY_PATH_MAGIC;
    path->type = GUI_GEOMETRY_PRIMITIVE_CIRCLE;
    path->width = (uint16_t)(radius * 2);
    path->height = (uint16_t)(radius * 2);
    path->radius = (float)radius;
    path->inset = inset < 0.0f ? 0.0f : (inset > (float)radius ? (float)radius : inset);
    path->samples = GUI_GEOMETRY_PATH_SAMPLES;
}

void gui_geometry_path_init_rounded_rect(gui_geometry_path_t *path, int width, int height,
                                         int radius, float inset)
{
    float limit;

    if (path == NULL || width <= 0 || height <= 0)
    {
        return;
    }

    limit = (float)(width < height ? width : height) * 0.5f;
    memset(path, 0x00, sizeof(*path));
    path->magic = GUI_GEOMETRY_PATH_MAGIC;
    path->type = GUI_GEOMETRY_PRIMITIVE_ROUNDED_RECT;
    path->width = (uint16_t)width;
    path->height = (uint16_t)height;
    path->radius = (float)radius;
    path->inset = inset < 0.0f ? 0.0f : (inset > limit ? limit : inset);
    path->samples = GUI_GEOMETRY_PATH_SAMPLES;
}
