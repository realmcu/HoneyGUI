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
#include "gui_geometry_glow.h"

/*============================================================================*
 *                           Configuration
 *============================================================================*/
// 1 = Enable Dither (Better gradients), 0 = Disable (Max speed)
#define GUI_CIRCLE_ENABLE_DITHER 1

/**
 * 1 = Store solid circles as A8 masks (saves 75% memory, allows color sharing).
 * 0 = Always ARGB8888 (use for hardware accelerators that don't support A8).
 * Override via: scons GUI_CIRCLE_ENABLE_A8=0
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

static bool circle_has_gradient(const gui_circle_t *circle)
{
    return circle->use_gradient && circle->gradient != NULL &&
           circle->gradient->stop_count >= 2;
}

/**
 * Fill in a descriptor for one cached payload.
 * Zeroes first to prevent padding holes from breaking byte-for-byte cache comparison.
 */
static void circle_desc_init(circle_desc_t *desc, const gui_circle_t *this,
                             gui_circle_part_t part, int w, int h, bool is_a8)
{
    memset(desc, 0x00, sizeof(*desc));

    desc->part = (uint32_t)part;
    desc->radius = this->radius;
    desc->size_a = w;
    desc->size_b = h;
    desc->gradient_type = UINT32_MAX;
    desc->stroke_width = this->stroke_width;
    if (this->stroke_width > 0.0f)
    {
        desc->stroke_color = this->stroke_color.color.argb_full;
    }

    /* A8 ignores RGB to allow mask sharing; alpha stays in as it's baked into mask values. */
    if (is_a8)
    {
        desc->is_a8 = 1u;
        desc->color = this->color.color.rgba.a;
    }
    else
    {
        desc->color = this->color.color.argb_full;
    }

    /* Only full-buffer carries gradient; split quadrants cannot survive gradient mirroring. */
    if (part == CIRCLE_PART_FULL && circle_has_gradient(this))
    {
        desc->gradient_type = (uint32_t)this->gradient_type;
        memcpy(&desc->gradient, this->gradient, sizeof(Gradient));
    }
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

    /* Accumulate widget origin to convert absolute touch coords to local. */
    gui_obj_absolute_xy(obj, &absolute_x, &absolute_y);

    if (!is_point_in_circle(this, tp->x - absolute_x, tp->y - absolute_y))
    {
        return;
    }

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG, "touch");
    /* PRESSING repeats every frame for drag; tp->x/tp->y remain at press origin. */
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING, "touch");
}

static void circle_geom_init(circle_geom_t *g, const gui_circle_t *this)
{
    g->radius = this->radius;
    g->stroke = LG_CLAMP(this->stroke_width, 0.0f, (float)this->radius);
    g->inner_radius = (float)this->radius - g->stroke;

    /* Center square side: keep corner pixel at least 0.5px inside the fill. */
    float fill_r = (g->stroke > 0.0f) ? g->inner_radius : (float)this->radius;
    g->half = (int)floorf(LG_MAX(fill_r - 0.5f, 0.0f) * (float)M_SQRT1_2);
    g->band = this->radius - g->half;
}

/** Angular gradient table: one entry per degree. */
#define CIRCLE_ANGULAR_LUT_SIZE 360

/** Gradient LUT: lookups by whole-pixel distance (radial) or whole degree (angular). */
typedef struct
{
    uint32_t *lut;              /**< NULL: no gradient, or no memory for the table. */
    int n;                      /**< Table entries. */
} circle_lut_t;

static float circle_angular_t(const gui_circle_t *this, float angle)
{
    float range = this->gradient->angular_end - this->gradient->angular_start;
    if (range <= 0.0f) { range = 360.0f; }
    float t = (angle - this->gradient->angular_start) / range;
    if (t < 0.0f) { t += 1.0f; }
    return LG_CLAMP(t, 0.0f, 1.0f);
}

static void circle_lut_init(circle_lut_t *l, gui_circle_t *this)
{
    l->lut = NULL;
    if (!circle_has_gradient(this)) { return; }

    bool angular = (this->gradient_type == CIRCLE_GRADIENT_ANGULAR);
    int n = angular ? CIRCLE_ANGULAR_LUT_SIZE : this->radius + 1;
    l->lut = gui_malloc((size_t)n * sizeof(uint32_t));
    if (l->lut == NULL) { return; }

    for (int i = 0; i < n; i++)
    {
        float t = angular ? circle_angular_t(this, (float)i * 360.0f / (float)n) :
                  (float)i / (float)this->radius;
        l->lut[i] = gradient_get_color(this->gradient, LG_CLAMP(t, 0.0f, 1.0f));
    }
    l->n = n;
}

static void circle_lut_free(circle_lut_t *l)
{
    if (l->lut != NULL)
    {
        gui_free(l->lut);
        l->lut = NULL;
    }
}

/** Fill colour at an offset from the centre; @p l may be NULL. */
static uint32_t circle_fill_color_at(gui_circle_t *this, const circle_lut_t *l,
                                     float dx, float dy, float dist)
{
    if (!circle_has_gradient(this))
    {
        return this->color.color.argb_full;
    }

    bool angular = (this->gradient_type == CIRCLE_GRADIENT_ANGULAR);
    float angle = 0.0f;
    if (angular)
    {
        angle = atan2f(dy, dx) * 180.0f / M_PI;
        if (angle < 0.0f) { angle += 360.0f; }
    }

    if (l != NULL && l->lut != NULL)
    {
        int i = angular ? (int)(angle * (float)l->n / 360.0f) : (int)(dist + 0.5f);
        return l->lut[LG_CLAMP(i, 0, l->n - 1)];
    }

    float t = angular ? circle_angular_t(this, angle) : dist / (float)this->radius;
    return gradient_get_color(this->gradient, LG_CLAMP(t, 0.0f, 1.0f));
}

/**
 * Blend fill and stroke for one pixel.
 * Uses squared distances to skip sqrt for pixels fully inside/outside.
 */
static uint32_t circle_pixel(gui_circle_t *this, const circle_geom_t *g,
                             const circle_lut_t *l, int x, int y)
{
    float r = (float)g->radius;
    float dx = (float)x + 0.5f - r;
    float dy = (float)y + 0.5f - r;
    float d2 = dx * dx + dy * dy;

    /* Coverage = r + 0.5 - dist */
    if (d2 >= (r + 0.5f) * (r + 0.5f)) { return 0u; }

    bool has_stroke = g->stroke > 0.0f;
    float ri = has_stroke ? g->inner_radius : r;
    bool radial = circle_has_gradient(this) && this->gradient_type == CIRCLE_GRADIENT_RADIAL;
    bool near_outer = d2 > (r - 0.5f) * (r - 0.5f);
    bool near_inner = has_stroke && ri > 0.0f &&
                      d2 > LG_MAX(ri - 0.5f, 0.0f) * LG_MAX(ri - 0.5f, 0.0f) &&
                      d2 < (ri + 0.5f) * (ri + 0.5f);
    bool in_fill = !has_stroke || (ri > 0.0f && d2 < (ri + 0.5f) * (ri + 0.5f));

    float dist = (near_outer || near_inner || (radial && in_fill)) ? sqrtf(d2) : 0.0f;
    float c_out = near_outer ? LG_CLAMP(r + 0.5f - dist, 0.0f, 1.0f) : 1.0f;
    float c_in;
    if (!has_stroke)
    {
        c_in = c_out;
    }
    else if (near_inner)
    {
        c_in = LG_CLAMP(ri + 0.5f - dist, 0.0f, c_out);
    }
    else
    {
        c_in = (ri > 0.0f && d2 <= (ri - 0.5f) * (ri - 0.5f)) ? c_out : 0.0f;
    }

    uint32_t fill = 0u;
    if (c_in > 0.0f)
    {
        fill = circle_fill_color_at(this, l, dx, dy, dist);
        if (circle_has_gradient(this) && GUI_CIRCLE_ENABLE_DITHER)
        {
            fill = gui_geometry_dither_argb8888(fill, x, y);
        }
    }
    return gui_geometry_mix_fill_stroke(fill, this->stroke_color.color.argb_full, c_out, c_in);
}

/** Rasterise the window (ox, oy, pw x ph) of the circle into a payload body. */
static void circle_raster(gui_circle_t *this, const circle_geom_t *g, uint8_t *body,
                          bool is_a8, int ox, int oy, int pw, int ph)
{
    uint32_t *pixels = (uint32_t *)body;
    circle_lut_t lut;
    circle_lut_init(&lut, this);

    for (int y = 0; y < ph; y++)
    {
        for (int x = 0; x < pw; x++)
        {
            uint32_t color = circle_pixel(this, g, &lut, ox + x, oy + y);

            if (is_a8)
            {
                body[y * pw + x] = (uint8_t)(color >> 24);
            }
            else
            {
                pixels[y * pw + x] = color;
            }
        }
    }

    circle_lut_free(&lut);
}

/** Rasterise (or share) the payload for the window (ox, oy, pw x ph). */
static uint8_t *circle_payload(gui_circle_t *this, const circle_geom_t *g,
                               gui_circle_part_t part, int ox, int oy, int pw, int ph,
                               bool is_a8)
{
    circle_desc_t desc;
    circle_desc_init(&desc, this, part, pw, ph, is_a8);

    bool is_new = false;
    uint8_t *buffer = gui_geometry_payload_acquire(&desc, circle_desc_len(&desc),
                                                   pw, ph, is_a8, &is_new);
    if (buffer != NULL && is_new)
    {
        circle_raster(this, g, buffer + sizeof(gui_rgb_data_head_t), is_a8, ox, oy, pw, ph);
    }
    return buffer;
}

static bool circle_add_pinwheel(gui_circle_t *this, uint8_t *payload, bool is_a8)
{
    const gui_matrix_t *m = GUI_BASE(this)->matrix;
    int b = this->radius * 2 - 1;

    /* (x, y, xx, xy, yx, yy): top strip, then rotated 90, 180, 270 deg. */
    const int t[4][6] =
    {
        {0, 0, 1, 0, 0, 1}, {b, 0, 0, -1, 1, 0}, {b, b, -1, 0, 0, -1}, {0, b, 0, 1, -1, 0},
    };

    for (int i = 0; i < 4; i++)
    {
        /* Each copy holds its own reference. */
        if (i > 0) { gui_geometry_cache_addref(payload); }

        if (!gui_geometry_parts_add_image(&this->parts, m, payload, this->opacity_value,
                                          is_a8, this->color, t[i][0], t[i][1],
                                          t[i][2], t[i][3], t[i][4], t[i][5]))
        {
            return false;
        }
    }
    return true;
}

/**
 * Build every piece of the circle.
 * Split: center square + one strip turned 4 times.
 * Single: one full image (for gradients, unsupported transforms, or tiny circles).
 */
static bool circle_build_parts(gui_circle_t *this, const circle_geom_t *g, bool single)
{
    int d = g->radius * 2;
    int side = g->half * 2;

    if (single)
    {
        bool is_a8 = g->stroke <= 0.0f &&
                     gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8, circle_has_gradient(this), false);
        uint8_t *payload = circle_payload(this, g, CIRCLE_PART_FULL, 0, 0, d, d, is_a8);
        return payload != NULL &&
               gui_geometry_parts_add_image(&this->parts, GUI_BASE(this)->matrix, payload,
                                            this->opacity_value, is_a8, this->color,
                                            0, 0, 1, 0, 0, 1);
    }

    bool is_a8 = g->stroke <= 0.0f &&
                 gui_geometry_use_split_a8(gui_geometry_use_a8(GUI_CIRCLE_ENABLE_A8, false, false),
                                           this->opacity_value,
                                           GUI_BASE(this)->parent != NULL &&
                                           GUI_BASE(this)->parent->opacity_value == UINT8_MAX);

    uint8_t *strip = circle_payload(this, g, CIRCLE_PART_STRIP, 0, 0,
                                    side + g->band, g->band, is_a8);
    if (strip == NULL || !circle_add_pinwheel(this, strip, is_a8))
    {
        return false;
    }

    return gui_geometry_parts_add_solid(&this->parts, GUI_BASE(this)->matrix,
                                        g->band, g->band, side, side,
                                        circle_pixel(this, g, NULL, g->radius, g->radius),
                                        this->opacity_value);
}

static void gui_circle_prepare(gui_obj_t *obj)
{
    gui_circle_t *this = (gui_circle_t *)obj;
    uint32_t last = this->checksum;

    if (this->radius <= 0 || this->radius > INT16_MAX / 2 ||
        this->base.w <= 0 || this->base.h <= 0)
    {
        gui_geometry_parts_release(&this->parts);
        return;
    }

    int radius = this->radius;

    bool uniform_scale = fabsf(this->scale_x - this->scale_y) < 1e-4f;
    float matrix_degrees = uniform_scale ? 0.0f : this->degrees;
    float folded_degrees = uniform_scale ? this->degrees : 0.0f;

    bool has_transform = (matrix_degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f ||
                          this->offset_x != 0.0f || this->offset_y != 0.0f);
    bool parent_has_non_translate = !matrix_only_translate(obj->matrix);
    gui_matrix_t base_matrix;
    memcpy(&base_matrix, obj->matrix, sizeof(gui_matrix_t));

    if (has_transform)
    {
        float center_x = (float)radius;
        float center_y = (float)radius;

        matrix_translate(this->offset_x, this->offset_y, obj->matrix);
        matrix_translate(center_x, center_y, obj->matrix);
        matrix_scale(this->scale_x, this->scale_y, obj->matrix);
        matrix_rotate(matrix_degrees, obj->matrix);
        matrix_translate(-center_x, -center_y, obj->matrix);
    }

    bool bake_scale = false;
    gui_matrix_t scaled_matrix;
    int16_t saved_w = this->base.w;
    int16_t saved_h = this->base.h;
    int saved_x = this->x;
    int saved_y = this->y;
    int saved_radius = this->radius;
    float saved_stroke_width = this->stroke_width;

    float scale_factor = 1.0f;

    Gradient saved_gradient;
    if (this->gradient != NULL)
    {
        memcpy(&saved_gradient, this->gradient, sizeof(Gradient));
    }

    if (has_transform && uniform_scale && !parent_has_non_translate &&
        this->scale_x > 0.0f && this->scale_x != 1.0f)
    {
        int scaled_radius = (int)roundf((float)radius * this->scale_x);

        if (scaled_radius >= 1 && scaled_radius <= INT16_MAX / 2)
        {
            bake_scale = true;
            scale_factor = this->scale_x;
            memcpy(&scaled_matrix, obj->matrix, sizeof(gui_matrix_t));

            /* Keep center snapped to whole pixel for seamless split pieces. */
            memcpy(obj->matrix, &base_matrix, sizeof(gui_matrix_t));
            matrix_translate(roundf(this->offset_x + (float)(radius - scaled_radius)),
                             roundf(this->offset_y + (float)(radius - scaled_radius)),
                             obj->matrix);

            this->radius = scaled_radius;
            this->x = scaled_radius;
            this->y = scaled_radius;
            this->base.w = (int16_t)(scaled_radius * 2);
            this->base.h = (int16_t)(scaled_radius * 2);
            this->stroke_width *= this->scale_x;

            update_circle_gradient_geometry(this);

            radius = scaled_radius;
        }
    }

    if (folded_degrees != 0.0f && circle_has_gradient(this) &&
        this->gradient_type == CIRCLE_GRADIENT_ANGULAR)
    {
        float range = this->gradient->angular_end - this->gradient->angular_start;
        float start = fmodf(this->gradient->angular_start + folded_degrees, 360.0f);
        if (start < 0.0f) { start += 360.0f; }
        this->gradient->angular_start = start;
        this->gradient->angular_end = start + range;
    }

    // Checksum (exclude pointers)
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
    /* degrees omitted: matrix part re-places pieces, folded part affects gradient hash. */
    new_checksum = gui_geometry_hash_update(new_checksum, &this->scale_x, sizeof(this->scale_x));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->scale_y, sizeof(this->scale_y));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->offset_x, sizeof(this->offset_x));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->offset_y, sizeof(this->offset_y));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->use_gradient,
                                            sizeof(this->use_gradient));
    new_checksum = gui_geometry_hash_update(new_checksum, &this->gradient_type,
                                            sizeof(this->gradient_type));

    uint32_t hidden_val = obj->hidden;
    new_checksum = gui_geometry_hash_update(new_checksum, &hidden_val, sizeof(hidden_val));

    if (this->gradient != NULL)
    {
        new_checksum = gui_geometry_hash_update(new_checksum, this->gradient, sizeof(Gradient));
    }

    circle_geom_t geom;
    circle_geom_init(&geom, this);
    uint64_t circle_area = (uint64_t)(uint32_t)(radius * 2) * (uint32_t)(radius * 2);
    bool payload_is_a8 = geom.stroke <= 0.0f && !circle_has_gradient(this) &&
                         GUI_CIRCLE_ENABLE_A8;
    bool single = circle_has_gradient(this) || (has_transform && !bake_scale) ||
                  parent_has_non_translate || geom.half <= 0 ||
                  !gui_geometry_split_pays_off(circle_area, payload_is_a8);
    new_checksum = gui_geometry_hash_update(new_checksum, &single, sizeof(single));

    /* Split strips A8 depends on parent opacity (rebuild when crossing fully opaque). */
    bool parent_opaque = obj->parent != NULL && obj->parent->opacity_value == UINT8_MAX;
    new_checksum = gui_geometry_hash_update(new_checksum, &parent_opaque, sizeof(parent_opaque));

    bool moved = this->parts.count > 0 &&
                 memcmp(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t)) != 0;

    // All pieces are rebuilt together
    if (last != new_checksum || this->parts.count == 0)
    {
        gui_geometry_parts_release(&this->parts);
        if (!circle_build_parts(this, &geom, single))
        {
            gui_geometry_parts_release(&this->parts);
        }
        memcpy(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t));
    }
    else if (moved)
    {
        // Moved only: re-place, keep pixels.
        memcpy(&this->last_matrix, obj->matrix, sizeof(gui_matrix_t));
        gui_geometry_parts_place(&this->parts, obj->matrix);
    }

    this->checksum = new_checksum;

    if (this->glow_blur > 0 && this->glow_color.color.rgba.a > 0)
    {
        gui_geometry_glow_input_t in;
        memset(&in, 0x00, sizeof(in));
        in.shape = GUI_GEOMETRY_GLOW_SHAPE_CIRCLE;
        in.single = (has_transform && !bake_scale) || parent_has_non_translate;
        in.w = this->radius * 2;
        in.h = this->radius * 2;
        in.radius = this->radius;
        in.blur_q = gui_geometry_glow_blur_q(bake_scale ? (float)this->glow_blur * scale_factor
                                             : (float)this->glow_blur);
        in.edge_alpha = this->stroke_width > 0.0f ? this->stroke_color.color.rgba.a
                        : this->color.color.rgba.a;
        gui_geometry_glow_prepare(&this->glow, &in, obj->matrix, moved, this->glow_color);
    }
    else if (this->glow != NULL)
    {
        gui_geometry_glow_prepare(&this->glow, NULL, obj->matrix, moved, this->glow_color);
    }

    if (bake_scale)
    {
        memcpy(obj->matrix, &scaled_matrix, sizeof(gui_matrix_t));
        this->base.w = saved_w;
        this->base.h = saved_h;
        this->x = saved_x;
        this->y = saved_y;
        this->radius = saved_radius;
        this->stroke_width = saved_stroke_width;
    }

    if (this->gradient != NULL)
    {
        memcpy(this->gradient, &saved_gradient, sizeof(Gradient));
    }

    /* Children inherit obj->matrix; add folded rotation back for them. */
    if (folded_degrees != 0.0f)
    {
        float c = (float)this->radius;
        matrix_translate(c, c, obj->matrix);
        matrix_rotate(folded_degrees, obj->matrix);
        matrix_translate(-c, -c, obj->matrix);
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

    gui_geometry_glow_draw(this->glow, obj->parent->opacity_value, this->opacity_value);

    gui_geometry_parts_draw(&this->parts,
                            obj->parent->opacity_value * this->opacity_value / UINT8_MAX);
}

/** End phase processing - Memory management */
static void gui_circle_end(gui_circle_t *this)
{
    gui_geometry_parts_end(&this->parts);

    if (this->glow != NULL)
    {
        gui_geometry_parts_end(&this->glow->parts);
    }
}

static void gui_circle_destroy(gui_circle_t *this)
{
    if (this->gradient != NULL)
    {
        gui_free(this->gradient);
        this->gradient = NULL;
    }

    gui_geometry_glow_free(&this->glow);
    gui_geometry_parts_release(&this->parts);
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

    /* Color alpha is rasterized into payload; widget opacity starts opaque. */
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

    int new_base_x = x - radius;
    int new_base_y = y - radius;
    int new_size = radius * 2;

    circle->base.x = new_base_x;
    circle->base.y = new_base_y;
    circle->base.w = new_size;
    circle->base.h = new_size;

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

    int new_base_x = x - circle->radius;
    int new_base_y = y - circle->radius;

    if (circle->base.x != new_base_x || circle->base.y != new_base_y)
    {
        circle->base.x = new_base_x;
        circle->base.y = new_base_y;

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
        gui_geometry_parts_release(&circle->parts);
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

void gui_circle_set_glow(gui_circle_t *circle, gui_color_t color, uint8_t blur_radius)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL)
    {
        return;
    }
    if (blur_radius == 0u || color.color.rgba.a == 0u)
    {
        gui_circle_clear_glow(circle);
        return;
    }

    circle->glow_color = color;
    circle->glow_blur = (blur_radius > GUI_GEOMETRY_GLOW_MAX_BLUR) ?
                        GUI_GEOMETRY_GLOW_MAX_BLUR : blur_radius;
}

void gui_circle_clear_glow(gui_circle_t *circle)
{
    GUI_ASSERT(circle != NULL);
    if (circle == NULL)
    {
        return;
    }

    circle->glow_blur = 0u;
    gui_geometry_glow_free(&circle->glow);
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