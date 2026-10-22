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
#include "gui_obj.h"
#include "gui_fb.h"
#include "draw_img.h"
#include "acc_api.h"
#include "lite_geometry.h"
#include "gui_geometry_image.h"
#include "gui_geometry_span.h"
#include "gui_arc.h"
#include "gui_geometry_cache.h"

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** Slack kept around the outer radius when sizing the widget bounding box. */
#define GUI_ARC_BOX_MARGIN 2.0f

#ifndef GUI_ARC_ENABLE_INK_BOUNDS
#define GUI_ARC_ENABLE_INK_BOUNDS 1
#endif

/**
 * 1 = store solid arcs as A8 coverage masks, 0 = always ARGB8888.
 *
 * A solid arc is one colour, so the mask holds exactly the coverage the
 * rasteriser computes: a quarter of the memory, no colour in the cache key, and
 * acc_sw's A8 blit instead of the generic rasteriser.  A gradient arc keeps
 * ARGB8888 because its colour varies across the stroke.
 *
 * Set to 0 when bringing up an accelerator whose blit may not handle A8; the
 * failure mode is a garbled shape, not a clean fallback.  There is no scons
 * option for this -- the SConstruct does not forward command-line assignments.
 */
#ifndef GUI_ARC_ENABLE_A8
#define GUI_ARC_ENABLE_A8 1
#endif

/**
 * Store solid full rings as runs rather than as a byte per pixel.
 *
 * A ring inks a band around its rim, so the mask is mostly zeros.  Only arcs
 * arc_can_use_sparse() accepts qualify; the rest keep the contiguous mask, and
 * the pixels are the same either way.  Set to 0 to force the contiguous layout
 * for an A/B of memory against draw cost.
 */
#ifndef GUI_ARC_ENABLE_SPARSE
#define GUI_ARC_ENABLE_SPARSE 1
#endif

#ifndef GUI_ARC_ENABLE_INCREMENTAL
#define GUI_ARC_ENABLE_INCREMENTAL 1
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/

/**
 * Identity of a cached arc payload.
 *
 * Everything that changes a pixel goes in here and nothing else does; position
 * stays out, since it only ever reaches the draw_img matrix.  The payload
 * dimensions and centre offset belong here too: they decide where in the buffer
 * the ink lands.  Gradient arcs pass the whole struct, solid ones stop short of
 * the gradient -- see arc_desc_len().
 */
typedef struct
{
    int32_t radius;             /**< Arc radius. */
    int32_t buffer_w;           /**< Payload width. */
    int32_t buffer_h;           /**< Payload height. */
    int32_t off_x;              /**< Payload left edge within the widget box. */
    int32_t off_y;              /**< Payload top edge within the widget box. */
    float line_width;           /**< Stroke width. */
    float start_angle;          /**< Sweep start, degrees. */
    float end_angle;            /**< Sweep end, degrees. */
    uint32_t color;             /**< ARGB baked into the pixels; alpha only for masks. */
    uint32_t store;             /**< arc_store_t the payload is encoded as. */
    uint32_t use_gradient;      /**< Non-zero when the gradient below applies. */
    Gradient gradient;          /**< Only present when use_gradient is set. */
} arc_desc_t;

/* 160 bytes of a 192-byte ceiling, most of it the 116-byte Gradient.  When the
 * check fires, take the bytes out of Gradient or raise GUI_GEOMETRY_DESC_MAX. */
GUI_GEOMETRY_DESC_SIZE_CHECK(arc_desc_t);

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static bool init_arc_buffer(gui_arc_t *this, arc_store_t store, bool *out_need_render);
static bool render_arc_to_buffer(gui_arc_t *this);
static void gui_arc_release_payload(gui_arc_t *this);
static bool arc_can_use_a8(const gui_arc_t *this);
static bool arc_can_use_sparse(const gui_arc_t *this, const gui_matrix_t *matrix);
static uint16_t arc_desc_len(const arc_desc_t *desc);
static void arc_desc_init(arc_desc_t *desc, gui_arc_t *this,
                          int buffer_w, int buffer_h, int off_x, int off_y,
                          arc_store_t store);

/** Touch input preparation - simplified version like round_rect */
static void gui_arc_input_process(gui_obj_t *obj)
{
    // Arc widget uses the bounding box for touch detection
    // The event is already enabled in prepare phase
    // System will handle touch detection based on widget's x, y, w, h
    GUI_UNUSED(obj);
}

/** Widget box side length that holds an arc of this radius and line width. */
static int gui_arc_box_size(int radius, float line_width)
{
    float outer_r = (float)radius + line_width / 2.0f + GUI_ARC_BOX_MARGIN;
    return (int)(outer_r * 2) + 4;
}

static void gui_arc_prepare(gui_arc_t *this)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    // Initialize obj->matrix if not already done
    if (obj->matrix == NULL)
    {
        obj->matrix = gui_malloc(sizeof(gui_matrix_t));
        GUI_ASSERT(obj->matrix != NULL);
        // Reset matrix to identity before applying transformations
        matrix_identity(obj->matrix);
    }

    // Append this arc's own transform.  The render tree rebuilds obj->matrix
    // from the parent plus position each frame before OBJ_PRE_PROCESS, so these
    // are not idempotent and must not be.  Same contract as gui_img_prepare().
    float center_x = obj->w / 2.0f;
    float center_y = obj->h / 2.0f;

    // Apply offset
    matrix_translate(this->offset_x, this->offset_y, obj->matrix);
    // Translate to center
    matrix_translate(center_x, center_y, obj->matrix);
    // Apply scale
    matrix_scale(this->scale_x, this->scale_y, obj->matrix);
    // Apply rotation
    matrix_rotate(this->degrees, obj->matrix);
    // Translate back
    matrix_translate(-center_x, -center_y, obj->matrix);

    gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_TOUCH_LONG, "touch");
    gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_TOUCH_SCROLL_HORIZONTAL, "touch");
    arc_store_t store = arc_can_use_sparse(this, obj->matrix) ? ARC_STORE_SPARSE
                        : (arc_can_use_a8(this) ? ARC_STORE_A8 : ARC_STORE_ARGB8888);
    bool store_changed = (store != this->store);

    /* Every setter that changes a pixel clears buffer_valid, so this flag alone
     * decides whether the payload has to be re-acquired and redrawn -- except
     * for the storage mode, which depends on the matrix and can therefore flip
     * without any setter running. */
    if (!this->buffer_valid || store_changed)
    {
        bool need_render = false;

        if (!init_arc_buffer(this, store, &need_render))
        {
            gui_log("[GUI MODULE]Arc prepare: init_arc_buffer failed!\n");
            return;
        }
        if (need_render)
        {
            if (!render_arc_to_buffer(this))
            {
                gui_arc_release_payload(this);
                return;
            }
        }
        this->buffer_valid = true;
        this->end_angle_dirty = false;
        this->rendered_end_angle = this->end_angle;
        gui_fb_change();
    }
}

/** Drawing phase processing */
static void gui_arc_draw(gui_arc_t *this)
{
    /* Do not gate on buffer_valid: object timers run after OBJ_PRE_PROCESS in
     * the same frame, so a setter called from a timer clears buffer_valid
     * between prepare and draw.  The payload still holds last frame's pixels,
     * draw those and let the next prepare re-render.  Likewise when a re-render
     * fails to get memory, keep showing the old arc instead of nothing. */
    if (this->pixel_buffer == NULL)
    {
        return;
    }

    gui_obj_t *obj = (gui_obj_t *)this;
    gui_matrix_t matrix;

    // Initialize with identity matrix or copy user's matrix
    if (obj->matrix != NULL)
    {
        memcpy(&matrix, obj->matrix, sizeof(matrix));
    }
    else
    {
        matrix_identity(&matrix);
    }

    // Place the (possibly clipped) buffer back at its origin inside the box.
    matrix_translate((float)this->buffer_off_x, (float)this->buffer_off_y, &matrix);

    if (this->store == ARC_STORE_SPARSE)
    {
        gui_dispdev_t *dc = gui_get_dc();
        uint8_t opacity = (uint8_t)((uint32_t)obj->parent->opacity_value *
                                    this->opacity_value / UINT8_MAX);

        gui_color_t tint = this->color;
        tint.color.rgba.a = 0xFFu;

        if (gui_geometry_span_draw((const gui_geometry_span_t *)this->pixel_buffer,
                                   &matrix, tint, opacity, dc))
        {
            return;
        }
        this->buffer_valid = false;
        return;
    }

    if (this->draw_img != NULL)
    {
        // Update opacity value to consider parent's opacity (like gui_img does)
        this->draw_img->opacity_value = obj->parent->opacity_value * this->opacity_value / UINT8_MAX;

        memcpy(&this->draw_img->matrix, &matrix, sizeof(struct gui_matrix));
        memcpy(&this->draw_img->inverse, &matrix, sizeof(struct gui_matrix));
        matrix_inverse(&this->draw_img->inverse);

        // Prepare for rendering with updated matrix
        draw_img_load_scale(this->draw_img, IMG_SRC_MEMADDR);
        draw_img_new_area(this->draw_img, NULL);

        gui_dispdev_t *dc = gui_get_dc();
        gui_acc_blit_to_dc(this->draw_img, dc, NULL);
    }
}

/** End phase processing */
static void gui_arc_end(gui_arc_t *this)
{
    // Only call acc_end_cb, keep buffer cached
    if (this->draw_img != NULL && draw_img_acc_end_cb != NULL)
    {
        draw_img_acc_end_cb(this->draw_img);
    }
}

/**
 * Release the payload, whatever its layout.
 *
 * All three layouts come from the geometry cache, so one release covers them;
 * the draw_img only exists for the contiguous ones.
 */
static void gui_arc_release_payload(gui_arc_t *this)
{
    if (this->pixel_buffer != NULL)
    {
        gui_geometry_cache_release(this->pixel_buffer);
        this->pixel_buffer = NULL;
    }

    if (this->draw_img != NULL)
    {
        // draw_img->data aliases pixel_buffer, already released above
        gui_free(this->draw_img);
        this->draw_img = NULL;
    }

    this->store = ARC_STORE_NONE;
    this->buffer_valid = false;
    this->end_angle_dirty = false;
}

static void gui_arc_destroy(gui_arc_t *this)
{
    gui_arc_release_payload(this);

    if (this->gradient != NULL)
    {
        gui_free(this->gradient);
        this->gradient = NULL;
    }
}

static void gui_arc_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PROCESS:
            gui_arc_input_process(obj);
            break;
        case OBJ_PRE_PROCESS:
            gui_arc_prepare((gui_arc_t *)obj);
            break;

        case OBJ_PROCESS:
            gui_arc_draw((gui_arc_t *)obj);
            break;

        case OBJ_POST_PROCESS:
            gui_arc_end((gui_arc_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_arc_destroy((gui_arc_t *)obj);
            break;

        default:
            break;
        }
    }
}
/**
 * Sub-rect of the widget box that the arc actually inks.
 *
 * Sizing the pixel buffer to the swept sector instead of the whole bounding
 * square is where the memory goes: a 30-degree arc inks only a sliver of it.
 */
static void gui_arc_ink_box(gui_arc_t *this, int *off_x, int *off_y, int *w, int *h)
{
    gui_obj_t *obj = GUI_BASE(this);

#if !GUI_ARC_ENABLE_INK_BOUNDS
    *off_x = 0;
    *off_y = 0;
    *w = obj->w;
    *h = obj->h;
    return;
#endif

    float min_x, min_y, max_x, max_y;

    lg_arc_ink_bounds((float)this->radius, this->line_width,
                      this->start_angle, this->end_angle,
                      &min_x, &min_y, &max_x, &max_y);

    /* Centre of the widget box, kept in float so the sub-pixel placement of an
     * odd-sized box matches what obj->matrix rotates about. */
    float cx = obj->w / 2.0f;
    float cy = obj->h / 2.0f;

    int x0 = (int)floorf(cx + min_x);
    int y0 = (int)floorf(cy + min_y);
    int x1 = (int)ceilf(cx + max_x);
    int y1 = (int)ceilf(cy + max_y);

    /* Anything outside the widget box would be clipped away by the DC anyway. */
    if (x0 < 0) { x0 = 0; }
    if (y0 < 0) { y0 = 0; }
    if (x1 > obj->w) { x1 = obj->w; }
    if (y1 > obj->h) { y1 = obj->h; }

    *off_x = x0;
    *off_y = y0;
    *w = (x1 > x0) ? (x1 - x0) : 1;
    *h = (y1 > y0) ? (y1 - y0) : 1;
}

/**
 * Whether this arc's payload can be a coverage mask.
 *
 * Only a gradient rules it out, and only because a mask carries no colour:
 * the stroke is a single colour by construction, and its alpha is folded into
 * the mask values rather than carried beside them.  Folding it in matters for
 * the same reason it does for gui_circle -- the two blit paths disagree about
 * the fixed foreground's alpha, so it is pinned at 255 and the mask holds the
 * whole answer.
 */
static bool arc_can_use_a8(const gui_arc_t *this)
{
    return GUI_ARC_ENABLE_A8 && !(this->use_gradient && this->gradient != NULL);
}

/** Whether the sweep covers a whole turn, so the shape is a ring. */
static bool arc_is_full_ring(const gui_arc_t *this)
{
    float start = fmodf(this->start_angle, 360.0f);
    float end = fmodf(this->end_angle, 360.0f);

    if (start < 0.0f) { start += 360.0f; }
    if (end < 0.0f) { end += 360.0f; }

    float span = end - start;
    if (span <= 0.0f) { span += 360.0f; }

    /* draw_arc_df_aa() treats a span this close to 360 as a full circle, and the
     * sparse builder only exists for that path; anything else goes through the
     * cap and angular-clip code, which has no run sink. */
    return fabsf(span - 360.0f) < 0.001f;
}

/**
 * Whether this arc's payload should be run-encoded instead of contiguous.
 *
 * All four are hard limits:
 *  - a full ring, because only draw_arc_as_ring() has the run sink wired in;
 *  - a mask, since runs carry coverage and a gradient needs a colour per pixel;
 *  - a whole-pixel translation of the final draw matrix.  Runs are written
 *    straight to their columns with no resampling, so a fraction of a pixel
 *    cannot be carried and the arc would snap frame to frame instead of
 *    sliding.  Judged on @p matrix because a rotated parent bends the arc just
 *    as its own rotation does;
 *  - 16 or 32 bit output, the only depths gui_geometry_span_draw() can blend.
 *
 * Opacity and clipping are absent on purpose -- both act on the coverage at
 * draw time, so neither changes what is stored.
 */
static bool arc_can_use_sparse(const gui_arc_t *this, const gui_matrix_t *matrix)
{
    gui_dispdev_t *dc = gui_get_dc();

    return GUI_ARC_ENABLE_SPARSE &&
           arc_can_use_a8(this) &&
           arc_is_full_ring(this) &&
           matrix != NULL &&
           dc != NULL && (dc->bit_depth == 16 || dc->bit_depth == 32) &&
           gui_geometry_span_can_draw(matrix);
}

/** Descriptor bytes to compare: solid arcs stop before the gradient. */
static uint16_t arc_desc_len(const arc_desc_t *desc)
{
    if (desc->use_gradient == 0u)
    {
        return (uint16_t)offsetof(arc_desc_t, gradient);
    }
    return (uint16_t)sizeof(arc_desc_t);
}

/**
 * Fill in a descriptor for this arc's payload.
 *
 * The payload dimensions and centre offset are part of the identity, not just
 * the geometry: they decide where in the buffer the ink lands.
 *
 * Zeroes first: gui_geometry_cache_acquire() compares descriptors byte for byte, so
 * a padding hole left uninitialised would make two identical arcs miss.
 */
static void arc_desc_init(arc_desc_t *desc, gui_arc_t *this,
                          int buffer_w, int buffer_h, int off_x, int off_y,
                          arc_store_t store)
{
    memset(desc, 0x00, sizeof(*desc));

    desc->radius = this->radius;
    desc->buffer_w = buffer_w;
    desc->buffer_h = buffer_h;
    desc->off_x = off_x;
    desc->off_y = off_y;
    desc->line_width = this->line_width;
    desc->start_angle = this->start_angle;
    desc->end_angle = this->end_angle;

    /* RGB is deliberately left out of a mask key -- that is what lets arcs
     * differing only in colour share one mask.  Alpha stays in, because it is
     * folded into the mask values themselves.  Sparse payloads are masks too,
     * so they key the same way and a contiguous mask maps to the same runs. */
    desc->store = (uint32_t)store;
    if (store == ARC_STORE_A8 || store == ARC_STORE_SPARSE)
    {
        desc->color = this->color.color.rgba.a;
    }
    else
    {
        desc->color = this->color.color.argb_full;
    }

    if (this->use_gradient && this->gradient != NULL)
    {
        desc->use_gradient = 1u;
        memcpy(&desc->gradient, this->gradient, sizeof(Gradient));
    }
}

/**
 * Rasterise this arc's ring into @p sink.
 *
 * The one place the ring's coverage is produced.  Both storage layouts go
 * through it, so they cannot drift apart: they differ only in which sink they
 * hand it.  A NULL @p sink means "count the runs, keep nothing", which is how
 * the payload is sized before it exists.
 */
static void arc_raster_ring(gui_arc_t *this, gui_geometry_run_sink_t *sink,
                            int buffer_h, int off_x, int off_y)
{
    DrawContext draw_ctx;
    init_draw_context(&draw_ctx, NULL, sink->width, buffer_h, PIXEL_FORMAT_A8);
    draw_ctx.enable_aa = true;
    draw_ctx.clip_rect.x = 0;
    draw_ctx.clip_rect.y = 0;
    draw_ctx.clip_rect.w = sink->width;
    draw_ctx.clip_rect.h = buffer_h;
    draw_ctx.gradient = NULL;
    /* Only one shape ever writes here, so the sink's max-on-overlap is the
     * whole story and this flag has nothing to arbitrate. */
    draw_ctx.overlap_keeps_max_alpha = true;
    draw_ctx.run_sink = sink;

    gui_obj_t *obj = GUI_BASE(this);
    float center_x = obj->w / 2.0f - (float)off_x;
    float center_y = obj->h / 2.0f - (float)off_y;

    draw_arc_df_aa(&draw_ctx,
                   center_x,
                   center_y,
                   this->radius,
                   this->line_width,
                   this->start_angle,
                   this->end_angle,
                   this->color.color.argb_full);
}

/**
 * Runs this arc's ring rasterises to, without writing any.
 *
 * Shares arc_raster_ring() with the real render, so the count and the fill
 * agree and the payload cannot overflow.  The count sets the requested size;
 * the cache may still round it up to its next capacity class.
 *
 * @return false if the scratch row could not be allocated.
 */
static bool arc_sparse_run_count(gui_arc_t *this, int buffer_w, int buffer_h,
                                 int off_x, int off_y, uint32_t *out_runs)
{
    uint8_t *scratch = gui_malloc((size_t)buffer_w);
    if (scratch == NULL)
    {
        return false;
    }

    /* No row table and no run store: the sink only counts. */
    gui_geometry_run_sink_t sink;
    gui_geometry_run_sink_init(&sink, scratch, (uint16_t)buffer_w, (uint16_t)buffer_h,
                               NULL, NULL, 0u);

    arc_raster_ring(this, &sink, buffer_h, off_x, off_y);
    gui_geometry_run_sink_flush(&sink);
    gui_free(scratch);

    *out_runs = sink.run_count;
    return true;
}

/**
 * Point pixel_buffer at a payload matching the current parameters.
 *
 * @param store           Layout chosen by the caller for this arc.
 * @param out_need_render Set true when the payload still has to be rasterised.
 */
static bool init_arc_buffer(gui_arc_t *this, arc_store_t store, bool *out_need_render)
{
    // Calculate required buffer size
    int buffer_w, buffer_h, off_x, off_y;
    gui_arc_ink_box(this, &off_x, &off_y, &buffer_w, &buffer_h);

    bool is_a8 = (store != ARC_STORE_ARGB8888);
    uint32_t required_size;
    uint32_t runs = 0u;

    if (store == ARC_STORE_SPARSE)
    {
        if (!arc_sparse_run_count(this, buffer_w, buffer_h, off_x, off_y, &runs))
        {
            return false;
        }

        uint64_t bytes = gui_geometry_span_bytes(runs, (uint16_t)buffer_h);
        if (bytes == 0u || bytes > UINT32_MAX)
        {
            return false;
        }
        required_size = (uint32_t)bytes;
    }
    else if (!gui_geometry_image_buffer_size(buffer_w, buffer_h, is_a8 ? 1u : 4u,
                                             &required_size))
    {
        return false;
    }

    arc_desc_t desc;
    arc_desc_init(&desc, this, buffer_w, buffer_h, off_x, off_y, store);
    uint16_t desc_len = arc_desc_len(&desc);

    const uint8_t *reuse = (this->store == store) ? this->pixel_buffer : NULL;

    if (GUI_ARC_ENABLE_INCREMENTAL && this->end_angle_dirty &&
        store == ARC_STORE_A8 && this->store == ARC_STORE_A8 &&
        reuse != NULL && this->draw_img != NULL &&
        buffer_w == this->buffer_w && buffer_h == this->buffer_h &&
        off_x == this->buffer_off_x && off_y == this->buffer_off_y)
    {
        uint8_t *hit = gui_geometry_cache_find(&desc, desc_len, required_size);
        bool updated = false;
        if (hit != NULL)
        {
            gui_geometry_cache_release(reuse);
            this->pixel_buffer = hit;
            *out_need_render = false;
            updated = true;
        }
        else if (gui_geometry_cache_rekey(reuse, &desc, desc_len, required_size))
        {
            DrawContext ctx;
            init_draw_context(&ctx, this->pixel_buffer + sizeof(gui_rgb_data_head_t),
                              buffer_w, buffer_h, PIXEL_FORMAT_A8);
            ctx.clip_rect = (Rect) {0, 0, (int16_t)buffer_w, (int16_t)buffer_h};
            ctx.overlap_keeps_max_alpha = true;
            *out_need_render = !lg_arc_update_a8(&ctx,
                                                 GUI_BASE(this)->w / 2.0f - off_x,
                                                 GUI_BASE(this)->h / 2.0f - off_y,
                                                 this->radius, this->line_width,
                                                 this->start_angle, this->rendered_end_angle,
                                                 this->end_angle, this->color.color.argb_full);
            gui_get_dc()->cache_need_clean = 1;
            updated = true;
        }
        if (updated)
        {
            gui_geometry_img_bind(this->draw_img, this->pixel_buffer, this->opacity_value,
                                  true, this->color);
            this->draw_img->high_quality = 0;
            return true;
        }
    }

    uint8_t *buffer =
        gui_geometry_cache_refresh_resizable(reuse, &desc, desc_len,
                                             required_size, out_need_render);
    if (buffer == NULL) { return false; }
    if (reuse == NULL && this->pixel_buffer != NULL)
    {
        gui_geometry_cache_release(this->pixel_buffer);
    }
    this->pixel_buffer = buffer;
    this->store = store;
    this->buffer_runs = runs;

    // Save buffer dimensions
    this->buffer_w = buffer_w;
    this->buffer_h = buffer_h;
    this->buffer_off_x = off_x;
    this->buffer_off_y = off_y;

    /* A sparse payload is drawn straight to the frame buffer by
     * gui_geometry_span_draw(), which positions it from the matrix rather than
     * from a draw_img, so there is nothing to bind. */
    if (store == ARC_STORE_SPARSE)
    {
        if (this->draw_img != NULL)
        {
            /* data aliases pixel_buffer, released above or replaced below. */
            gui_free(this->draw_img);
            this->draw_img = NULL;
        }
        return true;
    }

    // Allocate draw_img if needed
    if (this->draw_img == NULL)
    {
        this->draw_img = gui_malloc(sizeof(draw_img_t));
        if (this->draw_img == NULL)
        {
            return false;
        }
        memset(this->draw_img, 0x00, sizeof(draw_img_t));
    }

    // Setup draw_img
    gui_geometry_img_bind(this->draw_img, this->pixel_buffer, this->opacity_value,
                          is_a8, this->color);
    /* The bind turns filtered sampling on, as rect/circle want.  Arc has always
     * drawn unfiltered; keep it so, so that switching the storage format is the
     * only thing this change does.  Filtering is a separate decision, to be
     * weighed on its own for rotated and scaled arcs. */
    this->draw_img->high_quality = 0;

    return true;
}

/**
 * Rasterise the ring straight into the run-encoded payload.
 *
 * The coverage comes from arc_raster_ring(), the same function the contiguous
 * mask uses, so the two layouts cannot drift apart.  What differs is only the
 * destination: a covered run is written once, and the zeros that make up most
 * of a ring's bounding box are not written at all.
 *
 * @return false if the scratch row could not be had.  The payload is discarded
 *         rather than left half-written -- see the failure paths below.
 */
static bool render_arc_sparse(gui_arc_t *this)
{
    gui_geometry_span_t *spans = (gui_geometry_span_t *)this->pixel_buffer;

    /* Take the scratch before touching the payload.  A payload from the cache
     * carries whatever header the node last used, and that header is what says
     * where the row table ends: overwriting it and then failing would leave the
     * node describing a shape nobody wrote. */
    uint8_t *scratch = gui_malloc((size_t)this->buffer_w);
    if (scratch == NULL)
    {
        return false;
    }

    /* From here the payload is this arc's.  The row table is addressed through
     * spans->height, so the header has to be in place before either pointer is
     * derived -- and the runs follow the rows, so the two cannot be carved out
     * of a stale height. */
    spans->width = (uint16_t)this->buffer_w;
    spans->height = (uint16_t)this->buffer_h;
    spans->run_count = 0u;

    gui_geometry_span_row_t *rows = gui_geometry_span_rows(spans);
    gui_geometry_span_run_t *runs = gui_geometry_span_runs(spans);

    /* The payload was acquired for exactly this many runs, since the count and
     * this pass share the rasteriser. */
    gui_geometry_run_sink_t sink;
    gui_geometry_run_sink_init(&sink, scratch, (uint16_t)this->buffer_w,
                               (uint16_t)this->buffer_h, rows, runs, this->buffer_runs);

    arc_raster_ring(this, &sink, this->buffer_h, this->buffer_off_x, this->buffer_off_y);
    gui_geometry_run_sink_flush(&sink);
    gui_free(scratch);

    /* Count and raster are one function, so these agree by construction; an
     * overflow would mean a truncated run store. */
    if (sink.overflow || sink.run_count > this->buffer_runs)
    {
        return false;
    }

    spans->run_count = sink.run_count;

    return true;
}

/**
 * Render arc to buffer - called only when parameters change.
 *
 * @return true if the payload holds this arc's coverage.
 */
static bool render_arc_to_buffer(gui_arc_t *this)
{
    // Use the saved buffer dimensions
    int buffer_w = this->buffer_w;
    int buffer_h = this->buffer_h;

    if (this->store == ARC_STORE_SPARSE)
    {
        return render_arc_sparse(this);
    }

    /* Same predicate init_arc_buffer() sized the allocation with: this runs
     * right after it, with no setter in between, so the two always agree.  A
     * fresh render would overwrite the header anyway. */
    bool is_a8 = (this->store == ARC_STORE_A8);
    uint32_t pixel_bytes = is_a8 ? 1u : 4u;

    // Setup image header
    gui_rgb_data_head_t *img_header = (gui_rgb_data_head_t *)this->pixel_buffer;
    gui_geometry_data_head_set(img_header, (int16_t)buffer_w, (int16_t)buffer_h,
                               is_a8 ? A8 : ARGB8888);

    // Clear pixel data
    uint8_t *pixel_data = this->pixel_buffer + sizeof(gui_rgb_data_head_t);
    memset(pixel_data, 0x00, (size_t)((uint32_t)(buffer_w * buffer_h) * pixel_bytes));

    DrawContext draw_ctx;
    init_draw_context(&draw_ctx, pixel_data, buffer_w, buffer_h,
                      is_a8 ? PIXEL_FORMAT_A8 : PIXEL_FORMAT_ARGB8888);
    draw_ctx.enable_aa = true;
    draw_ctx.clip_rect.x = 0;
    draw_ctx.clip_rect.y = 0;
    draw_ctx.clip_rect.w = buffer_w;
    draw_ctx.clip_rect.h = buffer_h;
    draw_ctx.gradient = this->gradient;
    /* This buffer is this arc's own, so a pixel its spans, AA edges and caps
     * cover more than once is one shape's overlap and takes the larger alpha.
     * That is also what the A8 mask does, keeping the two formats identical.
     * gui_arc_group draws several arcs into one buffer and leaves the flag
     * clear, so separate arcs still composite. */
    draw_ctx.overlap_keeps_max_alpha = true;

    // The buffer covers only the inked sub-rect of the widget box, so the arc
    // centre sits at the widget-local centre minus that sub-rect's origin.
    gui_obj_t *obj = GUI_BASE(this);
    float center_x = obj->w / 2.0f - (float)this->buffer_off_x;
    float center_y = obj->h / 2.0f - (float)this->buffer_off_y;

    // Draw arc using gradient or solid color
    if (this->use_gradient && this->gradient != NULL)
    {
        draw_arc_df_aa_gradient(&draw_ctx,
                                center_x,
                                center_y,
                                this->radius,
                                this->line_width,
                                this->start_angle,
                                this->end_angle,
                                this->gradient);
    }
    else
    {
        // Draw arc using optimized SDF-based algorithm
        draw_arc_df_aa(&draw_ctx,
                       center_x,
                       center_y,
                       this->radius,
                       this->line_width,
                       this->start_angle,
                       this->end_angle,
                       this->color.color.argb_full);
    }
    gui_get_dc()->cache_need_clean = 1;
    return true;
}

/**
 * Re-fit the widget box after a geometry change, keeping the visual centre put.
 *
 * radius and line_width both feed the box size, so changing either without
 * re-deriving the box leaves the centre offset stale and the arc drifts.
 */
static void gui_arc_refit_box(gui_arc_t *arc)
{
    gui_obj_t *obj = GUI_BASE(arc);

    /* Centre in parent coordinates, recovered from the box we have now. */
    int cx = obj->x + arc->x;
    int cy = obj->y + arc->y;
    int box = gui_arc_box_size(arc->radius, arc->line_width);

    obj->x = cx - box / 2;
    obj->y = cy - box / 2;
    obj->w = box;
    obj->h = box;
    arc->x = box / 2;
    arc->y = box / 2;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_arc_t *gui_arc_create(void *parent, const char *name, int x, int y, int radius,
                          float start_angle, float end_angle,
                          float line_width, gui_color_t color)
{
    GUI_ASSERT(parent != NULL);
    gui_arc_t *arc = gui_malloc(sizeof(gui_arc_t));
    GUI_ASSERT(arc != NULL);
    memset(arc, 0x00, sizeof(gui_arc_t));

    /* Colour alpha is baked into the ARGB payload.  Keep widget opacity as an
     * independent multiplier so alpha is applied exactly once by default. */
    arc->opacity_value = UINT8_MAX;
    arc->draw_img = NULL;
    arc->pixel_buffer = NULL;
    arc->buffer_valid = false;

    // Initialize gradient support
    arc->gradient = NULL;
    arc->use_gradient = false;

    // Calculate widget bounding box
    int box_size = gui_arc_box_size(radius, line_width);
    int box_x = x - box_size / 2;  // Widget position (top-left corner relative to parent)
    int box_y = y - box_size / 2;

    // Set arc parameters
    // arc->x, arc->y store the center position relative to the bounding box
    arc->x = box_size / 2;  // Center X relative to widget's bounding box
    arc->y = box_size / 2;  // Center Y relative to widget's bounding box
    arc->radius = radius;
    arc->start_angle = start_angle;
    arc->end_angle = end_angle;
    arc->line_width = line_width;
    arc->color = color;

    gui_obj_ctor((gui_obj_t *)arc, parent, name, box_x, box_y, box_size, box_size);
    GET_BASE(arc)->obj_cb = gui_arc_cb;
    GET_BASE(arc)->has_input_process_cb = true;
    GET_BASE(arc)->has_pre_process_cb = true;
    GET_BASE(arc)->has_process_cb = true;
    GET_BASE(arc)->has_post_process_cb = true;
    GET_BASE(arc)->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(arc)->child_list));
    if ((GET_BASE(arc)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(arc)->parent)->child_list),
                               &(GET_BASE(arc)->brother_list));
    }
    GET_BASE(arc)->create_done = true;

    // Initialize transformation parameters
    arc->degrees = 0.0f;
    arc->scale_x = 1.0f;
    arc->scale_y = 1.0f;
    arc->offset_x = 0.0f;
    arc->offset_y = 0.0f;

    return arc;
}


void gui_arc_set_position(gui_arc_t *arc, int x, int y)
{
    GUI_ASSERT(arc != NULL);

    // Calculate widget bounding box size
    int box_size = gui_arc_box_size(arc->radius, arc->line_width);

    // Update widget position (top-left corner relative to parent)
    arc->base.x = x - box_size / 2;
    arc->base.y = y - box_size / 2;

    arc->base.w = box_size;
    arc->base.h = box_size;
    arc->x = box_size / 2;
    arc->y = box_size / 2;
}

void gui_arc_set_radius(gui_arc_t *arc, int radius)
{
    GUI_ASSERT(arc != NULL);
    arc->radius = radius;
    gui_arc_refit_box(arc);
    arc->buffer_valid = false;
    arc->end_angle_dirty = false;
}
void gui_arc_set_opacity(gui_arc_t *arc, uint8_t opacity)
{
    GUI_ASSERT(arc != NULL);
    arc->opacity_value = opacity;
}
void gui_arc_set_color(gui_arc_t *arc, gui_color_t color)
{
    GUI_ASSERT(arc != NULL);
    arc->color = color;
    arc->buffer_valid = false;
    arc->end_angle_dirty = false;
}

void gui_arc_set_start_angle(gui_arc_t *arc, float start_angle)
{
    GUI_ASSERT(arc != NULL);
    arc->start_angle = start_angle;
    arc->buffer_valid = false;
    arc->end_angle_dirty = false;
}

void gui_arc_set_end_angle(gui_arc_t *arc, float end_angle)
{
    GUI_ASSERT(arc != NULL);
    if (arc->end_angle == end_angle) { return; }
    if (arc->buffer_valid)
    {
        arc->end_angle_dirty = true;
    }
    arc->end_angle = end_angle;
    arc->buffer_valid = false;
}

void gui_arc_set_line_width(gui_arc_t *arc, float line_width)
{
    GUI_ASSERT(arc != NULL);
    arc->line_width = line_width;
    gui_arc_refit_box(arc);
    arc->buffer_valid = false;
    arc->end_angle_dirty = false;
}

void gui_arc_on_click(gui_arc_t *arc, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)arc, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}

void gui_arc_rotate(gui_arc_t *arc, float degrees)
{
    GUI_ASSERT(arc != NULL);
    arc->degrees = degrees;
}

void gui_arc_scale(gui_arc_t *arc, float scale_x, float scale_y)
{
    GUI_ASSERT(arc != NULL);
    arc->scale_x = scale_x;
    arc->scale_y = scale_y;
}

void gui_arc_translate(gui_arc_t *arc, float tx, float ty)
{
    GUI_ASSERT(arc != NULL);
    arc->offset_x = tx;
    arc->offset_y = ty;
}

void gui_arc_set_angular_gradient(gui_arc_t *arc, float start_angle, float end_angle)
{
    GUI_ASSERT(arc != NULL);

    // Allocate gradient if not exists
    if (arc->gradient == NULL)
    {
        arc->gradient = gui_malloc(sizeof(Gradient));
        GUI_ASSERT(arc->gradient != NULL);
    }

    // REMOVED: Auto-compensation for full circle gradient
    // The draw_arc_df_aa_gradient function now handles full circles efficiently
    // without needing the +1 degree trick for end caps

    // Initialize angular gradient
    gradient_init(arc->gradient, GRADIENT_ANGULAR);
    arc->gradient->angular_cx = arc->x;
    arc->gradient->angular_cy = arc->y;
    arc->gradient->angular_start = start_angle;
    arc->gradient->angular_end = end_angle;

    arc->use_gradient = true;
    arc->buffer_valid = false;
    arc->end_angle_dirty = false;
}

void gui_arc_add_gradient_stop(gui_arc_t *arc, float position, gui_color_t color)
{
    GUI_ASSERT(arc != NULL);

    if (arc->gradient == NULL)
    {
        // Initialize default angular gradient if not set
        gui_arc_set_angular_gradient(arc, arc->start_angle, arc->end_angle);
    }

    gradient_add_stop(arc->gradient, position, color.color.argb_full);
    arc->buffer_valid = false;
    arc->end_angle_dirty = false;
}

void gui_arc_clear_gradient(gui_arc_t *arc)
{
    GUI_ASSERT(arc != NULL);

    if (arc->gradient != NULL)
    {
        gui_free(arc->gradient);
        arc->gradient = NULL;
    }

    arc->use_gradient = false;
    arc->buffer_valid = false;
    arc->end_angle_dirty = false;
}
