/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_lite_geometry_circle.c
  * @brief lite geometry circle widget
  * @details lite geometry circle widget is used to draw circle shapes on the screen
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
#include "gui_fb.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "gui_lite_geometry_circle.h"

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
static bool init_block_drawing(gui_lite_circle_t *this, gui_obj_t *obj);
static void draw_one_tile(gui_lite_circle_t *this, gui_obj_t *obj,
                          int tile_x, int tile_y, int tile_w, int tile_h);

/** Check if a point is inside the circle's bounding circle */
static bool is_point_in_circle(gui_lite_circle_t *circle, int x, int y)
{
    int dx = x - circle->x;
    int dy = y - circle->y;
    int distance_sq = dx * dx + dy * dy;
    int radius_sq = circle->radius * circle->radius;

    return (distance_sq <= radius_sq);
}

static void gui_lite_circle_input_prepare(gui_obj_t *obj)
{
    gui_lite_circle_t *this = (gui_lite_circle_t *)obj;

    // Check for touch events
    touch_info_t *tp = tp_get_info();
    if (tp->type == TOUCH_SHORT)
    {
        // Convert touch coordinates to widget local coordinates
        int local_x = tp->x - obj->x;
        int local_y = tp->y - obj->y;

        // Check if touch point is inside the arc
        if (is_point_in_circle(this, local_x, local_y))
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
            // gui_log("gui_lite_circle_input_prepare: %d, %d\n", local_x, local_y);
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

static void gui_lite_circle_prepare(gui_lite_circle_t *this)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    if (!init_block_drawing(this, obj))
    {
        return;
    }
}

/** Drawing phase processing */
static void gui_lite_circle_draw(gui_lite_circle_t *this)
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
static void gui_lite_circle_end(gui_lite_circle_t *this)
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

static void gui_lite_circle_destroy(gui_lite_circle_t *this)
{
    GUI_UNUSED(this);
}

static void gui_lite_circle_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_lite_circle_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_lite_circle_prepare((gui_lite_circle_t *)obj);
            break;

        case OBJ_DRAW:
            gui_lite_circle_draw((gui_lite_circle_t *)obj);
            break;

        case OBJ_END:
            gui_lite_circle_end((gui_lite_circle_t *)obj);
            break;

        case OBJ_DESTROY:
            gui_lite_circle_destroy((gui_lite_circle_t *)obj);
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

/** Draw the circle onto the Tile (coordinates are offset from the Tile origin) */
static void draw_circle_to_tile(DrawContext *ctx, gui_lite_circle_t *circle, int ox, int oy)
{
    fill_circle(ctx,
                circle->x - ox,
                circle->y - oy,
                circle->radius,
                circle->color);
}

/** Precise determination of whether circle intersects with a Tile */
static inline bool circle_intersects_tile(const gui_lite_circle_t *circle,
                                          int tile_x, int tile_y, int tile_w, int tile_h)
{
    int circle_left = circle->x - circle->radius;
    int circle_top = circle->y - circle->radius;
    int circle_right = circle->x + circle->radius;
    int circle_bottom = circle->y + circle->radius;
    int circle_width = circle_right - circle_left;
    int circle_height = circle_bottom - circle_top;

    if (!rect_intersects(circle_left, circle_top, circle_width, circle_height,
                         tile_x, tile_y, tile_w, tile_h))
    {
        return false;
    }

    int tile_center_x = tile_x + tile_w / 2;
    int tile_center_y = tile_y + tile_h / 2;

    int dx = abs(circle->x - tile_center_x);
    int dy = abs(circle->y - tile_center_y);

    int half_tile_w = tile_w / 2;
    int half_tile_h = tile_h / 2;
    if (dx <= (circle->radius - half_tile_w) && dy <= (circle->radius - half_tile_h))
    {
        return true;
    }

    int corners[4][2] =
    {
        {tile_x, tile_y},
        {tile_x + tile_w - 1, tile_y},
        {tile_x, tile_y + tile_h - 1},
        {tile_x + tile_w - 1, tile_y + tile_h - 1}
    };

    for (int i = 0; i < 4; i++)
    {
        int corner_x = corners[i][0];
        int corner_y = corners[i][1];
        int dx = corner_x - circle->x;
        int dy = corner_y - circle->y;

        if (dx * dx + dy * dy <= circle->radius * circle->radius)
        {
            return true;
        }
    }

    int circle_points[4][2] =
    {
        {circle->x - circle->radius, circle->y},
        {circle->x + circle->radius, circle->y},
        {circle->x, circle->y - circle->radius},
        {circle->x, circle->y + circle->radius}
    };

    for (int i = 0; i < 4; i++)
    {
        int point_x = circle_points[i][0];
        int point_y = circle_points[i][1];

        if (point_x >= tile_x && point_x < tile_x + tile_w &&
            point_y >= tile_y && point_y < tile_y + tile_h)
        {
            return true;
        }
    }

    return false;
}

/** Initialize the block drawing parameters and Tile buffer */
static bool init_block_drawing(gui_lite_circle_t *this, gui_obj_t *obj)
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
static void draw_one_tile(gui_lite_circle_t *this, gui_obj_t *obj,
                          int tile_x, int tile_y, int tile_w, int tile_h)
{
    gui_rgb_data_head_t *img_header = (gui_rgb_data_head_t *)this->block_buffer;
    set_img_header(img_header, (uint16_t)tile_w, (uint16_t)tile_h);
    uint8_t *pixel_data = this->block_buffer + sizeof(gui_rgb_data_head_t);
    memset(pixel_data, 0x00, (size_t)(tile_w * tile_h * 4));

    // initialize the DrawContext
    init_draw_context(&this->draw_ctx, pixel_data, tile_w, tile_h, this->draw_ctx.format);

    // clip the DrawContext to the Tile area
    this->draw_ctx.clip_rect.x = 0;
    this->draw_ctx.clip_rect.y = 0;
    this->draw_ctx.clip_rect.w = tile_w;
    this->draw_ctx.clip_rect.h = tile_h;

    // draw circle if it intersects with the Tile
    if (circle_intersects_tile(this, tile_x, tile_y, tile_w, tile_h))
    {
        draw_circle_to_tile(&this->draw_ctx, this, tile_x, tile_y);
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
 * @brief Create circle geometry widget
 * @param parent Parent widget
 * @param name Widget name
 * @return gui_lite_circle_t* Created widget pointer
 */
gui_lite_circle_t *gui_lite_circle_create(void *parent, const char *name)
{
    GUI_ASSERT(parent != NULL);
    gui_lite_circle_t *circle = gui_malloc(sizeof(gui_lite_circle_t));
    GUI_ASSERT(circle != NULL);
    memset(circle, 0x00, sizeof(gui_lite_circle_t));

    circle->opacity_value = UINT8_MAX;
    circle->draw_img = NULL;
    circle->block_buffer = NULL;
    circle->use_block_drawing = false;
    circle->draw_ctx.format = PIXEL_FORMAT_ARGB8888;
    circle->draw_ctx.enable_aa = 1;

    gui_obj_ctor((gui_obj_t *)circle, parent, name, 0, 0, 0, 0);
    GET_BASE(circle)->obj_cb = gui_lite_circle_cb;
    GET_BASE(circle)->has_input_prepare_cb = true;
    GET_BASE(circle)->has_prepare_cb = true;
    GET_BASE(circle)->has_draw_cb = true;
    GET_BASE(circle)->has_end_cb = true;
    GET_BASE(circle)->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(circle)->child_list));
    if ((GET_BASE(circle)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(circle)->parent)->child_list),
                               &(GET_BASE(circle)->brother_list));
    }
    GET_BASE(circle)->create_done = true;

    return circle;
}

/**
 * @brief Set circle geometry
 * @param this Pointer to the circle widget
 * @param x Center X coordinate relative to widget
 * @param y Center Y coordinate relative to widget
 * @param radius circle radius
 * @param color circle color
 */
void gui_lite_circle_set_style(gui_lite_circle_t *this,
                               int x, int y,
                               int radius, uint32_t color)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
    this->radius = radius;
    this->color = color;
    this->opacity_value = (color >> 24) & 0xFF;
}

/**
 * @brief Set circle position
 * @param this Pointer to the circle widget
 * @param x Center X coordinate relative to widget
 * @param y Center Y coordinate relative to widget
 */
void gui_lite_circle_set_position(gui_lite_circle_t *this, int x, int y)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
}
void gui_lite_circle_set_radius(gui_lite_circle_t *this, int radius)
{
    GUI_ASSERT(this != NULL);
    this->radius = radius;
}
void gui_lite_circle_set_color(gui_lite_circle_t *this, uint32_t color)
{
    GUI_ASSERT(this != NULL);
    this->color = color;
    this->opacity_value = (color >> 24) & 0xFF;
}
void gui_lite_circle_on_click(gui_lite_circle_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);

}
