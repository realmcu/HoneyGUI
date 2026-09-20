/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_GEOMETRY_PATH_H__
#define __GUI_GEOMETRY_PATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GUI_GEOMETRY_PATH_MAGIC 0x50544853u
#define GUI_GEOMETRY_PATH_SAMPLES 4u

/*
 * A geometry path is intentionally paint-free. Widgets provide position through
 * their matrix and color through their existing paint/blit path.
 */
typedef enum
{
    GUI_GEOMETRY_PRIMITIVE_CIRCLE = 1,
    GUI_GEOMETRY_PRIMITIVE_ROUNDED_RECT,
} gui_geometry_primitive_t;

typedef struct
{
    uint32_t magic;
    uint32_t type;
    uint16_t width;
    uint16_t height;
    float radius;
    float inset;
    uint32_t samples;
} gui_geometry_path_t;

void gui_geometry_path_init_circle(gui_geometry_path_t *path, int radius, float inset);
void gui_geometry_path_init_rounded_rect(gui_geometry_path_t *path, int width, int height,
                                         int radius, float inset);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_PATH_H__ */
