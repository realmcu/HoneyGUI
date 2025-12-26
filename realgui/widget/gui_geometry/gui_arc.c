/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_arc.c
  * @brief arc widget
  * @details arc widget is used to draw arc shapes on the screen
  * @author
  * @date 2025/11/07
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
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
#include "tp_algo.h"
#include "lite_geometry.h"
#include "gui_arc.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                           Constants
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/

/*============================================================================*
 *                            Variables
 *============================================================================*/

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static bool init_arc_buffer(gui_arc_t *this);
static void render_arc_to_buffer(gui_arc_t *this);
static bool is_arc_dirty(gui_arc_t *this);

/** Touch input preparation - simplified version like round_rect */
static void gui_arc_input_prepare(gui_obj_t *obj)
{
    // Arc widget uses the bounding box for touch detection
    // The event is already enabled in prepare phase
    // System will handle touch detection based on widget's x, y, w, h
    GUI_UNUSED(obj);
}

/** Check if arc parameters have changed */
static bool is_arc_dirty(gui_arc_t *this)
{
    return (this->x != this->cached_x ||
            this->y != this->cached_y ||
            this->radius != this->cached_radius ||
            this->start_angle != this->cached_start_angle ||
            this->end_angle != this->cached_end_angle ||
            this->line_width != this->cached_line_width ||
            this->color.color.argb_full != this->cached_color.color.argb_full);
}

/** Update cached parameters */
static void update_cache(gui_arc_t *this)
{
    this->cached_x = this->x;
    this->cached_y = this->y;
    this->cached_radius = this->radius;
    this->cached_start_angle = this->start_angle;
    this->cached_end_angle = this->end_angle;
    this->cached_line_width = this->line_width;
    this->cached_color = this->color;
}

/** Set image data header */
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

static void gui_arc_prepare(gui_arc_t *this)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    // Initialize obj->matrix if not already done
    if (obj->matrix == NULL)
    {
        obj->matrix = gui_malloc(sizeof(gui_matrix_t));
        GUI_ASSERT(obj->matrix != NULL);
        matrix_identity(obj->matrix);
    }

    // Apply transformations (like gui_img_prepare does)
    float center_x = obj->w / 2.0f;
    float center_y = obj->h / 2.0f;

    // Apply offset first
    matrix_translate(this->offset_x, this->offset_y, obj->matrix);
    // Translate to center
    matrix_translate(center_x, center_y, obj->matrix);
    // Apply scale
    matrix_scale(this->scale_x, this->scale_y, obj->matrix);
    // Apply rotation
    matrix_rotate(this->degrees, obj->matrix);
    // Translate back
    matrix_translate(-center_x, -center_y, obj->matrix);

    gui_obj_enable_event(GUI_BASE(this), GUI_EVENT_TOUCH_CLICKED);
    // Check if we need to re-render
    if (!this->buffer_valid || is_arc_dirty(this))
    {
        if (!init_arc_buffer(this))
        {
            gui_log("[GUI MODULE]Arc prepare: init_arc_buffer failed!\n");
            return;
        }
        render_arc_to_buffer(this);
        update_cache(this);
        this->buffer_valid = true;
    }
}

/** Drawing phase processing */
static void gui_arc_draw(gui_arc_t *this)
{
    if (this->draw_img != NULL && this->buffer_valid)
    {
        gui_obj_t *obj = (gui_obj_t *)this;

        // The arc is rendered at the center of the buffer
        // We just need to position the buffer at the widget's location
        // No offset needed since arc->x, arc->y are already relative to the bounding box

        // Initialize with identity matrix or copy user's matrix
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
    // Free cached resources
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

    this->buffer_valid = false;
}

static void gui_arc_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_arc_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_arc_prepare((gui_arc_t *)obj);
            break;

        case OBJ_DRAW:
            gui_arc_draw((gui_arc_t *)obj);
            break;

        case OBJ_END:
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

/** Initialize arc buffer - allocate once and reuse */
static bool init_arc_buffer(gui_arc_t *this)
{
    // Calculate required buffer size
    float outer_r = this->radius + this->line_width / 2 + 2;
    int buffer_w = (int)(outer_r * 2) + 4;
    int buffer_h = (int)(outer_r * 2) + 4;

    int bytes_per_pixel = 4;
    uint32_t required_size = (uint32_t)(buffer_w * buffer_h * bytes_per_pixel + sizeof(
                                            gui_rgb_data_head_t));

    // Allocate or reallocate buffer if needed
    if (this->pixel_buffer == NULL || this->buffer_size < required_size)
    {
        if (this->pixel_buffer != NULL)
        {
            gui_free(this->pixel_buffer);
        }

        this->pixel_buffer = (uint8_t *)gui_malloc(required_size);
        if (this->pixel_buffer == NULL)
        {
            return false;
        }
        this->buffer_size = required_size;
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
    this->draw_img->data = this->pixel_buffer;
    this->draw_img->blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img->opacity_value = this->opacity_value;
    this->draw_img->high_quality = 0;

    return true;
}

/** Render arc to buffer - called only when parameters change */
static void render_arc_to_buffer(gui_arc_t *this)
{
    float outer_r = this->radius + this->line_width / 2 + 2;
    int buffer_w = (int)(outer_r * 2) + 4;

    // Setup image header
    gui_rgb_data_head_t *img_header = (gui_rgb_data_head_t *)this->pixel_buffer;
    set_img_header(img_header, (uint16_t)buffer_w, (uint16_t)buffer_w);

    // Clear pixel data
    uint8_t *pixel_data = this->pixel_buffer + sizeof(gui_rgb_data_head_t);
    memset(pixel_data, 0x00, (size_t)(buffer_w * buffer_w * 4));

    // Initialize draw context
    init_draw_context(&this->draw_ctx, pixel_data, buffer_w, buffer_w, PIXEL_FORMAT_ARGB8888);
    this->draw_ctx.enable_aa = true;
    this->draw_ctx.clip_rect.x = 0;
    this->draw_ctx.clip_rect.y = 0;
    this->draw_ctx.clip_rect.w = buffer_w;
    this->draw_ctx.clip_rect.h = buffer_w;

    // Draw arc at center of buffer
    float center_x = buffer_w / 2.0f;

    // Draw arc using optimized SDF-based algorithm
    draw_arc_df_aa(&this->draw_ctx,
                   center_x,
                   center_x,
                   this->radius,
                   this->line_width,
                   this->start_angle,
                   this->end_angle,
                   this->color.color.argb_full);
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

    arc->opacity_value = color.color.rgba.a;
    arc->draw_img = NULL;
    arc->pixel_buffer = NULL;
    arc->buffer_size = 0;
    arc->buffer_valid = false;
    arc->draw_ctx.format = PIXEL_FORMAT_ARGB8888;
    arc->draw_ctx.enable_aa = true;

    // Calculate widget bounding box
    float outer_r = radius + line_width / 2 + 2;
    int box_size = (int)(outer_r * 2) + 4;
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

    // Initialize cache as invalid
    arc->cached_x = -1;
    arc->cached_y = -1;
    arc->cached_radius = -1;
    arc->cached_start_angle = -1.0f;
    arc->cached_end_angle = -1.0f;
    arc->cached_line_width = -1.0f;
    arc->cached_color.color.argb_full = 0;

    gui_obj_ctor((gui_obj_t *)arc, parent, name, box_x, box_y, box_size, box_size);
    GET_BASE(arc)->obj_cb = gui_arc_cb;
    GET_BASE(arc)->has_input_prepare_cb = true;
    GET_BASE(arc)->has_prepare_cb = true;
    GET_BASE(arc)->has_draw_cb = true;
    GET_BASE(arc)->has_end_cb = true;
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


void gui_arc_set_position(gui_arc_t *this, int x, int y)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
    this->buffer_valid = false;
}

void gui_arc_set_radius(gui_arc_t *this, int radius)
{
    GUI_ASSERT(this != NULL);
    this->radius = radius;
    this->buffer_valid = false;
}
void gui_arc_set_opacity(gui_arc_t *this, uint8_t opacity)
{
    GUI_ASSERT(this != NULL);
    this->opacity_value = opacity;
    this->buffer_valid = false;
}
void gui_arc_set_color(gui_arc_t *this, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    this->color = color;
    this->opacity_value = color.color.rgba.a;
    this->buffer_valid = false;
}

void gui_arc_set_start_angle(gui_arc_t *this, float start_angle)
{
    GUI_ASSERT(this != NULL);
    this->start_angle = start_angle;
    this->buffer_valid = false;
}

void gui_arc_set_end_angle(gui_arc_t *this, float end_angle)
{
    GUI_ASSERT(this != NULL);
    this->end_angle = end_angle;
    this->buffer_valid = false;
}

void gui_arc_set_line_width(gui_arc_t *this, float line_width)
{
    GUI_ASSERT(this != NULL);
    this->line_width = line_width;
    this->buffer_valid = false;
}

void gui_arc_on_click(gui_arc_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}

void gui_arc_rotate(gui_arc_t *this, float degrees)
{
    GUI_ASSERT(this != NULL);
    this->degrees = degrees;
    this->buffer_valid = false;
}

void gui_arc_scale(gui_arc_t *this, float scale_x, float scale_y)
{
    GUI_ASSERT(this != NULL);
    this->scale_x = scale_x;
    this->scale_y = scale_y;
    this->buffer_valid = false;
}

void gui_arc_translate(gui_arc_t *this, float tx, float ty)
{
    GUI_ASSERT(this != NULL);
    this->offset_x = tx;
    this->offset_y = ty;
    this->buffer_valid = false;
}
