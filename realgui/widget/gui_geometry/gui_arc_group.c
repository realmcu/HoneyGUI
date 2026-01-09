/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_arc_group.c
  * @brief Arc group widget for batch rendering
  * @details Combines multiple static arcs into one buffer for optimized rendering
  * @author
  * @date 2025/01/09
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
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

#include "gui_api_os.h"
    gui_log("[GUI MODULE]Arc group rendering %d arcs in one buffer\n", this->arc_count);

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

int gui_arc_group_add_arc(gui_arc_group_t *this, float cx, float cy,
                          float radius, float start_angle, float end_angle,
                          float line_width, gui_color_t color)
{
    GUI_ASSERT(this != NULL);

    if (this->arc_count >= MAX_ARCS_IN_GROUP)
    {
        return -1;  // Group is full
    }

    int index = this->arc_count;
    arc_def_t *arc = &this->arcs[index];

    arc->cx = cx;
    arc->cy = cy;
    arc->radius = radius;
    arc->start_angle = start_angle;
    arc->end_angle = end_angle;
    arc->line_width = line_width;
    arc->color = color;
    arc->gradient = NULL;
    arc->use_gradient = false;

    this->arc_count++;
    this->buffer_valid = false;

    return index;
}

void gui_arc_group_set_gradient(gui_arc_group_t *this, int arc_index,
                                float start_angle, float end_angle)
{
    GUI_ASSERT(this != NULL);
    GUI_ASSERT(arc_index >= 0 && arc_index < this->arc_count);

    arc_def_t *arc = &this->arcs[arc_index];

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
    this->buffer_valid = false;
}

void gui_arc_group_add_gradient_stop(gui_arc_group_t *this, int arc_index,
                                     float position, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    GUI_ASSERT(arc_index >= 0 && arc_index < this->arc_count);

    arc_def_t *arc = &this->arcs[arc_index];

    if (arc->gradient == NULL)
    {
        gui_arc_group_set_gradient(this, arc_index, arc->start_angle, arc->end_angle);
    }

    gradient_add_stop(arc->gradient, position, color.color.argb_full);
    this->buffer_valid = false;
}
