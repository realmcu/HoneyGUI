/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_GEOMETRY_SPAN_H__
#define __GUI_GEOMETRY_SPAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_matrix.h"
#include "gui_geometry_raster.h"

/**
 * Rasterised coverage of a stroked primitive.
 *
 * The geometry types, the rasteriser and the payload layout all live in
 * gui_geometry_raster.h, which stays free of GUI headers so it can be exercised on
 * a host.  This alias keeps the name the widget layer already uses.
 */
gui_geometry_span_t *gui_geometry_span_acquire(const gui_geometry_path_t *path);
void gui_geometry_span_release(const gui_geometry_span_t *spans);

bool gui_geometry_span_can_draw(const gui_matrix_t *matrix);
bool gui_geometry_span_draw(const gui_geometry_span_t *spans,
                            const gui_matrix_t *matrix,
                            gui_color_t color, uint8_t opacity,
                            gui_dispdev_t *dc);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_SPAN_H__ */
