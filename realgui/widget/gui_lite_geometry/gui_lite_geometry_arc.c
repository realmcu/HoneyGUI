/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_lite_geometry_arc.c
  * @brief lite geometry arc widget
  * @details lite geometry arc widget is used to draw arc shapes on the screen
  * @author
  * @date 2025/11/07
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
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
#include "gui_lite_geometry_arc.h"

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
static bool init_block_drawing(gui_lite_arc_t *this, gui_obj_t *obj);
static void draw_one_tile(gui_lite_arc_t *this, gui_obj_t *obj,
                          int tile_x, int tile_y, int tile_w, int tile_h);
static bool is_angle_in_range(gui_lite_arc_t *arc, float dx, float dy, float distance);
/** Check if a point is inside the arc's bounding circle */
static bool is_point_in_arc(gui_lite_arc_t *arc, int x, int y)
{
    float inner_radius = arc->radius - arc->line_width / 2;
    float outer_radius = arc->radius + arc->line_width / 2;

    if (x < arc->x - outer_radius || x > arc->x + outer_radius ||
        y < arc->y - outer_radius || y > arc->y + outer_radius)
    {
        return false;
    }

    float dx = x - arc->x;
    float dy = y - arc->y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance < inner_radius || distance > outer_radius)
    {
        return false;
    }

    return is_angle_in_range(arc, dx, dy, distance);
}
static bool is_angle_in_range(gui_lite_arc_t *arc, float dx, float dy, float distance)
{
    if (distance == 0) { return false; }

    float angle = atan2f(dy, dx) * 180.0f / M_PI;
    if (angle < 0) { angle += 360.0f; }

    float start_angle = fmodf(arc->start_angle, 360.0f);
    float end_angle = fmodf(arc->end_angle, 360.0f);

    if (fabsf(end_angle - start_angle) >= 360.0f - 0.1f)
    {
        return true;
    }
    if (start_angle <= end_angle)
    {
        return (angle >= start_angle && angle <= end_angle);
    }
    else
    {
        return (angle >= start_angle || angle <= end_angle);
    }
}

/** Touch input preparation */
static void gui_lite_arc_input_prepare(gui_obj_t *obj)
{
    gui_lite_arc_t *this = (gui_lite_arc_t *)obj;

    // Check for touch events
    touch_info_t *tp = tp_get_info();
    if (tp->type == TOUCH_SHORT)
    {
        // Convert touch coordinates to widget local coordinates
        int local_x = tp->x - obj->x;
        int local_y = tp->y - obj->y;

        // Check if touch point is inside the arc
        if (is_point_in_arc(this, local_x, local_y))
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
            // gui_log("gui_lite_arc_input_prepare: %d, %d\n", local_x, local_y);
        }

    }

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

static void gui_lite_arc_prepare(gui_lite_arc_t *this)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    if (!init_block_drawing(this, obj))
    {
        return;
    }
}

/** Drawing phase processing */
static void gui_lite_arc_draw(gui_lite_arc_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    if (this->use_block_drawing)
    {
        gui_obj_t *obj = (gui_obj_t *)this;
        // tile per block
        for (uint16_t by = 0; by < this->block_rows; by++)
        {
            for (uint16_t bx = 0; bx < this->block_cols; bx++)
            {
                int tile_x = bx * BLOCK_SIZE;
                int tile_y = by * BLOCK_SIZE;
                int tile_w = LG_MIN(BLOCK_SIZE, this->base.w - tile_x);
                int tile_h = LG_MIN(BLOCK_SIZE, this->base.h - tile_y);
                if (tile_w <= 0 || tile_h <= 0) { continue; }

                draw_one_tile(this, obj, tile_x, tile_y, tile_w, tile_h);
            }
        }
        return;
    }

    if (this->draw_img != NULL)
    {
        gui_acc_blit_to_dc(this->draw_img, dc, NULL);
    }
}

/** End phase processing */
static void gui_lite_arc_end(gui_lite_arc_t *this)
{
    // Clean up drawing image object
    if (this->draw_img != NULL)
    {
        if (draw_img_acc_end_cb != NULL)
        {
            draw_img_acc_end_cb(this->draw_img);
        }

        // Free pixel buffer (stored in draw_img->data)
        if (this->draw_img->data != NULL)
        {
            gui_free(this->draw_img->data);
            this->draw_img->data = NULL;
        }

        gui_free(this->draw_img);
        this->draw_img = NULL;
    }
}

static void gui_lite_arc_destroy(gui_lite_arc_t *this)
{
    GUI_UNUSED(this);
}

static void gui_lite_arc_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_lite_arc_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_lite_arc_prepare((gui_lite_arc_t *)obj);
            break;

        case OBJ_DRAW:
            gui_lite_arc_draw((gui_lite_arc_t *)obj);
            break;

        case OBJ_END:
            gui_lite_arc_end((gui_lite_arc_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_lite_arc_destroy((gui_lite_arc_t *)obj);
            break;

        default:
            break;
        }
    }
}

/** Check if two rectangles intersect (coordinates are offset from the Tile origin) */
static inline bool rect_intersects(int ax, int ay, int aw, int ah,
                                   int bx, int by, int bw, int bh)
{
    return !(ax + aw <= bx || bx + bw <= ax || ay + ah <= by || by + bh <= ay);
}

/** Draw the arc onto the Tile (coordinates are offset from the Tile origin) */
static void draw_arc_to_tile(DrawContext *ctx, gui_lite_arc_t *arc, int ox, int oy)
{
    draw_arc_df_aa(ctx,
                   arc->x - ox,
                   arc->y - oy,
                   arc->radius,
                   arc->line_width,
                   arc->start_angle,
                   arc->end_angle,
                   arc->color);
}

/** Rough determination of whether arc intersects with a Tile (for cropping to enhance efficiency) */
static inline bool arc_intersects_tile(const gui_lite_arc_t *arc,
                                       int tile_x, int tile_y, int tile_w, int tile_h)
{
    int r = arc->radius + (int)(arc->line_width);
    return rect_intersects(arc->x - r, arc->y - r, 2 * r, 2 * r,
                           tile_x, tile_y, tile_w, tile_h);
}

/** Initialize the block drawing parameters and Tile buffer */
static bool init_block_drawing(gui_lite_arc_t *this, gui_obj_t *obj)
{
    int bytes_per_pixel = 4;
    this->block_buffer_size = (uint32_t)(BLOCK_SIZE * BLOCK_SIZE * bytes_per_pixel + sizeof(
                                             gui_rgb_data_head_t));
    this->block_buffer = (uint8_t *)gui_malloc(this->block_buffer_size);
    if (this->block_buffer == NULL)
    {
        return false;
    }

    this->draw_img = gui_malloc(sizeof(draw_img_t));
    if (this->draw_img == NULL)
    {
        gui_free(this->block_buffer);
        this->block_buffer = NULL;
        return false;
    }
    memset(this->draw_img, 0x00, sizeof(draw_img_t));
    this->draw_img->data = this->block_buffer;
    this->draw_img->blend_mode = IMG_SRC_OVER_MODE;
    this->draw_img->opacity_value = this->opacity_value;
    this->draw_img->high_quality = 0;

    memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    memcpy(&this->draw_img->inverse, &this->draw_img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&this->draw_img->inverse);

    // calculate block count
    this->block_cols = (uint16_t)((this->base.w + BLOCK_SIZE - 1) / BLOCK_SIZE);
    this->block_rows = (uint16_t)((this->base.h + BLOCK_SIZE - 1) / BLOCK_SIZE);

    this->use_block_drawing = true;
    return true;
}

/** Draw one Tile onto the DrawContext (coordinates are offset from the Tile origin) */
static void draw_one_tile(gui_lite_arc_t *this, gui_obj_t *obj,
                          int tile_x, int tile_y, int tile_w, int tile_h)
{
    gui_rgb_data_head_t *img_header = (gui_rgb_data_head_t *)this->block_buffer;
    set_img_header(img_header, (uint16_t)tile_w, (uint16_t)tile_h);
    uint8_t *pixel_data = this->block_buffer + sizeof(gui_rgb_data_head_t);
    memset(pixel_data, 0x00, (size_t)(tile_w * tile_h * 4));

    // initialize the DrawContext
    init_draw_context(&this->draw_ctx, pixel_data, tile_w, tile_h, this->draw_ctx.format);
    this->draw_ctx.enable_aa = true;

    // clip the DrawContext to the Tile area
    this->draw_ctx.clip_rect.x = 0;
    this->draw_ctx.clip_rect.y = 0;
    this->draw_ctx.clip_rect.w = tile_w;
    this->draw_ctx.clip_rect.h = tile_h;

    // draw arc if it intersects with the Tile
    if (arc_intersects_tile(this, tile_x, tile_y, tile_w, tile_h))
    {
        draw_arc_to_tile(&this->draw_ctx, this, tile_x, tile_y);
    }

    memcpy(&this->draw_img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate((float)tile_x, (float)tile_y, &this->draw_img->matrix);
    memcpy(&this->draw_img->inverse, &this->draw_img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&this->draw_img->inverse);

    draw_img_load_scale(this->draw_img, IMG_SRC_MEMADDR);
    draw_img_new_area(this->draw_img, NULL);

    // submit to the DrawContext
    gui_dispdev_t *dc = gui_get_dc();
    gui_acc_blit_to_dc(this->draw_img, dc, NULL);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Create arc geometry widget
 * @param parent Parent widget
 * @param name Widget name
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @return gui_lite_arc_t* Created widget pointer
 */
gui_lite_arc_t *gui_lite_arc_create(void *parent, const char *name)
{
    GUI_ASSERT(parent != NULL);
    gui_lite_arc_t *arc = gui_malloc(sizeof(gui_lite_arc_t));
    GUI_ASSERT(arc != NULL);
    memset(arc, 0x00, sizeof(gui_lite_arc_t));

    arc->opacity_value = UINT8_MAX;
    arc->draw_img = NULL;
    arc->block_buffer = NULL;
    arc->use_block_drawing = false;
    arc->draw_ctx.format = PIXEL_FORMAT_ARGB8888;
    arc->draw_ctx.enable_aa = true;

    // Initialize arc with default values
    arc->x = 100;
    arc->y = 100;
    arc->radius = LG_MIN(100, 100) / 3;
    arc->start_angle = 0.0f;
    arc->end_angle = 270.0f;
    arc->line_width = 8.0f;
    arc->color = 0xFFFF0000; // Red by default

    gui_obj_ctor((gui_obj_t *)arc, parent, name, 0, 0, 0, 0);
    GET_BASE(arc)->obj_cb = gui_lite_arc_cb;
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

    return arc;
}

/**
 * @brief Set arc geometry
 * @param this Pointer to the arc widget
 * @param x Center X coordinate relative to widget
 * @param y Center Y coordinate relative to widget
 * @param radius Arc radius
 * @param start_angle Start angle in degrees
 * @param end_angle End angle in degrees
 * @param line_width Line width
 * @param color Arc color
 */
void gui_lite_arc_set_style(gui_lite_arc_t *this,
                            int x, int y, int radius,
                            float start_angle, float end_angle,
                            float line_width, uint32_t color)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
    this->radius = radius;
    this->start_angle = start_angle;
    this->end_angle = end_angle;
    this->line_width = line_width;
    this->color = color;
    this->opacity_value = (color >> 24) & 0xFF;
}
/**
 * @brief Move arc geometry
 * @param this Pointer to the arc widget
 * @param x New center X coordinate relative to widget
 * @param y New center Y coordinate relative to widget
 */
void gui_lite_arc_set_position(gui_lite_arc_t *this, int x, int y)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
}
void gui_lite_arc_set_radius(gui_lite_arc_t *this, int radius)
{
    GUI_ASSERT(this != NULL);
    this->radius = radius;
}
void gui_lite_arc_set_color(gui_lite_arc_t *this, uint32_t color)
{
    GUI_ASSERT(this != NULL);
    this->color = color;
    this->opacity_value = (color >> 24) & 0xFF;
}
void gui_lite_arc_set_start_angle(gui_lite_arc_t *this, float start_angle)
{
    GUI_ASSERT(this != NULL);
    this->start_angle = start_angle;
}
void gui_lite_arc_set_end_angle(gui_lite_arc_t *this, float end_angle)
{
    GUI_ASSERT(this != NULL);
    this->end_angle = end_angle;
}
void gui_lite_arc_set_line_width(gui_lite_arc_t *this, float line_width)
{
    GUI_ASSERT(this != NULL);
    this->line_width = line_width;
}
void gui_lite_arc_on_click(gui_lite_arc_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);

}
