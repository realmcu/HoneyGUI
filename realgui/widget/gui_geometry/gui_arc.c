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
 * An arc is a stroke with one colour, so what the mask holds is exactly the
 * anti-aliased coverage the rasteriser already computes.  A8 costs a quarter of
 * the memory, drops colour from the cache key so arcs differing only in colour
 * share one payload, and reaches acc_sw's dedicated A8 blit instead of the
 * generic rasteriser -- the same trade gui_circle and gui_rect make.
 *
 * A gradient arc keeps ARGB8888: the colour varies across the stroke, so there
 * is no single mask to share.
 *
 * Set to 0 when bringing up a hardware accelerator whose blit may not handle A8.
 * An unsupported source format usually shows up as a garbled or missing shape
 * rather than a clean fallback, so flipping this is the quickest way to rule it
 * out.
 *
 * There is no `scons GUI_ARC_ENABLE_A8=0`: the SConstruct does not forward
 * command-line assignments to the compiler, so the value has to be changed
 * here.  The arc example prints the payload size of each arc it allocates, read
 * back from the buffer header, so a before/after comparison needs only this one
 * line.
 */
#ifndef GUI_ARC_ENABLE_A8
#define GUI_ARC_ENABLE_A8 1
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
    uint32_t color;             /**< ARGB baked into the pixels; alpha only when A8. */
    uint32_t is_a8;             /**< Non-zero when the payload is a coverage mask. */
    uint32_t use_gradient;      /**< Non-zero when the gradient below applies. */
    Gradient gradient;          /**< Only present when use_gradient is set. */
} arc_desc_t;

/* Exactly at the limit today (160 bytes), so any new field must come out of
 * Gradient or raise GUI_GEOMETRY_DESC_MAX -- this turns the overflow into a
 * build error instead of a runtime assert in the cache. */
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
static bool init_arc_buffer(gui_arc_t *this, bool *out_need_render);
static void render_arc_to_buffer(gui_arc_t *this);

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

    // Apply transformations (like gui_img_prepare does)
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
    /* Every setter that changes a pixel clears buffer_valid, so this flag alone
     * decides whether the payload has to be re-acquired and redrawn. */
    if (!this->buffer_valid)
    {
        bool need_render = false;

        if (!init_arc_buffer(this, &need_render))
        {
            gui_log("[GUI MODULE]Arc prepare: init_arc_buffer failed!\n");
            return;
        }
        if (need_render)
        {
            render_arc_to_buffer(this);
        }
        this->buffer_valid = true;
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
    if (this->draw_img != NULL && this->pixel_buffer != NULL)
    {
        gui_obj_t *obj = (gui_obj_t *)this;

        // The arc is rendered at the center of the buffer
        // We just need to position the buffer at the widget's location
        // No offset needed since arc->x, arc->y are already relative to the bounding box

        // Update opacity value to consider parent's opacity (like gui_img does)
        this->draw_img->opacity_value = obj->parent->opacity_value * this->opacity_value / UINT8_MAX;

        // Initialize with identity matrix or copy user's matrix
        if (obj->matrix != NULL)
        {
            memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
        }
        else
        {
            matrix_identity(&this->draw_img->matrix);
        }

        // Place the (possibly clipped) buffer back at its origin inside the box.
        matrix_translate((float)this->buffer_off_x, (float)this->buffer_off_y,
                         &this->draw_img->matrix);

        memcpy(&this->draw_img->inverse, &this->draw_img->matrix, sizeof(struct gui_matrix));
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

static void gui_arc_destroy(gui_arc_t *this)
{
    /* The payload is shared, so drop a reference rather than freeing it; other
     * arcs with the same geometry may still be drawing from it. */
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

    if (this->gradient != NULL)
    {
        gui_free(this->gradient);
        this->gradient = NULL;
    }

    this->buffer_valid = false;
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
                          bool is_a8)
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

    /* RGB is deliberately left out of an A8 key -- that is what lets arcs
     * differing only in colour share one mask.  Alpha stays in, because it is
     * folded into the mask values themselves. */
    if (is_a8)
    {
        desc->is_a8 = 1u;
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
 * Point pixel_buffer at a payload matching the current parameters.
 *
 * @param out_need_render Set true when the payload still has to be rasterised.
 */
static bool init_arc_buffer(gui_arc_t *this, bool *out_need_render)
{
    // Calculate required buffer size
    int buffer_w, buffer_h, off_x, off_y;
    gui_arc_ink_box(this, &off_x, &off_y, &buffer_w, &buffer_h);

    bool is_a8 = arc_can_use_a8(this);
    uint32_t required_size;
    if (!gui_geometry_image_buffer_size(buffer_w, buffer_h, is_a8 ? 1u : 4u, &required_size))
    {
        return false;
    }

    arc_desc_t desc;
    arc_desc_init(&desc, this, buffer_w, buffer_h, off_x, off_y, is_a8);
    uint16_t desc_len = arc_desc_len(&desc);

    /* Crossing between A8 and ARGB8888 (a gradient set or cleared) must not
     * re-key the old payload in place: going to A8 it would keep the 4x
     * allocation for good, and the saving would never reach the heap.  Acquire
     * a right-sized payload first and drop the old one after, so a failed
     * allocation still leaves the previous arc to draw. */
    const uint8_t *reuse = this->pixel_buffer;
    if (reuse != NULL &&
        (((const gui_rgb_data_head_t *)reuse)->type == (char)A8) != is_a8)
    {
        reuse = NULL;
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

    // Save buffer dimensions
    this->buffer_w = buffer_w;
    this->buffer_h = buffer_h;
    this->buffer_off_x = off_x;
    this->buffer_off_y = off_y;

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

/** Render arc to buffer - called only when parameters change */
static void render_arc_to_buffer(gui_arc_t *this)
{
    // Use the saved buffer dimensions
    int buffer_w = this->buffer_w;
    int buffer_h = this->buffer_h;

    /* Same predicate init_arc_buffer() sized the allocation with: this runs
     * right after it, with no setter in between, so the two always agree.  A
     * fresh render would overwrite the header anyway. */
    bool is_a8 = arc_can_use_a8(this);
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
}

void gui_arc_set_start_angle(gui_arc_t *arc, float start_angle)
{
    GUI_ASSERT(arc != NULL);
    arc->start_angle = start_angle;
    arc->buffer_valid = false;
}

void gui_arc_set_end_angle(gui_arc_t *arc, float end_angle)
{
    GUI_ASSERT(arc != NULL);
    arc->end_angle = end_angle;
    arc->buffer_valid = false;
}

void gui_arc_set_line_width(gui_arc_t *arc, float line_width)
{
    GUI_ASSERT(arc != NULL);
    arc->line_width = line_width;
    gui_arc_refit_box(arc);
    arc->buffer_valid = false;
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
}
