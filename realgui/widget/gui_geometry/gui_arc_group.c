/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_fb.h"
#include "draw_img.h"
#include "acc_api.h"
#include "lite_geometry.h"
#include "gui_arc_group.h"

static void gui_arc_group_prepare(gui_arc_group_t *this);
static void gui_arc_group_draw(gui_arc_group_t *this);
static void gui_arc_group_end(gui_arc_group_t *this);
static void gui_arc_group_destroy(gui_arc_group_t *this);

static void set_img_header(gui_rgb_data_head_t *head, uint16_t w, uint16_t h)
{
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = w;
    head->h = h;
    head->version = 0;
    head->rsvd2 = 0;
}

/**
 * Calculate the bounding box for a single arc
 * Returns the min/max coordinates that the arc occupies
 */
static void calculate_arc_bbox(float cx, float cy, float radius, float line_width,
                               float start_angle, float end_angle,
                               float *min_x, float *max_x, float *min_y, float *max_y)
{
    float outer_r = radius + line_width / 2.0f + 2.0f;
    float inner_r = radius - line_width / 2.0f - 2.0f;
    if (inner_r < 0) { inner_r = 0; }

    // Calculate angle span BEFORE normalization to detect full circles
    float angle_span = end_angle - start_angle;
    if (angle_span < 0) { angle_span += 360.0f; }

    // Check if this is a full circle (including 0->360 case)
    if (angle_span >= 359.0f || angle_span <= 1.0f)
    {
        // Full circle - use entire bounding box
        *min_x = cx - outer_r;
        *max_x = cx + outer_r;
        *min_y = cy - outer_r;
        *max_y = cy + outer_r;
        return;
    }

    // Normalize angles to 0-360 range for partial arc calculation
    while (start_angle < 0) { start_angle += 360.0f; }
    while (end_angle < 0) { end_angle += 360.0f; }
    while (start_angle >= 360.0f) { start_angle -= 360.0f; }
    while (end_angle >= 360.0f) { end_angle -= 360.0f; }

    // For partial arcs, calculate actual bounds
    *min_x = cx;
    *max_x = cx;
    *min_y = cy;
    *max_y = cy;

    // Sample points along the arc
    int num_samples = (int)(angle_span / 10.0f) + 2;
    if (num_samples < 3) { num_samples = 3; }

    for (int i = 0; i <= num_samples; i++)
    {
        float t = (float)i / (float)num_samples;
        float angle = start_angle + t * angle_span;
        float rad = angle * 3.14159265f / 180.0f;
        float cos_a = cosf(rad);
        float sin_a = sinf(rad);

        // Check outer edge
        float x_outer = cx + outer_r * cos_a;
        float y_outer = cy + outer_r * sin_a;
        if (x_outer < *min_x) { *min_x = x_outer; }
        if (x_outer > *max_x) { *max_x = x_outer; }
        if (y_outer < *min_y) { *min_y = y_outer; }
        if (y_outer > *max_y) { *max_y = y_outer; }

        // Check inner edge
        if (inner_r > 0)
        {
            float x_inner = cx + inner_r * cos_a;
            float y_inner = cy + inner_r * sin_a;
            if (x_inner < *min_x) { *min_x = x_inner; }
            if (x_inner > *max_x) { *max_x = x_inner; }
            if (y_inner < *min_y) { *min_y = y_inner; }
            if (y_inner > *max_y) { *max_y = y_inner; }
        }
    }

    // Check cardinal directions if they fall within the arc
    float cardinal_angles[] = {0, 90, 180, 270};
    for (int i = 0; i < 4; i++)
    {
        float angle = cardinal_angles[i];
        bool in_arc = false;

        if (start_angle <= end_angle)
        {
            in_arc = (angle >= start_angle && angle <= end_angle);
        }
        else
        {
            in_arc = (angle >= start_angle || angle <= end_angle);
        }

        if (in_arc)
        {
            float rad = angle * 3.14159265f / 180.0f;
            float x = cx + outer_r * cosf(rad);
            float y = cy + outer_r * sinf(rad);
            if (x < *min_x) { *min_x = x; }
            if (x > *max_x) { *max_x = x; }
            if (y < *min_y) { *min_y = y; }
            if (y > *max_y) { *max_y = y; }
        }
    }
}

static void gui_arc_group_input_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
}

static void gui_arc_group_prepare(gui_arc_group_t *this)
{
    if (this->buffer_valid)
    {
        return;  // Already rendered
    }

    gui_obj_t *obj = (gui_obj_t *)this;

    // Initialize matrix
    if (obj->matrix == NULL)
    {
        obj->matrix = gui_malloc(sizeof(gui_matrix_t));
        GUI_ASSERT(obj->matrix != NULL);
        matrix_identity(obj->matrix);
    }

    // Optimize buffer size on first allocation based on actual arcs
    if (this->pixel_buffer == NULL && this->arc_count > 0)
    {
        float min_x = 1e9f, max_x = -1e9f, min_y = 1e9f, max_y = -1e9f;

        // Calculate bounding box for all arcs
        for (int i = 0; i < this->arc_count; i++)
        {
            arc_def_t *arc = &this->arcs[i];
            float arc_min_x, arc_max_x, arc_min_y, arc_max_y;

            calculate_arc_bbox(arc->cx, arc->cy, arc->radius, arc->line_width,
                               arc->start_angle, arc->end_angle,
                               &arc_min_x, &arc_max_x, &arc_min_y, &arc_max_y);
            if (arc_min_x < min_x) { min_x = arc_min_x; }
            if (arc_max_x > max_x) { max_x = arc_max_x; }
            if (arc_min_y < min_y) { min_y = arc_min_y; }
            if (arc_max_y > max_y) { max_y = arc_max_y; }
        }

        // Calculate optimized buffer size
        int optimized_w = (int)(max_x - min_x) + 4;
        int optimized_h = (int)(max_y - min_y) + 4;


        // Use optimized size if it saves memory (either dimension smaller)
        int original_pixels = this->buffer_w * this->buffer_h;
        int optimized_pixels = optimized_w * optimized_h;

        if (optimized_pixels < original_pixels)
        {
            // Adjust arc coordinates to be relative to the optimized buffer
            float offset_x = min_x;
            float offset_y = min_y;

            for (int i = 0; i < this->arc_count; i++)
            {
                this->arcs[i].cx -= offset_x;
                this->arcs[i].cy -= offset_y;
            }

            // Update widget position to compensate for the offset
            gui_obj_t *obj = (gui_obj_t *)this;
            obj->x += (int)offset_x;
            obj->y += (int)offset_y;

            this->buffer_w = optimized_w;
            this->buffer_h = optimized_h;
        }
    }

    // Allocate buffer if needed
    int bytes_per_pixel = 4;
    uint32_t required_size = (uint32_t)(this->buffer_w * this->buffer_h * bytes_per_pixel +
                                        sizeof(gui_rgb_data_head_t));

    if (this->pixel_buffer == NULL || this->buffer_size < required_size)
    {
        if (this->pixel_buffer != NULL)
        {
            gui_free(this->pixel_buffer);
        }

        this->pixel_buffer = (uint8_t *)gui_malloc(required_size);
        if (this->pixel_buffer == NULL)
        {
            return;
        }
        this->buffer_size = required_size;
    }

    // Allocate draw_img if needed
    if (this->draw_img == NULL)
    {
        this->draw_img = gui_malloc(sizeof(draw_img_t));
        if (this->draw_img == NULL)
        {
            return;
        }
        memset(this->draw_img, 0x00, sizeof(draw_img_t));
    }

    // Setup draw_img
    this->draw_img->data = this->pixel_buffer;
    this->draw_img->blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img->opacity_value = this->opacity_value;
    this->draw_img->high_quality = 0;

    // Setup image header
    gui_rgb_data_head_t *img_header = (gui_rgb_data_head_t *)this->pixel_buffer;
    set_img_header(img_header, (uint16_t)this->buffer_w, (uint16_t)this->buffer_h);

    // Clear pixel data
    uint8_t *pixel_data = this->pixel_buffer + sizeof(gui_rgb_data_head_t);
    memset(pixel_data, 0x00, (size_t)(this->buffer_w * this->buffer_h * 4));

    // Initialize draw context
    init_draw_context(&this->draw_ctx, pixel_data, this->buffer_w, this->buffer_h,
                      PIXEL_FORMAT_ARGB8888);
    this->draw_ctx.enable_aa = true;
    this->draw_ctx.clip_rect.x = 0;
    this->draw_ctx.clip_rect.y = 0;
    this->draw_ctx.clip_rect.w = this->buffer_w;
    this->draw_ctx.clip_rect.h = this->buffer_h;

    // Render all arcs to buffer
    for (int i = 0; i < this->arc_count; i++)
    {
        arc_def_t *arc = &this->arcs[i];
        this->draw_ctx.gradient = arc->gradient;

        if (arc->use_gradient && arc->gradient != NULL)
        {
            draw_arc_df_aa_gradient(&this->draw_ctx,
                                    arc->cx, arc->cy,
                                    arc->radius,
                                    arc->line_width,
                                    arc->start_angle,
                                    arc->end_angle,
                                    arc->gradient);
        }
        else
        {
            draw_arc_df_aa(&this->draw_ctx,
                           arc->cx, arc->cy,
                           arc->radius,
                           arc->line_width,
                           arc->start_angle,
                           arc->end_angle,
                           arc->color.color.argb_full);
        }
    }

    this->buffer_valid = true;
}

static void gui_arc_group_draw(gui_arc_group_t *this)
{
    if (this->draw_img != NULL && this->buffer_valid)
    {
        gui_obj_t *obj = (gui_obj_t *)this;

        if (obj->matrix != NULL)
        {
            memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
        }
        else
        {
            matrix_identity(&this->draw_img->matrix);
        }

        memcpy(&this->draw_img->inverse, &this->draw_img->matrix, sizeof(struct gui_matrix));
        matrix_inverse(&this->draw_img->inverse);

        draw_img_load_scale(this->draw_img, IMG_SRC_MEMADDR);
        draw_img_new_area(this->draw_img, NULL);

        gui_dispdev_t *dc = gui_get_dc();
        gui_acc_blit_to_dc(this->draw_img, dc, NULL);
    }
}

static void gui_arc_group_end(gui_arc_group_t *this)
{
    if (this->draw_img != NULL && draw_img_acc_end_cb != NULL)
    {
        draw_img_acc_end_cb(this->draw_img);
    }
}

static void gui_arc_group_destroy(gui_arc_group_t *this)
{
    if (this->pixel_buffer != NULL)
    {
        gui_free(this->pixel_buffer);
        this->pixel_buffer = NULL;
    }

    if (this->draw_img != NULL)
    {
        gui_free(this->draw_img);
        this->draw_img = NULL;
    }

    // Free gradients
    for (int i = 0; i < this->arc_count; i++)
    {
        if (this->arcs[i].gradient != NULL)
        {
            gui_free(this->arcs[i].gradient);
            this->arcs[i].gradient = NULL;
        }
    }

    this->buffer_valid = false;
}

static void gui_arc_group_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_arc_group_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_arc_group_prepare((gui_arc_group_t *)obj);
            break;
        case OBJ_DRAW:
            gui_arc_group_draw((gui_arc_group_t *)obj);
            break;
        case OBJ_END:
            gui_arc_group_end((gui_arc_group_t *)obj);
            break;
        case OBJ_DESTROY:
            gui_arc_group_destroy((gui_arc_group_t *)obj);
            break;
        default:
            break;
        }
    }
}

gui_arc_group_t *gui_arc_group_create(void *parent, const char *name,
                                      int x, int y, int w, int h)
{
    GUI_ASSERT(parent != NULL);
    gui_arc_group_t *group = gui_malloc(sizeof(gui_arc_group_t));
    GUI_ASSERT(group != NULL);
    memset(group, 0x00, sizeof(gui_arc_group_t));

    group->opacity_value = 255;
    group->draw_img = NULL;
    group->pixel_buffer = NULL;
    group->buffer_size = 0;
    group->buffer_valid = false;
    group->arc_count = 0;
    group->buffer_w = w;
    group->buffer_h = h;

    gui_obj_ctor((gui_obj_t *)group, parent, name, x, y, w, h);
    GET_BASE(group)->obj_cb = gui_arc_group_cb;
    GET_BASE(group)->has_input_prepare_cb = true;
    GET_BASE(group)->has_prepare_cb = true;
    GET_BASE(group)->has_draw_cb = true;
    GET_BASE(group)->has_end_cb = true;
    GET_BASE(group)->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(group)->child_list));
    if ((GET_BASE(group)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(group)->parent)->child_list),
                               &(GET_BASE(group)->brother_list));
    }
    GET_BASE(group)->create_done = true;

    return group;
}

int gui_arc_group_add_arc(gui_arc_group_t *group, float cx, float cy,
                          float radius, float start_angle, float end_angle,
                          float line_width, gui_color_t color)
{
    GUI_ASSERT(group != NULL);

    if (group->arc_count >= MAX_ARCS_IN_GROUP)
    {
        return -1;  // Group is full
    }

    int index = group->arc_count;
    arc_def_t *arc = &group->arcs[index];

    arc->cx = cx;
    arc->cy = cy;
    arc->radius = radius;
    arc->start_angle = start_angle;
    arc->end_angle = end_angle;
    arc->line_width = line_width;
    arc->color = color;
    arc->gradient = NULL;
    arc->use_gradient = false;

    group->arc_count++;
    group->buffer_valid = false;

    return index;
}

void gui_arc_group_set_gradient(gui_arc_group_t *group, int arc_index,
                                float start_angle, float end_angle)
{
    GUI_ASSERT(group != NULL);
    GUI_ASSERT(arc_index >= 0 && arc_index < group->arc_count);

    arc_def_t *arc = &group->arcs[arc_index];

    if (arc->gradient == NULL)
    {
        arc->gradient = gui_malloc(sizeof(Gradient));
        GUI_ASSERT(arc->gradient != NULL);
    }

    gradient_init(arc->gradient, GRADIENT_ANGULAR);
    arc->gradient->angular_cx = arc->cx;
    arc->gradient->angular_cy = arc->cy;
    arc->gradient->angular_start = start_angle;
    arc->gradient->angular_end = end_angle;

    arc->use_gradient = true;
    group->buffer_valid = false;
}

void gui_arc_group_add_gradient_stop(gui_arc_group_t *group, int arc_index,
                                     float position, gui_color_t color)
{
    GUI_ASSERT(group != NULL);
    GUI_ASSERT(arc_index >= 0 && arc_index < group->arc_count);

    arc_def_t *arc = &group->arcs[arc_index];

    if (arc->gradient == NULL)
    {
        gui_arc_group_set_gradient(group, arc_index, arc->start_angle, arc->end_angle);
    }

    gradient_add_stop(arc->gradient, position, color.color.argb_full);
    group->buffer_valid = false;
}
