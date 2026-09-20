/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_GEOMETRY_RASTER_H__
#define __GUI_GEOMETRY_RASTER_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gui_geometry_path.h"
/*============================================================================*
 *                         Constants
 *============================================================================*/
#define RASTER_ZONE_MAX 4u
/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief Hard-edged and rounded primitives the rasteriser understands.
 *
 * Each type is stroked, not filled: the primitive describes an outer boundary
 * and an inset of the same shape, and the painted region is the ring between
 * them.  A filled shape is the degenerate case where the inset reaches the
 * centre so the inner region vanishes.
 */
/**
 * Sub-scanlines evaluated per pixel row.
 *
 * Coverage is exact along x and box-filtered along y, so this is the only
 * quality knob: raising it smooths horizontal edges (the top and bottom caps of
 * a ring), while vertical edges are already exact.  Four matches what the
 * supersampled implementation produced on both axes.
 */
#define GUI_GEOMETRY_RASTER_SAMPLES GUI_GEOMETRY_PATH_SAMPLES

/**
 * @brief One scanline's slice of the run store.
 *
 * Rows are laid out first, then the runs they point into, so a row's runs are
 * contiguous and a partial-row decode (dirty rectangles, section clipping) can
 * walk straight to the runs it needs.
 */
typedef struct
{
    uint32_t first_run;                 /**< Index of the row's first run. */
    uint16_t run_count;                 /**< Runs belonging to this row. */
    uint16_t reserved;                  /**< Keeps the row 8 bytes, zeroed. */
} gui_geometry_span_row_t;

/**
 * @brief A horizontal run of pixels sharing one coverage value.
 *
 * Coverage changes only near a boundary, so a row collapses to a handful of
 * runs rather than one entry per pixel.  Runs never span rows.
 */
typedef struct
{
    uint16_t x;                         /**< First covered pixel column. */
    uint16_t length;                    /**< Covered pixel count, >= 1. */
    uint8_t alpha;                      /**< Coverage, 1..255; 0 never stored. */
    uint8_t reserved;                   /**< Keeps the run 6 bytes, zeroed. */
} gui_geometry_span_run_t;

/**
 * @brief Rasterised coverage for one shape: a row table followed by runs.
 *
 * The row table holds @c height entries and the runs @c run_count, so the
 * payload is @c height * sizeof(row) + run_count * sizeof(run) bytes past the
 * header.  Only coverage is stored -- never colour -- so one payload is shared
 * by every widget drawing that shape, at any opacity, in any colour.
 */
typedef struct gui_geometry_span
{
    uint16_t width;
    uint16_t height;
    uint32_t run_count;
    uint8_t data[];
} gui_geometry_span_t;
/**
 * The horizontal extent of one shape boundary on one sub-scanline.
 *
 * A stroked primitive is an outer span minus an inner span, both convex, so
 * each contributes at most one interval per sub-scanline and the pair is enough
 * to describe the whole scanline.  Everything the rasteriser needs is derived
 * from these two intervals, which is why no per-pixel distance test appears
 * anywhere below.
 */
typedef struct
{
    float oa;               /**< Outer span start, valid when has_outer. */
    float ob;               /**< Outer span end, valid when has_outer. */
    float ia;               /**< Inner span start, valid when has_inner. */
    float ib;               /**< Inner span end, valid when has_inner. */
    bool has_outer;         /**< False when the sub-scanline misses the shape. */
    bool has_inner;         /**< False when the inset hole is absent here. */
} raster_bands_t;

/**
 * A column range that has to be evaluated pixel by pixel.
 *
 * Between two of these the coverage is constant, because no sub-scanline has a
 * boundary crossing there.  That is the whole point of the type: it lets a row
 * be walked in a handful of narrow zones with one probe per gap, instead of
 * once per column.
 */
typedef struct
{
    int32_t lo;             /**< First column to evaluate individually. */
    int32_t hi;             /**< Last column to evaluate individually. */
} raster_zone_t;

/**
 * The column range a row needs to walk, and the shortcuts that shrink it.
 *
 * Derived once per row so the per-pixel loop compares against plain bounds
 * instead of re-deriving geometry.
 */
typedef struct
{
    int32_t x_from;         /**< First column that can hold coverage. */
    int32_t x_to;           /**< Last column that can hold coverage. */
    int32_t hole_from;      /**< First column proven unpainted. */
    int32_t hole_to;        /**< Last column proven unpainted; < from when none. */
    bool fast_ok;           /**< Whether the fully-covered shortcut applies. */
    float full_lo;          /**< Start of the span every sub-scanline covers. */
    float full_hi;          /**< End of that span. */
    float gap_lo;           /**< Start of the union of the inner spans. */
    float gap_hi;           /**< End of that union. */
    uint32_t zone_count;    /**< Zones below; 1 spanning the row when unprovable. */
    raster_zone_t zone[RASTER_ZONE_MAX];
} raster_row_span_t;

/**
 * The run currently being accumulated, before it is known to be complete.
 */
typedef struct
{
    int32_t x;              /**< First column of the open run. */
    uint16_t length;        /**< Columns gathered so far; 0 means closed. */
    uint8_t alpha;          /**< Coverage shared by those columns. */
} raster_run_builder_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Count the runs a shape rasterises to, without writing any.
 *
 * Shares its scan core with gui_geometry_raster_build(), so the two always agree:
 * a payload sized from this count cannot overflow.  Callers need the count up
 * front because the payload is allocated at its exact size.
 *
 * @param path Shape to measure.
 * @return Number of runs, or 0 if the shape covers no pixels.
 */
uint32_t gui_geometry_raster_run_count(const gui_geometry_path_t *path);

/**
 * @brief Rasterise @p path into @p spans.
 *
 * On entry @c spans->run_count holds the run capacity. On success it is
 * replaced with the number of runs written; @c width and @c height are also
 * written. This permits a caller to reserve an estimated capacity and rasterise
 * in one pass.
 *
 * @param path  Shape to rasterise.
 * @param spans Destination, sized by gui_geometry_span_bytes().
 * @return true on success, false if the runs did not fit the capacity.
 */
bool gui_geometry_raster_build(const gui_geometry_path_t *path,
                               gui_geometry_span_t *spans);

/**
 * @brief Total payload size in bytes for a shape's rasterised coverage.
 *
 * Includes the header, the row table and the runs, so it can size an allocation
 * directly.
 *
 * @param run_count Runs the shape rasterises to.
 * @param height    Shape height in pixels.
 * @return Size in bytes, excluding any enclosing cache node.
 */
uint64_t gui_geometry_span_bytes(uint32_t run_count, uint16_t height);

/**
 * @brief The row table of a rasterised shape.
 *
 * @param spans Payload from gui_geometry_raster_build().
 * @return Pointer to @c height rows.
 */
static inline gui_geometry_span_row_t *gui_geometry_span_rows(
    gui_geometry_span_t *spans)
{
    return (gui_geometry_span_row_t *)(void *)spans->data;
}

/**
 * @brief The row table of a rasterised shape, read-only.
 *
 * @param spans Payload from gui_geometry_raster_build().
 * @return Pointer to @c height rows.
 */
static inline const gui_geometry_span_row_t *gui_geometry_span_rows_const(
    const gui_geometry_span_t *spans)
{
    return (const gui_geometry_span_row_t *)(const void *)spans->data;
}

/**
 * @brief The run store of a rasterised shape.
 *
 * @param spans Payload from gui_geometry_raster_build().
 * @return Pointer to @c run_count runs, addressed through the row table.
 */
static inline gui_geometry_span_run_t *gui_geometry_span_runs(
    gui_geometry_span_t *spans)
{
    return (gui_geometry_span_run_t *)(void *)(spans->data +
                                               (size_t)spans->height *
                                               sizeof(gui_geometry_span_row_t));
}

/**
 * @brief The run store of a rasterised shape, read-only.
 *
 * @param spans Payload from gui_geometry_raster_build().
 * @return Pointer to @c run_count runs, addressed through the row table.
 */
static inline const gui_geometry_span_run_t *gui_geometry_span_runs_const(
    const gui_geometry_span_t *spans)
{
    return (const gui_geometry_span_run_t *)(const void *)(spans->data +
                                                           (size_t)spans->height *
                                                           sizeof(gui_geometry_span_row_t));
}

#ifdef __cplusplus
}
#endif

#endif /* __GUI_GEOMETRY_RASTER_H__ */
