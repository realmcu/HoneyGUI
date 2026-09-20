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
#include <stddef.h>
#include "guidef.h"
#include "gui_fb.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "gui_circle.h"
#include "gui_geometry_cache.h"
#include "gui_geometry_common.h"
#include "gui_geometry_image.h"

/*============================================================================*
 *                           Configuration
 *============================================================================*/
// 1 = Enable Dither (Better gradients), 0 = Disable (Max speed)
#define GUI_CIRCLE_ENABLE_DITHER 1

/**
 * 1 = store solid circles as A8 coverage masks, 0 = always ARGB8888.
 *
 * A8 costs a quarter of the memory, drops colour from the cache key so circles
 * differing only in colour share one payload, and reaches acc_sw's dedicated A8
 * blit instead of the generic rasteriser.
 *
 * Set to 0 when bringing up a hardware accelerator whose blit may not handle A8.
 * An unsupported source format usually shows up as a garbled or missing shape
 * rather than a clean fallback, so flipping this is the quickest way to rule it
 * out.  The same switch isolates what A8 alone contributes to frame time -- the
 * shape cache stays active either way.
 *
 * Overridable from the build, so a bring-up run needs no source edit:
 *   scons GUI_CIRCLE_ENABLE_A8=0
 */
#ifndef GUI_CIRCLE_ENABLE_A8
#define GUI_CIRCLE_ENABLE_A8 1
#endif

/*============================================================================*
 *                           Types
 *============================================================================*/

GUI_GEOMETRY_DESC_SIZE_CHECK(circle_desc_t);

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/** Descriptor bytes to compare: solid parts stop before the gradient. */
static uint16_t circle_desc_len(const circle_desc_t *desc)
{
    if (desc->gradient_type == UINT32_MAX)
    {
        return (uint16_t)offsetof(circle_desc_t, gradient);
    }
    return (uint16_t)sizeof(circle_desc_t);
}

/**
 * Whether this circle can be stored as an A8 coverage mask.
 *
 * A mask carries no RGB, so one payload serves every colour of the same shape --
 * which is the point, since "same size, different colour" is otherwise a full
 * cache miss.  Only a gradient rules it out, since that genuinely needs per-pixel
 * RGB.
 *
 * A translucent colour is fine: its alpha is folded into the mask values, not
 * carried in fg_color_set.  That matters because the two blit paths disagree
 * about fg_color_set's alpha -- acc_sw_generic multiplies it in, a8_2_rgb565
 * discards it -- so it is pinned at 255 and the mask carries everything.
 */
static bool circle_has_gradient(const gui_circle_t *circle)
{
    return circle->use_gradient && circle->gradient != NULL &&
           circle->gradient->stop_count >= 2;
}

/**
 * Fill in a descriptor for one part of this circle.
 *
 * Zeroes first: gui_geometry_cache_acquire() compares descriptors byte for byte, so
 * a padding hole left uninitialised would make two identical circles miss.
 */
static void circle_desc_init(circle_desc_t *desc, gui_circle_t *this,
                             gui_circle_part_t part, int size_a, int size_b)
{
    memset(desc, 0x00, sizeof(*desc));

    desc->part = (uint32_t)part;
    desc->size_a = size_a;
    desc->size_b = size_b;
    desc->gradient_type = UINT32_MAX;
    desc->stroke_width = (part == CIRCLE_PART_FULL || part == CIRCLE_PART_STROKE) ?
                         this->stroke_width : 0.0f;

    /* RGB is deliberately left out of an A8 key -- that is what lets circles
     * differing only in colour share one mask.  Alpha stays in, because it is
     * folded into the mask values themselves. */
    bool is_stroke = (part == CIRCLE_PART_STROKE);
    bool is_a8 = gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8,
                                     circle_has_gradient(this), is_stroke);
    gui_color_t payload_color = is_stroke ? this->stroke_color : this->color;

    if (is_a8)
    {
        desc->is_a8 = 1u;
        desc->color = payload_color.color.rgba.a;
    }
    else
    {
        desc->color = payload_color.color.argb_full;
    }

    /* Only the full-circle part reads the gradient; the strip and the inscribed
     * square are always solid. */
    if (part == CIRCLE_PART_FULL && circle_has_gradient(this))
    {
        desc->gradient_type = (uint32_t)this->gradient_type;
        memcpy(&desc->gradient, this->gradient, sizeof(Gradient));
    }
}

/**
 * Free the whole arc buffer set used by multi-part rendering.
 *
 * All four hold a reference to the same strip payload, so all four release the
 * same way and the pixels go when the last one does.
 */
static void gui_circle_release_arc_images(gui_circle_t *this)
{
    gui_geometry_draw_img_release(&this->arc_right);
    gui_geometry_draw_img_release(&this->arc_top);
    gui_geometry_draw_img_release(&this->arc_bottom);

    gui_geometry_draw_img_release(&this->arc_left);
}

static void gui_circle_release_images(gui_circle_t *circle)
{
    gui_circle_release_arc_images(circle);
    gui_geometry_draw_img_release(&circle->center_rect);
    gui_geometry_draw_img_release(&circle->stroke_img);
    gui_geometry_span_release(circle->stroke_span);
    circle->stroke_span = NULL;
}

static void update_circle_gradient_geometry(gui_circle_t *circle)
{
    if (circle->gradient == NULL)
    {
        return;
    }

    if (circle->gradient_type == CIRCLE_GRADIENT_RADIAL)
    {
        circle->gradient->radial_cx = (float)circle->radius;
        circle->gradient->radial_cy = (float)circle->radius;
        circle->gradient->radial_r = (float)circle->radius;
    }
    else
    {
        circle->gradient->angular_cx = (float)circle->radius;
        circle->gradient->angular_cy = (float)circle->radius;
    }
}

/** Check if a point is inside the circle's bounding circle */
static bool is_point_in_circle(gui_circle_t *circle, int x, int y)
{
    int64_t dx = (int64_t)x - circle->x;
    int64_t dy = (int64_t)y - circle->y;
    int64_t distance_sq = dx * dx + dy * dy;
    int64_t radius_sq = (int64_t)circle->radius * circle->radius;

    return (distance_sq <= radius_sq);
}

static void gui_circle_input_process(gui_obj_t *obj)
{
    gui_circle_t *this = (gui_circle_t *)obj;
    touch_info_t *tp = tp_get_info();
    int absolute_x = 0;
    int absolute_y = 0;

    /* Touch coordinates are absolute while obj->x/y are parent relative, so the
     * widget origin has to be accumulated up the tree. */
    gui_obj_absolute_xy(obj, &absolute_x, &absolute_y);

    if (!is_point_in_circle(this, tp->x - absolute_x, tp->y - absolute_y))
    {
        return;
    }

    /* No extra touch-state condition here: gui_obj_enable_event() checks the
     * state that belongs to each event code (TOUCH_SHORT for CLICKED,
     * tp->pressed for PRESSED, tp->released for RELEASED, ...). Gating all of
     * them on TOUCH_SHORT made everything except CLICKED unreachable, because
     * at press time the type is still TOUCH_INIT/TOUCH_HOLD_* and TOUCH_SHORT
     * only appears once a short tap is released. */
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, "touch");
    /* PRESSING repeats every frame while the touch is held, which is what a
     * drag needs. tp->x/tp->y stay at the press origin for the whole press, so
     * the hit test above means "the press started on the circle". */
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING, "touch");
}

static bool circle_row_bounds(float center, float inner_sq, float outer_sq,
                              float dy_sq, int diameter,
                              int *outer_left, int *outer_right,
                              int *solid_left, int *solid_right)
{
    if (dy_sq >= outer_sq)
    {
        return false;
    }

    float outer_half = sqrtf(outer_sq - dy_sq);
    int left = (int)ceilf(center - outer_half - 0.5f);
    int right = (int)floorf(center + outer_half - 0.5f);

    *outer_left = LG_MAX(left, 0);
    *outer_right = LG_MIN(right, diameter - 1);
    *solid_left = 1;
    *solid_right = 0;

    if (dy_sq < inner_sq)
    {
        float inner_half = sqrtf(inner_sq - dy_sq);

        left = (int)ceilf(center - inner_half - 0.5f);
        right = (int)floorf(center + inner_half - 0.5f);
        *solid_left = LG_MAX(left, 0);
        *solid_right = LG_MIN(right, diameter - 1);
    }

    return *outer_left <= *outer_right;
}

/**
 * Rasterise a circle coverage mask, one byte per pixel.
 *
 * Same quadrant mirroring and same coverage maths as the ARGB8888 path, minus
 * the dithering a mask cannot carry.  The colour's alpha is folded in with the
 * same arithmetic that path uses, so the two agree byte for byte: interior
 * pixels take colour alpha unchanged, and only edge pixels scale it by coverage.
 */
static void rasterize_circle_mask_a8(gui_circle_t *this, uint8_t *buffer, uint32_t buffer_size)
{
    int r = this->radius;
    int diameter = r * 2;
    uint8_t color_a = this->color.color.rgba.a;
    float fill_radius = LG_MAX((float)r - this->stroke_width, 0.0f);

    memset(buffer, 0x00, buffer_size);
    gui_geometry_data_head_set((gui_rgb_data_head_t *)buffer, (int16_t)diameter,
                               (int16_t)diameter, A8);

    if (fill_radius <= 0.0f) { return; }

    uint8_t *mask = buffer + sizeof(gui_rgb_data_head_t);

    float center = (float)r;
    float r_in = LG_MAX(fill_radius - 0.5f, 0.0f);
    float r_out = fill_radius + 0.5f;
    float r_in_sq = r_in * r_in;
    float r_out_sq = r_out * r_out;

    /*
     * Each row has a long, fully covered center span. Fill it at once and
     * only evaluate the narrow anti-aliased bands on either side.
     */
    for (int y = 0; y < r; y++)
    {
        uint8_t *row_top = mask + y * diameter;
        uint8_t *row_bottom = mask + (diameter - 1 - y) * diameter;
        float dy = y + 0.5f - center;
        float dy_sq = dy * dy;
        int outer_left;
        int outer_right;
        int solid_left;
        int solid_right;

        if (!circle_row_bounds(center, r_in_sq, r_out_sq, dy_sq, diameter,
                               &outer_left, &outer_right,
                               &solid_left, &solid_right))
        {
            continue;
        }

        if (solid_left <= solid_right)
        {
            size_t solid_len = (size_t)(solid_right - solid_left + 1);

            memset(row_top + solid_left, color_a, solid_len);
            memset(row_bottom + solid_left, color_a, solid_len);
        }

        for (int x = outer_left; x <= outer_right; x++)
        {
            if (x >= solid_left && x <= solid_right)
            {
                continue;
            }

            float dx = x + 0.5f - center;
            float dist_sq = dx * dx + dy_sq;
            uint8_t edge = (uint8_t)((r_out - sqrtf(dist_sq)) * 255.0f);
            uint8_t coverage = (uint8_t)(((uint32_t)edge * color_a) >> 8);

            row_top[x] = coverage;
            row_bottom[x] = coverage;
        }
    }
}

/** Rasterise a complete circle into a payload, with Symmetry Optimization */
static bool rasterize_circle_payload(gui_circle_t *this, uint8_t *buffer, uint32_t buffer_size)
{
    int r = this->radius;
    int diameter = r * 2; // Keep diameter even for perfect symmetry logic

    memset(buffer, 0x00, buffer_size);

    // Set header
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)buffer;
    gui_geometry_data_head_set(head, (int16_t)diameter, (int16_t)diameter,
                               ARGB8888);

    uint32_t *pixels = (uint32_t *)(buffer + sizeof(gui_rgb_data_head_t));
    uint32_t solid_color = this->color.color.argb_full;
    float fill_radius = LG_MAX((float)r - this->stroke_width, 0.0f);

    if (fill_radius <= 0.0f) { return true; }

    // --- Pre-compute Gradient LUT ---
    uint32_t *gradient_lut = NULL;
    int lut_size = 0;
    bool use_radial = (this->use_gradient && this->gradient != NULL &&
                       this->gradient->stop_count >= 2 &&
                       this->gradient_type == CIRCLE_GRADIENT_RADIAL);
    bool use_angular = (this->use_gradient && this->gradient != NULL &&
                        this->gradient->stop_count >= 2 &&
                        this->gradient_type == CIRCLE_GRADIENT_ANGULAR);

    if (use_radial)
    {
        lut_size = r + 1;
        gradient_lut = gui_malloc((size_t)lut_size * sizeof(uint32_t));
        if (gradient_lut == NULL)
        {
            return false;
        }
        for (int i = 0; i < lut_size; i++)
        {
            float t = (float)i / (float)r;
            gradient_lut[i] = gradient_get_color(this->gradient, t);
        }
    }
    else if (use_angular)
    {
        lut_size = 360;
        gradient_lut = gui_malloc((size_t)lut_size * sizeof(uint32_t));
        if (gradient_lut == NULL)
        {
            return false;
        }

        float start_angle = this->gradient->angular_start;
        float end_angle = this->gradient->angular_end;
        float angle_range = end_angle - start_angle;
        if (angle_range <= 0) { angle_range = 360.0f; }

        for (int i = 0; i < lut_size; i++)
        {
            float angle = (float)i;
            float t = (angle - start_angle) / angle_range;
            if (t < 0) { t += 1.0f; } if (t > 1.0f) { t -= 1.0f; }
            t = (t < 0) ? 0 : ((t > 1.0f) ? 1.0f : t);
            gradient_lut[i] = gradient_get_color(this->gradient, t);
        }
    }

    if (!use_radial && !use_angular)
    {
        float center = (float)r;
        float r_in = LG_MAX(fill_radius - 0.5f, 0.0f);
        float r_out = fill_radius + 0.5f;
        float r_in_sq = r_in * r_in;
        float r_out_sq = r_out * r_out;
        uint32_t rgb = solid_color & 0x00FFFFFFu;
        uint8_t color_alpha = (uint8_t)(solid_color >> 24);

        for (int y = 0; y < diameter; y++)
        {
            float dy = y + 0.5f - center;
            float dy_sq = dy * dy;
            int outer_left;
            int outer_right;
            int solid_left;
            int solid_right;

            if (!circle_row_bounds(center, r_in_sq, r_out_sq, dy_sq, diameter,
                                   &outer_left, &outer_right,
                                   &solid_left, &solid_right))
            {
                continue;
            }

            uint32_t *line = pixels + y * diameter;
            if (solid_left <= solid_right)
            {
                gui_memset32(line + solid_left, solid_color,
                             (uint32_t)(solid_right - solid_left + 1));
            }

            for (int x = outer_left; x <= outer_right; x++)
            {
                if (x >= solid_left && x <= solid_right)
                {
                    continue;
                }

                float dx = x + 0.5f - center;
                float coverage = r_out - sqrtf(dx * dx + dy_sq);
                uint8_t alpha = (uint8_t)((coverage * color_alpha));

                line[x] = rgb | ((uint32_t)alpha << 24);
            }
        }

        return true;
    }

    // --- High Performance Quadrant Rendering ---
    // We iterate only the Top-Left quadrant (0 to r-1)
    // and map the results to the other 3 quadrants.

    float center = (float)r;
    bool dither = GUI_CIRCLE_ENABLE_DITHER && (use_radial || use_angular);

    // Optimization: Calculate r_sq once
    float r_in = LG_MAX(fill_radius - 0.5f, 0.0f);
    float r_out = fill_radius + 0.5f;
    float r_in_sq = r_in * r_in;
    float r_out_sq = r_out * r_out;

    for (int y = 0; y < r; y++)
    {
        // Pointers to the 4 target rows
        uint32_t *line_tl = pixels + y * diameter;                  // Top-Left Row
        uint32_t *line_tr = pixels + y * diameter;                  // Top-Right Row (same row)
        uint32_t *line_bl = pixels + (diameter - 1 - y) * diameter; // Bottom-Left Row
        uint32_t *line_br = pixels + (diameter - 1 - y) * diameter; // Bottom-Right Row (same row)

        for (int x = 0; x < r; x++)
        {
            // Coordinate relative to center (Top-Left quadrant: dx < 0, dy < 0)
            float dx = x + 0.5f - center;
            float dy = y + 0.5f - center;
            float dist_sq = dx * dx + dy * dy;

            // 1. Check bounds (Clipping)
            if (dist_sq >= r_out_sq) { continue; } // Outside circle, skip (pixels are 0 init)

            uint32_t base_color = solid_color;
            float dist = 0.0f; // Calculate only if needed

            // 2. Calculate Color
            if (use_radial && gradient_lut)
            {
                dist = sqrtf(dist_sq); // SQRT #1
                int idx = (int)dist;
                if (idx >= lut_size) { idx = lut_size - 1; }
                base_color = gradient_lut[idx];
            }
            else if (use_angular && gradient_lut)
            {
                // Determine Angle for Top-Left
                float angle = atan2f(dy, dx) * 180.0f / M_PI; // ATAN2 #1
                if (angle < 0) { angle += 360.0f; }
                // Hack: pass angle in color var to avoid struct overhead
                base_color = (uint32_t)angle;
            }

            // 3. Calculate Alpha (Anti-aliasing)
            uint8_t alpha_final = 255;
            if (dist_sq > r_in_sq)
            {
                if (dist == 0.0f) { dist = sqrtf(dist_sq); } // SQRT #2 (Only on edge)
                float alpha_f = r_out - dist;
                alpha_final = (uint8_t)(alpha_f * 255.0f);
            }

            // 4. Fill 4 Quadrants (Mirroring)
            int x_tl = x;
            int x_tr = diameter - 1 - x;
            int y_bottom = (diameter - 1 - y);

            // Function macro to apply alpha and dither
#define APPLY_PIXEL(target_ptr, t_x, t_y, t_color) \
    do { \
        uint32_t c = t_color; \
        if(dither) c = gui_geometry_dither_argb8888(c, t_x, t_y); \
        if(alpha_final < 255) { \
            uint8_t base_a = (c >> 24) & 0xFF; \
            uint8_t new_a = (uint8_t)((alpha_final * base_a) >> 8); \
            c = (c & 0x00FFFFFF) | ((uint32_t)new_a << 24); \
        } \
        target_ptr[t_x] = c; \
    } while(0)

            if (use_angular && gradient_lut)
            {
                // Angular gradient needs special mirroring
                float angle_tl = (float)base_color;

                // TL
                int idx = (int)angle_tl % 360;
                APPLY_PIXEL(line_tl, x_tl, y, gradient_lut[idx]);

                // TR (Mirror X -> 180 - angle + 360)
                float angle_tr = 540.0f - angle_tl;
                idx = (int)angle_tr % 360;
                APPLY_PIXEL(line_tr, x_tr, y, gradient_lut[idx]);

                // BL (Mirror Y -> 360 - angle)
                float angle_bl = 360.0f - angle_tl;
                idx = (int)angle_bl % 360;
                APPLY_PIXEL(line_bl, x_tl, y_bottom, gradient_lut[idx]);

                // BR (Mirror XY -> angle + 180)
                float angle_br = angle_tl + 180.0f;
                idx = (int)angle_br % 360;
                APPLY_PIXEL(line_br, x_tr, y_bottom, gradient_lut[idx]);

            }
            else
            {
                // Radial: Color is identical for all 4 quadrants
                APPLY_PIXEL(line_tl, x_tl, y, base_color);
                APPLY_PIXEL(line_tr, x_tr, y, base_color);
                APPLY_PIXEL(line_bl, x_tl, y_bottom, base_color);
                APPLY_PIXEL(line_br, x_tr, y_bottom, base_color);
            }
#undef APPLY_PIXEL
        }
    }

    if (gradient_lut != NULL)
    {
        gui_free(gradient_lut);
    }
    return true;
}

static void rasterize_circle_stroke(gui_circle_t *this, uint8_t *buffer,
                                    uint32_t buffer_size, bool is_a8)
{
    int radius = this->radius;
    int diameter = radius * 2;

    memset(buffer, 0x00, buffer_size);
    if (is_a8)
    {
        gui_geometry_data_head_set((gui_rgb_data_head_t *)buffer, (int16_t)diameter,
                                   (int16_t)diameter, A8);
    }
    else
    {
        gui_geometry_data_head_set((gui_rgb_data_head_t *)buffer, (int16_t)diameter,
                                   (int16_t)diameter, ARGB8888);
    }

    float stroke_width = LG_CLAMP(this->stroke_width, 0.0f, (float)radius);
    if (radius <= 0 || stroke_width <= 0.0f) { return; }

    uint8_t *mask = buffer + sizeof(gui_rgb_data_head_t);
    uint32_t *pixels = (uint32_t *)mask;
    uint32_t stroke_color = this->stroke_color.color.argb_full;
    uint8_t stroke_alpha = this->stroke_color.color.rgba.a;

    float center = (float)radius;
    float outer_in = (float)radius - 0.5f;
    float outer_out = (float)radius + 0.5f;
    float outer_in_sq = outer_in * outer_in;
    float outer_out_sq = outer_out * outer_out;
    float inner_radius = (float)radius - stroke_width;
    float inner_cut = LG_MAX(inner_radius - 0.5f, 0.0f);
    float inner_cut_sq = inner_cut * inner_cut;

    for (int y = 0; y < radius; y++)
    {
        int y_mirror = diameter - 1 - y;
        for (int x = 0; x < radius; x++)
        {
            float dx = x + 0.5f - center;
            float dy = y + 0.5f - center;
            float dist_sq = dx * dx + dy * dy;

            if (dist_sq >= outer_out_sq ||
                (inner_radius > 0.0f && dist_sq < inner_cut_sq))
            {
                continue;
            }

            float coverage = 1.0f;
            if (dist_sq > outer_in_sq)
            {
                coverage = outer_out - sqrtf(dist_sq);
                coverage = LG_CLAMP(coverage, 0.0f, 1.0f);
            }

            uint8_t alpha = stroke_alpha;
            if (coverage < 0.999f)
            {
                alpha = (uint8_t)(coverage * (float)stroke_alpha);
            }

            int x_mirror = diameter - 1 - x;
            int offsets[4] =
            {
                y *diameter + x,
                y *diameter + x_mirror,
                y_mirror *diameter + x,
                y_mirror *diameter + x_mirror
            };

            if (is_a8)
            {
                for (int i = 0; i < 4; i++) { mask[offsets[i]] = alpha; }
            }
            else
            {
                uint32_t color = (stroke_color & 0x00FFFFFFu) | ((uint32_t)alpha << 24);
                for (int i = 0; i < 4; i++) { pixels[offsets[i]] = color; }
            }
        }
    }
}

/** Create a complete circle in a single buffer, shared with identical circles */
static draw_img_t *create_circle_buffer(gui_circle_t *this, gui_obj_t *obj, draw_img_t **old_img)
{
    // Free old buffer first to prevent memory leak
    gui_geometry_draw_img_release(old_img);

    int diameter = this->radius * 2;
    uint32_t buffer_size;
    uint32_t pixel_bytes = gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8, circle_has_gradient(this),
                                               false) ? 1u : 4u;
    if (this->radius <= 0 ||
        !gui_geometry_image_buffer_size(diameter, diameter, pixel_bytes, &buffer_size))
    {
        return NULL;
    }

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    /* Every circle of the same radius and gradient draws from one payload; only
     * this draw_img_t and its matrix are per widget.  An opaque solid circle
     * stores a colourless mask, so colour does not split the cache either. */
    bool is_a8 = (pixel_bytes == 1u);
    circle_desc_t desc;
    bool is_new = false;
    circle_desc_init(&desc, this, CIRCLE_PART_FULL, this->radius, 0);

    uint8_t *buffer = gui_geometry_cache_acquire(&desc, circle_desc_len(&desc), buffer_size, &is_new);
    if (buffer == NULL)
    {
        gui_free(img);
        return NULL;
    }

    if (is_new)
    {
        if (is_a8)
        {
            rasterize_circle_mask_a8(this, buffer, buffer_size);
        }
        else
        {
            if (!rasterize_circle_payload(this, buffer, buffer_size))
            {
                gui_geometry_cache_release(buffer);
                gui_free(img);
                return NULL;
            }
        }
    }

    gui_geometry_img_bind(img, buffer, this->opacity_value, is_a8, this->color);

    // Apply transformation matrix
    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

static draw_img_t *create_circle_stroke_buffer(gui_circle_t *this, gui_obj_t *obj,
                                               draw_img_t **old_img)
{
    gui_geometry_draw_img_release(old_img);

    if (this->stroke_width <= 0.0f || this->radius <= 0) { return NULL; }

    int diameter = this->radius * 2;
    bool is_a8 = gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8, false, true);
    uint32_t buffer_size;
    if (!gui_geometry_image_buffer_size(diameter, diameter, is_a8 ? 1u : 4u, &buffer_size))
    {
        return NULL;
    }

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    circle_desc_t desc;
    bool is_new = false;
    circle_desc_init(&desc, this, CIRCLE_PART_STROKE, this->radius, 0);

    uint8_t *buffer = gui_geometry_cache_acquire(&desc, circle_desc_len(&desc), buffer_size, &is_new);
    if (buffer == NULL)
    {
        gui_free(img);
        return NULL;
    }

    if (is_new)
    {
        rasterize_circle_stroke(this, buffer, buffer_size, is_a8);
    }

    gui_geometry_img_bind(img, buffer, this->opacity_value, is_a8, this->stroke_color);

    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

/** Set image data header for rectangle */
static void set_rect_header(gui_rgb_data_head_t *head, uint16_t w, uint16_t h, gui_color_t color)
{
    gui_geometry_data_head_set(head, (int16_t)w, (int16_t)h, ARGB8888);

    gui_rect_file_head_t *rect_head = (gui_rect_file_head_t *)head;
    rect_head->color = color;
}

/** Create a solid color image buffer for better anti-aliasing with transformations */
static uint8_t *create_solid_color_buffer_circle(gui_circle_t *this, uint16_t w, uint16_t h,
                                                 gui_color_t color)
{
    bool is_a8 = gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8, circle_has_gradient(this), false);
    circle_desc_t desc;
    bool is_new = false;
    circle_desc_init(&desc, this, CIRCLE_PART_SOLID_RECT, w, h);

    uint32_t pixel_bytes = is_a8 ? 1u : 4u;
    uint32_t buffer_size;
    if (!gui_geometry_image_buffer_size(w, h, pixel_bytes, &buffer_size))
    {
        return NULL;
    }

    uint8_t *buffer = gui_geometry_cache_acquire(&desc, circle_desc_len(&desc), buffer_size, &is_new);
    if (buffer == NULL) { return NULL; }
    if (!is_new) { return buffer; }

    if (is_a8)
    {
        /* Fully covered everywhere, so the mask is just the colour's alpha and
         * every inscribed square of this size and alpha shares it. */
        gui_geometry_data_head_set((gui_rgb_data_head_t *)buffer, (int16_t)w, (int16_t)h, A8);
        memset(buffer + sizeof(gui_rgb_data_head_t), color.color.rgba.a, (size_t)w * h);
        return buffer;
    }

    gui_geometry_data_head_set((gui_rgb_data_head_t *)buffer, (int16_t)w, (int16_t)h, ARGB8888);

    uint32_t *pixels = (uint32_t *)(buffer + sizeof(gui_rgb_data_head_t));
    uint32_t argb = color.color.argb_full;
    uint32_t pixel_count = (uint32_t)w * h;
    for (uint32_t i = 0; i < pixel_count; i++)
    {
        pixels[i] = argb;
    }
    return buffer;
}

/** Create a rectangle image object */
static void set_rect_img(gui_circle_t *this, draw_img_t **input_img, int16_t x,
                         int16_t y, int32_t w, int32_t h)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    // Clean up previous image if exists (also releases its acc_user scratch data)
    gui_geometry_draw_img_release(input_img);

    if (w <= 0 || h <= 0 || w > UINT16_MAX || h > UINT16_MAX)
    {
        return;
    }

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return; }

    memset(img, 0x00, sizeof(draw_img_t));

    // Check if transformation is applied
    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f);

    if (has_transform)
    {
        uint8_t *payload = create_solid_color_buffer_circle(this, (uint16_t)w, (uint16_t)h,
                                                            this->color);
        if (payload == NULL)
        {
            gui_free(img);
            *input_img = NULL;
            return;
        }
        gui_geometry_img_bind(img, payload, this->opacity_value, gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8,
                                                                                     circle_has_gradient(this), false), this->color);
    }
    else
    {
        /* Deliberately not cached: this payload is 12 bytes of header with no
         * pixels, and its width and height make it near-unique anyway, so the
         * per-node bookkeeping would cost several times the payload. */
        gui_rect_file_head_t *rect_data = gui_malloc(sizeof(gui_rect_file_head_t));
        if (rect_data == NULL)
        {
            gui_free(img);
            *input_img = NULL;
            return;
        }

        set_rect_header((gui_rgb_data_head_t *)rect_data, (uint16_t)w, (uint16_t)h, this->color);

        img->blend_mode = IMG_RECT;
        img->data = rect_data;
    }

    img->opacity_value = this->opacity_value;

    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    matrix_translate(x, y, &img->matrix);
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);
    *input_img = img;
}

/** Attach a strip payload to its draw_img and compute its screen area */
static draw_img_t *finish_arc_strip(gui_circle_t *this, gui_obj_t *obj, draw_img_t *img,
                                    uint8_t *arc_data)
{
    gui_geometry_img_bind(img, arc_data, this->opacity_value, gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8,
                                                                                  circle_has_gradient(this), false), this->color);

    // Copy parent matrix (don't reinitialize - it may contain parent transformations)
    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

/** create vertical arc strip (Legacy/Fallback) */
static draw_img_t *create_vertical_arc_strip(gui_circle_t *this, gui_obj_t *obj,
                                             draw_img_t **old_img, int radius)
{
    // Free old buffer first to prevent memory leak
    gui_geometry_draw_img_release(old_img);

    if (radius <= 0) { return NULL; }

    int inner_half = (int)(radius * M_SQRT1_2);
    int inner_size = inner_half * 2;
    int arc_width = radius - inner_half;
    if (arc_width < 1) { arc_width = 1; }
    int arc_height = inner_size;

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    bool is_a8 = gui_geometry_use_split_a8(gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8,
                                                               circle_has_gradient(this), false), this->opacity_value, GUI_BASE(this)->parent != NULL &&
                                           GUI_BASE(this)->parent->opacity_value == UINT8_MAX);
    circle_desc_t desc;
    bool is_new = false;
    circle_desc_init(&desc, this, CIRCLE_PART_ARC_STRIP, radius, 0);

    uint32_t pixel_bytes = is_a8 ? 1u : 4u;
    uint32_t buffer_size;
    if (!gui_geometry_image_buffer_size(arc_width, arc_height, pixel_bytes, &buffer_size))
    {
        gui_free(img);
        return NULL;
    }

    uint8_t *arc_data = gui_geometry_cache_acquire(&desc, circle_desc_len(&desc), buffer_size,
                                                   &is_new);
    if (arc_data == NULL)
    {
        gui_free(img);
        return NULL;
    }
    if (!is_new)
    {
        // Identical circle already built this strip
        return finish_arc_strip(this, obj, img, arc_data);
    }
    memset(arc_data, 0x00, buffer_size);

    if (is_a8)
    {
        gui_geometry_data_head_set((gui_rgb_data_head_t *)arc_data, (int16_t)arc_width,
                                   (int16_t)arc_height, A8);
    }
    else
    {
        gui_geometry_data_head_set((gui_rgb_data_head_t *)arc_data, (int16_t)arc_width,
                                   (int16_t)arc_height, ARGB8888);
    }

    /* One of these is used, depending on the payload format; the coverage maths
     * below is shared. */
    uint32_t *pixels = (uint32_t *)(arc_data + sizeof(gui_rgb_data_head_t));
    uint8_t *mask = arc_data + sizeof(gui_rgb_data_head_t);
    uint32_t solid_color = this->color.color.argb_full;

    float center_y = (float)arc_height / 2.0f;
    float radius_f = (float)radius;
    float radius_sq = radius_f * radius_f;

    float *exact_boundaries = gui_malloc((size_t)arc_height * sizeof(float));
    if (exact_boundaries == NULL)
    {
        gui_geometry_cache_release(arc_data);
        gui_free(img);
        return NULL;
    }

    for (int y = 0; y < arc_height; y++)
    {
        float py = y + 0.5f - center_y;
        float py_sq = py * py;
        if (py_sq > radius_sq) { exact_boundaries[y] = -1000.0f; }
        else
        {
            float dx = sqrtf(radius_sq - py_sq);
            exact_boundaries[y] = radius_f - dx;
        }
    }

    for (int y = 0; y < arc_height; y++)
    {
        float exact_boundary = exact_boundaries[y];
        if (exact_boundary < -500.0f)
        {
            if (is_a8)
            {
                memset(mask + y * arc_width, this->color.color.rgba.a, (size_t)arc_width);
            }
            else
            {
                for (int x = 0; x < arc_width; x++) { pixels[y * arc_width + x] = solid_color; }
            }
            continue;
        }

        for (int x = 0; x < arc_width; x++)
        {
            float pixel_center_x = x + 0.5f;
            float distance_to_boundary = pixel_center_x - exact_boundary;
            float coverage;

            if (distance_to_boundary >= 0.5f) { coverage = 1.0f; }
            else if (distance_to_boundary <= -0.5f) { coverage = 0.0f; }
            else
            {
                float t = (distance_to_boundary + 0.5f);
                coverage = 0.5f - 0.5f * cosf(M_PI * t);
            }

            if (is_a8)
            {
                /* Mirror the ARGB8888 path's three bands below, so the two agree. */
                uint8_t color_a = this->color.color.rgba.a;
                if (coverage > 0.999f)      { mask[y * arc_width + x] = color_a; }
                else if (coverage > 0.001f) { mask[y * arc_width + x] = (uint8_t)(coverage * (float)color_a); }
                else                        { mask[y * arc_width + x] = 0; }
            }
            else if (coverage > 0.999f) { pixels[y * arc_width + x] = solid_color; }
            else if (coverage > 0.001f)
            {
                gui_color_t color = this->color;
                color.color.rgba.a = (uint8_t)(coverage * color.color.rgba.a);
                pixels[y * arc_width + x] = color.color.argb_full;
            }
            else
            {
                pixels[y * arc_width + x] = 0;
            }
        }
    }

    gui_free(exact_boundaries);

    return finish_arc_strip(this, obj, img, arc_data);
}

/** Create other three arc segments through transformation (Legacy/Fallback)*/
static draw_img_t *create_transformed_arc(gui_circle_t *this, gui_obj_t *obj,
                                          draw_img_t *base_img,
                                          int pos_x, int pos_y,
                                          bool mirror_x, bool mirror_y,
                                          bool is_top_bottom,
                                          draw_img_t **old_img)
{
    GUI_UNUSED(this);
    if (base_img == NULL || base_img->data == NULL) { return NULL; }

    gui_geometry_draw_img_release(old_img);

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }

    memcpy(img, base_img, sizeof(draw_img_t));

    /* Take our own reference on the strip: the mirrored copies and the original
     * are then interchangeable, and the pixels live until the last one goes. */
    img->data = base_img->data;
    gui_geometry_cache_addref(img->data);

    /* Accelerator scratch data is per draw_img and per frame, so it must not be
     * inherited from the copy -- releasing it would double-free base_img's. */
    img->acc_user = NULL;

    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)base_img->data;
    int base_width = head->w;
    int base_height = head->h;

    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    matrix_translate((float)pos_x, (float)pos_y, &img->matrix);

    if (is_top_bottom)
    {
        if (mirror_y) { matrix_rotate(-90.0f, &img->matrix); }
        else { matrix_rotate(90.0f, &img->matrix); }
    }
    else if (mirror_x)
    {
        matrix_translate((float)base_width / 2.0f, (float)base_height / 2.0f, &img->matrix);
        matrix_scale(-1.0f, 1.0f, &img->matrix);
        matrix_translate(-(float)base_width / 2.0f, -(float)base_height / 2.0f, &img->matrix);
    }

    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

static void gui_circle_prepare(gui_obj_t *obj)
{
    gui_circle_t *this = (gui_circle_t *)obj;
    uint32_t last = this->checksum;

    if (this->radius <= 0 || this->radius > INT16_MAX / 2 ||
        this->base.w <= 0 || this->base.h <= 0)
    {
        gui_circle_release_images(this);
        return;
    }

    int radius = this->radius;
    int diameter = radius * 2;

    // obj->matrix is already initialized by gui_obj_ctor
    // Don't reinitialize it - it may contain parent transformations (e.g., list scrolling)

    // Apply transformations if needed
    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f ||
                          this->offset_x != 0.0f || this->offset_y != 0.0f);

    if (has_transform)
    {
        float center_x = (float)radius;
        float center_y = (float)radius;

        matrix_translate(this->offset_x, this->offset_y, obj->matrix);
        matrix_translate(center_x, center_y, obj->matrix);
        matrix_scale(this->scale_x, this->scale_y, obj->matrix);
        matrix_rotate(this->degrees, obj->matrix);
        matrix_translate(-center_x, -center_y, obj->matrix);
    }

    // Calculate checksum only for key properties (exclude pointers)
    // Manually calculate checksum for critical fields only
    uint32_t new_checksum = GUI_GEOMETRY_HASH_INIT;
    new_checksum = gui_geometry_hash_update(new_checksum, &this->x, sizeof(this->x));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->y, sizeof(this->y));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->radius, sizeof(this->radius));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->color, sizeof(this->color));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->stroke_width,
                                            sizeof(this->stroke_width));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->stroke_color,
                                            sizeof(this->stroke_color));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->opacity_value,
                                            sizeof(this->opacity_value));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->degrees, sizeof(this->degrees));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->scale_x, sizeof(this->scale_x));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->scale_y, sizeof(this->scale_y));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->offset_x, sizeof(this->offset_x));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->offset_y, sizeof(this->offset_y));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->use_gradient,
                                            sizeof(this->use_gradient));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->gradient_type,
                                            sizeof(this->gradient_type));
    // Handle bit-field hidden with temporary variable
    uint32_t hidden_val = obj->hidden;
    new_checksum = gui_geometry_hash_update(new_checksum, &hidden_val, sizeof(hidden_val));

    // Include gradient data if present
    if (this->gradient != NULL)
    {
        new_checksum = gui_geometry_hash_update(new_checksum, this->gradient, sizeof(Gradient));
    }

    // Only regenerate buffers if properties changed
    bool need_regenerate = (last != new_checksum);
    uint32_t stroke_path_checksum = GUI_GEOMETRY_HASH_INIT;
    stroke_path_checksum = gui_geometry_hash_update(stroke_path_checksum,
                                                    &this->radius, sizeof(this->radius));
    stroke_path_checksum = gui_geometry_hash_update(stroke_path_checksum,
                                                    &this->stroke_width,
                                                    sizeof(this->stroke_width));
    bool stroke_path_dirty = (stroke_path_checksum != this->stroke_path_checksum);

    /* Single buffer is the baseline representation; splitting the circle into
     * one centre rect plus four arc pieces is the large-shape optimisation.
     *
     * The two groups are kept separate on purpose: must_single_buffer lists
     * what the split representation cannot express (a gradient varies per
     * pixel, a stroke needs the span path, a translucent colour needs a
     * single surface), while the size rule is a cost trade-off.  Merging them
     * into one boolean is how this table and the one in gui_rect.c drifted
     * apart -- see gui_geometry_common.h for what the threshold means. */
    uint64_t circle_area = (uint64_t)(uint32_t)diameter * (uint32_t)diameter;
    bool payload_is_a8 = gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8,
                                             circle_has_gradient(this), false);

    bool must_single_buffer = (this->color.color.rgba.a < 255) ||
                              (this->use_gradient && this->gradient != NULL) ||
                              (this->stroke_width > 0.0f);
    bool need_single_buffer = must_single_buffer ||
                              !gui_geometry_split_pays_off(circle_area, payload_is_a8);

    if (need_single_buffer)
    {
        bool switching_from_split = this->arc_left != NULL || this->arc_right != NULL ||
                                    this->arc_top != NULL || this->arc_bottom != NULL;
        gui_circle_release_arc_images(this);
        if (need_regenerate || switching_from_split || this->center_rect == NULL)
        {
            this->center_rect = create_circle_buffer(this, obj, &this->center_rect);
        }
    }
    else
    {
        // Use optimized multi-part rendering for large opaque circles
        if (need_regenerate || this->center_rect == NULL || this->arc_left == NULL ||
            this->arc_right == NULL || this->arc_top == NULL || this->arc_bottom == NULL)
        {
            gui_circle_release_images(this);

            int inner_half = (int)floorf(radius * M_SQRT1_2);
            int inner_size = inner_half * 2;
            int arc_width = radius - inner_half;

            if (inner_size < 1) { inner_size = 1; }
            if (arc_width < 1) { arc_width = 1; }
            if (inner_size > diameter) { inner_size = diameter; }

            int inner_x = arc_width;
            int inner_y = arc_width;

            if (inner_size > 0)
            {
                set_rect_img(this, &this->center_rect, inner_x, inner_y, inner_size, inner_size);
            }

            this->arc_left = create_vertical_arc_strip(this, obj, &this->arc_left, radius);

            if (this->arc_left != NULL)
            {
                int left_x = 0;
                int left_y = arc_width;

                // Apply position offset on top of parent matrix
                matrix_translate((float)left_x, (float)left_y, &this->arc_left->matrix);

                memcpy(&this->arc_left->inverse, &this->arc_left->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->arc_left->inverse);
                draw_img_new_area(this->arc_left, NULL);

                int right_x = arc_width + inner_size - 1;
                int right_y = arc_width;
                this->arc_right = create_transformed_arc(this, obj, this->arc_left,
                                                         right_x, right_y,
                                                         true, false, false, &this->arc_right);
                int top_x = arc_width + inner_size;
                int top_y = 0;
                this->arc_top = create_transformed_arc(this, obj, this->arc_left,
                                                       top_x, top_y,
                                                       false, false, true, &this->arc_top);

                int bottom_x = arc_width;
                int bottom_y = arc_width * 2 + inner_size - 1;
                this->arc_bottom = create_transformed_arc(this, obj, this->arc_left,
                                                          bottom_x, bottom_y,
                                                          false, true, true, &this->arc_bottom);
            }

            if (this->center_rect == NULL || this->arc_left == NULL ||
                this->arc_right == NULL || this->arc_top == NULL || this->arc_bottom == NULL)
            {
                gui_circle_release_images(this);
            }
        }
    }

    if (this->stroke_width > 0.0f)
    {
        gui_dispdev_t *dc = gui_get_dc();
        bool use_path = gui_geometry_span_can_draw(obj->matrix) &&
                        dc != NULL && (dc->bit_depth == 16 || dc->bit_depth == 32);

        if (use_path)
        {
            if (stroke_path_dirty || this->stroke_span == NULL)
            {
                gui_geometry_path_t path;
                gui_geometry_path_init_circle(&path, radius, this->stroke_width);
                gui_geometry_span_t *spans = gui_geometry_span_acquire(&path);
                if (spans != NULL)
                {
                    gui_geometry_span_release(this->stroke_span);
                    this->stroke_span = spans;
                }
                else
                {
                    gui_geometry_span_release(this->stroke_span);
                    this->stroke_span = NULL;
                }
            }

            if (this->stroke_span != NULL)
            {
                gui_geometry_draw_img_release(&this->stroke_img);
            }
            else if (need_regenerate || this->stroke_img == NULL)
            {
                this->stroke_img =
                    create_circle_stroke_buffer(this, obj, &this->stroke_img);
            }
        }
        else
        {
            gui_geometry_span_release(this->stroke_span);
            this->stroke_span = NULL;
            if (need_regenerate || this->stroke_img == NULL)
            {
                this->stroke_img =
                    create_circle_stroke_buffer(this, obj, &this->stroke_img);
            }
        }
    }
    else
    {
        gui_geometry_draw_img_release(&this->stroke_img);
        gui_geometry_span_release(this->stroke_span);
        this->stroke_span = NULL;
    }

    this->checksum = new_checksum;
    this->stroke_path_checksum = stroke_path_checksum;

    // Check if matrix changed (important for list scrolling optimization)
    bool matrix_changed = (memcmp(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t)) != 0);

    // Only update draw_img matrix and area when matrix actually changed
    if (matrix_changed)
    {
        memcpy(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t));

        // Update center_rect matrix
        if (this->center_rect != NULL)
        {
            // For single-buffer rendering (gradient/alpha/small circles), no offset needed
            if (need_single_buffer)
            {
                // Single buffer - no offset
                memcpy(&this->center_rect->matrix, obj->matrix, sizeof(struct gui_matrix));
            }
            else
            {
                // Multi-part rendering - need to add center rect offset
                int inner_half = (int)floorf(radius * M_SQRT1_2);
                int arc_width = radius - inner_half;
                int inner_x = arc_width;
                int inner_y = arc_width;

                memcpy(&this->center_rect->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate((float)inner_x, (float)inner_y, &this->center_rect->matrix);
            }

            memcpy(&this->center_rect->inverse, &this->center_rect->matrix, sizeof(struct gui_matrix));
            matrix_inverse(&this->center_rect->inverse);
            draw_img_new_area(this->center_rect, NULL);
        }

        if (this->stroke_img != NULL)
        {
            memcpy(&this->stroke_img->matrix, obj->matrix, sizeof(struct gui_matrix));
            memcpy(&this->stroke_img->inverse, obj->matrix, sizeof(struct gui_matrix));
            matrix_inverse(&this->stroke_img->inverse);
            draw_img_new_area(this->stroke_img, NULL);
        }

        // Update arc matrices (for multi-part rendering)
        if (this->arc_left != NULL)
        {
            int inner_half = (int)floorf(radius * M_SQRT1_2);
            int arc_width = radius - inner_half;

            // Update arc_left
            memcpy(&this->arc_left->matrix, obj->matrix, sizeof(struct gui_matrix));
            matrix_translate(0.0f, (float)arc_width, &this->arc_left->matrix);
            memcpy(&this->arc_left->inverse, &this->arc_left->matrix, sizeof(struct gui_matrix));
            matrix_inverse(&this->arc_left->inverse);
            draw_img_new_area(this->arc_left, NULL);

            // Update arc_right
            if (this->arc_right != NULL)
            {
                int inner_size = inner_half * 2;
                int right_x = arc_width + inner_size - 1;
                int right_y = arc_width;

                memcpy(&this->arc_right->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate((float)right_x, (float)right_y, &this->arc_right->matrix);

                gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)this->arc_left->data;
                int base_width = head->w;
                int base_height = head->h;
                matrix_translate((float)base_width / 2.0f, (float)base_height / 2.0f, &this->arc_right->matrix);
                matrix_scale(-1.0f, 1.0f, &this->arc_right->matrix);
                matrix_translate(-(float)base_width / 2.0f, -(float)base_height / 2.0f, &this->arc_right->matrix);

                memcpy(&this->arc_right->inverse, &this->arc_right->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->arc_right->inverse);
                draw_img_new_area(this->arc_right, NULL);
            }

            // Update arc_top
            if (this->arc_top != NULL)
            {
                int inner_size = inner_half * 2;
                int top_x = arc_width + inner_size;
                int top_y = 0;

                memcpy(&this->arc_top->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate((float)top_x, (float)top_y, &this->arc_top->matrix);
                matrix_rotate(90.0f, &this->arc_top->matrix);

                memcpy(&this->arc_top->inverse, &this->arc_top->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->arc_top->inverse);
                draw_img_new_area(this->arc_top, NULL);
            }

            // Update arc_bottom
            if (this->arc_bottom != NULL)
            {
                int inner_size = inner_half * 2;
                int bottom_x = arc_width;
                int bottom_y = arc_width * 2 + inner_size - 1;

                memcpy(&this->arc_bottom->matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_translate((float)bottom_x, (float)bottom_y, &this->arc_bottom->matrix);
                matrix_rotate(-90.0f, &this->arc_bottom->matrix);

                memcpy(&this->arc_bottom->inverse, &this->arc_bottom->matrix, sizeof(struct gui_matrix));
                matrix_inverse(&this->arc_bottom->inverse);
                draw_img_new_area(this->arc_bottom, NULL);
            }
        }
    }

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

/** Drawing phase processing */
static void gui_circle_draw(gui_obj_t *obj)
{
    gui_circle_t *this = (gui_circle_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    // Update opacity value to consider parent's opacity (like gui_img does)
    uint8_t final_opacity = obj->parent->opacity_value * this->opacity_value / UINT8_MAX;

    if (this->stroke_span != NULL)
    {
        gui_geometry_span_draw(this->stroke_span, obj->matrix,
                               this->stroke_color, final_opacity, dc);
    }
    if (this->stroke_img != NULL)
    {
        this->stroke_img->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->stroke_img, dc, NULL);
    }
    if (this->center_rect != NULL)
    {
        this->center_rect->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->center_rect, dc, NULL);
    }
    if (this->arc_left != NULL)
    {
        this->arc_left->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->arc_left, dc, NULL);
    }
    if (this->arc_right != NULL)
    {
        this->arc_right->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->arc_right, dc, NULL);
    }
    if (this->arc_top != NULL)
    {
        this->arc_top->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->arc_top, dc, NULL);
    }
    if (this->arc_bottom != NULL)
    {
        this->arc_bottom->opacity_value = final_opacity;
        gui_acc_blit_to_dc(this->arc_bottom, dc, NULL);
    }
}

/** End phase processing - Memory management */
static void gui_circle_end(gui_circle_t *this)
{
    /* Pixel buffers are cached across frames and must NOT be freed here; they
     * are released in gui_circle_destroy() or when the render mode changes.
     * HW-acceleration user data (acc_user) is different: a hardware
     * draw_img_acc_prepare_cb allocates it every frame inside
     * draw_img_new_area, so it must be released now or the next frame's
     * prepare call orphans the old pointer. */
    if (draw_img_acc_end_cb != NULL)
    {
        if (this->stroke_img != NULL) { draw_img_acc_end_cb(this->stroke_img); }
        if (this->center_rect != NULL) { draw_img_acc_end_cb(this->center_rect); }
        if (this->arc_left   != NULL) { draw_img_acc_end_cb(this->arc_left);   }
        if (this->arc_right  != NULL) { draw_img_acc_end_cb(this->arc_right);  }
        if (this->arc_top    != NULL) { draw_img_acc_end_cb(this->arc_top);    }
        if (this->arc_bottom != NULL) { draw_img_acc_end_cb(this->arc_bottom); }
    }
}

static void gui_circle_destroy(gui_circle_t *this)
{
    // Free gradient data
    if (this->gradient != NULL)
    {
        gui_free(this->gradient);
        this->gradient = NULL;
    }

    gui_circle_release_images(this);
}

static void gui_circle_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PROCESS:
            gui_circle_input_process(obj);
            break;
        case OBJ_PRE_PROCESS:
            gui_circle_prepare(obj);
            break;
        case OBJ_PROCESS:
            gui_circle_draw(obj);
            break;
        case OBJ_POST_PROCESS:
            gui_circle_end((gui_circle_t *)obj);
            break;
        case OBJ_DESTROY:
            gui_circle_destroy((gui_circle_t *)obj);
            break;
        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_circle_t *gui_circle_create(void *parent, const char *name, int x, int y,
                                int radius, gui_color_t color)
{
    if (parent == NULL || !gui_geometry_circle_bounds_valid(x, y, radius, false))
    {
        GUI_ASSERT(parent != NULL && gui_geometry_circle_bounds_valid(x, y, radius, false));
        return NULL;
    }

    gui_circle_t *circle = gui_malloc(sizeof(gui_circle_t));
    if (circle == NULL)
    {
        gui_log("Failed to allocate circle widget\n");
        return NULL;
    }

    memset(circle, 0x00, sizeof(gui_circle_t));

    /* Colour alpha is stored in the raster payload.  Widget opacity is an
     * independent multiplier and therefore starts fully opaque. */
    circle->opacity_value = UINT8_MAX;
    gui_obj_ctor((gui_obj_t *)circle, parent, name, x - radius, y - radius, radius * 2, radius * 2);
    GET_BASE(circle)->obj_cb = gui_circle_cb;
    GET_BASE(circle)->has_input_process_cb = true;
    GET_BASE(circle)->has_pre_process_cb = true;
    GET_BASE(circle)->has_process_cb = true;
    GET_BASE(circle)->has_post_process_cb = true;
    GET_BASE(circle)->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(circle)->child_list));
    if ((GET_BASE(circle)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(circle)->parent)->child_list),
                               &(GET_BASE(circle)->brother_list));
    }
    GET_BASE(circle)->create_done = true;
    circle->x = radius;
    circle->y = radius;
    circle->radius = radius;
    circle->color = color;
    circle->stroke_width = 0.0f;
    circle->stroke_color = gui_rgba(0, 0, 0, 0);
    circle->checksum = 0;
    circle->degrees = 0.0f;
    circle->scale_x = 1.0f;
    circle->scale_y = 1.0f;
    circle->offset_x = 0.0f;
    circle->offset_y = 0.0f;
    circle->gradient = NULL;
    circle->use_gradient = false;
    circle->gradient_type = CIRCLE_GRADIENT_RADIAL;

    // Initialize last_matrix to identity
    matrix_identity(&circle->last_matrix);

    return circle;
}

void gui_circle_set_style(gui_circle_t *circle, int x, int y, int radius, gui_color_t color)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL || !gui_geometry_circle_bounds_valid(x, y, radius, true))
    {
        return;
    }

    // Calculate new widget bounding box
    int new_base_x = x - radius;
    int new_base_y = y - radius;
    int new_size = radius * 2;

    // Update widget position and size
    circle->base.x = new_base_x;
    circle->base.y = new_base_y;
    circle->base.w = new_size;
    circle->base.h = new_size;

    // Circle center is always at the center of the bounding box
    circle->x = radius;
    circle->y = radius;
    circle->radius = radius;
    circle->color = color;
    update_circle_gradient_geometry(circle);
}

void gui_circle_set_position(gui_circle_t *circle, int x, int y)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL || !gui_geometry_circle_bounds_valid(x, y, circle->radius, true))
    {
        return;
    }

    // Calculate new widget bounding box position
    int new_base_x = x - circle->radius;
    int new_base_y = y - circle->radius;

    // Update widget position if changed
    if (circle->base.x != new_base_x || circle->base.y != new_base_y)
    {
        circle->base.x = new_base_x;
        circle->base.y = new_base_y;

        // Circle center is always at the center of the bounding box
        circle->x = circle->radius;
        circle->y = circle->radius;
    }
}

void gui_circle_set_radius(gui_circle_t *circle, int radius)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL)
    {
        return;
    }

    int center_x = circle->base.x + circle->x;
    int center_y = circle->base.y + circle->y;
    if (!gui_geometry_circle_bounds_valid(center_x, center_y, radius, true))
    {
        return;
    }

    if (circle->radius != radius)
    {
        gui_circle_release_images(circle);
        circle->base.x = center_x - radius;
        circle->base.y = center_y - radius;
        circle->base.w = radius * 2;
        circle->base.h = radius * 2;
        circle->x = radius;
        circle->y = radius;
        circle->radius = radius;
        update_circle_gradient_geometry(circle);
    }
}
void gui_circle_set_opacity(gui_circle_t *circle, uint8_t opacity)
{
    GUI_ASSERT(circle != NULL);
    circle->opacity_value = opacity;
}
void gui_circle_set_color(gui_circle_t *circle, gui_color_t color)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL)
    {
        return;
    }

    if (circle->color.color.argb_full != color.color.argb_full)
    {
        circle->color = color;
    }
}

void gui_circle_set_stroke(gui_circle_t *circle, float width, gui_color_t color)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL)
    {
        return;
    }

    circle->stroke_width = (width > 0.0f) ? width : 0.0f;
    circle->stroke_color = color;
}

void gui_circle_clear_stroke(gui_circle_t *circle)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL)
    {
        return;
    }

    circle->stroke_width = 0.0f;
}

void gui_circle_on_click(gui_circle_t *circle, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)circle, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}

void gui_circle_rotate(gui_circle_t *circle, float degrees)
{
    GUI_ASSERT(circle != NULL);
    circle->degrees = degrees;
}

void gui_circle_scale(gui_circle_t *circle, float scale_x, float scale_y)
{
    GUI_ASSERT(circle != NULL);
    circle->scale_x = scale_x;
    circle->scale_y = scale_y;
}

void gui_circle_translate(gui_circle_t *circle, float tx, float ty)
{
    GUI_ASSERT(circle != NULL);
    circle->offset_x = tx;
    circle->offset_y = ty;
}

void gui_circle_set_radial_gradient(gui_circle_t *circle)
{
    GUI_ASSERT(circle != NULL);
    if (circle->gradient == NULL)
    {
        circle->gradient = gui_malloc(sizeof(Gradient));
        if (circle->gradient == NULL) { return; }
    }
    gradient_init(circle->gradient, GRADIENT_RADIAL);
    circle->gradient->radial_cx = (float)circle->radius;
    circle->gradient->radial_cy = (float)circle->radius;
    circle->gradient->radial_r = (float)circle->radius;
    circle->gradient_type = CIRCLE_GRADIENT_RADIAL;
    circle->use_gradient = true;
}

void gui_circle_set_angular_gradient(gui_circle_t *circle, float start_angle, float end_angle)
{
    GUI_ASSERT(circle != NULL);
    if (circle->gradient == NULL)
    {
        circle->gradient = gui_malloc(sizeof(Gradient));
        if (circle->gradient == NULL) { return; }
    }
    gradient_init(circle->gradient, GRADIENT_ANGULAR);
    circle->gradient->angular_cx = (float)circle->radius;
    circle->gradient->angular_cy = (float)circle->radius;
    circle->gradient->angular_start = start_angle;
    circle->gradient->angular_end = end_angle;
    circle->gradient_type = CIRCLE_GRADIENT_ANGULAR;
    circle->use_gradient = true;
}

void gui_circle_add_gradient_stop(gui_circle_t *circle, float position, gui_color_t color)
{
    GUI_ASSERT(circle != NULL);
    if (circle->gradient == NULL)
    {
        gui_circle_set_radial_gradient(circle);
    }
    if (circle->gradient != NULL)
    {
        gradient_add_stop(circle->gradient, position, color.color.argb_full);
    }
}

void gui_circle_clear_gradient(gui_circle_t *circle)
{
    GUI_ASSERT(circle != NULL);
    if (circle->gradient != NULL)
    {
        gui_free(circle->gradient);
        circle->gradient = NULL;
    }
    circle->use_gradient = false;
}
