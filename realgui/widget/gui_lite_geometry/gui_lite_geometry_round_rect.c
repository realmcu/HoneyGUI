/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_lite_geometry_round_rect.c
  * @brief lite geometry round_rect widget with direct rendering
  * @details round_rect widget using direct rectangle data
  * @author
  * @date 2025/11/26
  * @version 2.0
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
#include "gui_lite_geometry_round_rect.h"

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

/** Check if a point is inside the round_rect */
static bool is_point_in_round_rect(gui_lite_round_rect_t *this, int x, int y)
{
    return (x >= this->x &&
            x <= this->x + this->w &&
            y >= this->y &&
            y <= this->y + this->h);
}
static void gui_lite_round_rect_input_prepare(gui_obj_t *obj)
{
    gui_lite_round_rect_t *this = (gui_lite_round_rect_t *)obj;

    // Check for touch events
    touch_info_t *tp = tp_get_info();
    if (tp->type == TOUCH_SHORT)
    {
        // Convert touch coordinates to widget local coordinates
        int local_x = tp->x - obj->x;
        int local_y = tp->y - obj->y;

        // Check if touch point is inside the arc
        if (is_point_in_round_rect(this, local_x, local_y))
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
        }
    }
}

/** Set image data header for rectangle */
static void set_rect_header(gui_rgb_data_head_t *head, uint16_t w, uint16_t h, gui_color_t color)
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

    // For rectangle, we can use the color directly in the header
    gui_rect_file_head_t *rect_head = (gui_rect_file_head_t *)head;
    rect_head->color = color;
}

/** Create a rectangle image object */
static draw_img_t *create_rect_img(gui_lite_round_rect_t *this, gui_obj_t *obj,
                                   int x, int y, int w, int h, gui_color_t color)
{
    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL)
    {
        return NULL;
    }
    memset(img, 0x00, sizeof(draw_img_t));

    // Create rectangle data
    gui_rect_file_head_t *rect_data = gui_malloc(sizeof(gui_rect_file_head_t));
    if (rect_data == NULL)
    {
        gui_free(img);
        return NULL;
    }

    set_rect_header((gui_rgb_data_head_t *)rect_data, w, h, color);
    rect_data->color = color;

    img->data = rect_data;
    img->blend_mode = IMG_RECT;  // Use rectangle blend mode for solid color
    img->opacity_value = this->opacity_value;
    img->high_quality = 0;

    // Set up matrix
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate(x, y, &img->matrix);
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}
/** Prepare arc image data for a specific corner */
static void prepare_arc_img(gui_lite_round_rect_t *this, uint8_t *circle_data, int corner_type)
{
    uint32_t *data = (uint32_t *)(circle_data + sizeof(gui_rgb_data_head_t));
    uint16_t img_size = this->radius + 1;

    // Clear the image data
    memset(data, 0, img_size * img_size * 4);

    // Calculate boundary points for anti-aliasing
    float boundary[this->radius];
    memset(boundary, 0, sizeof(boundary));
    for (int i = 1; i <= this->radius; i++)
    {
        float y = i - 0.5 - this->radius;
        boundary[i - 1] = this->radius - sqrtf(this->radius * this->radius - y * y);
    }

    // Fill the corner based on corner type
    for (int i = 1; i <= this->radius; i++)
    {
        int k = i - 1;
        uint16_t right = (int)boundary[k];

        // Fill solid pixels
        if (right < img_size)
        {
            for (int j = right + 2; j < img_size; j++)
            {
                int fill_i, fill_j;
                switch (corner_type)
                {
                case 0: // Top-left
                    fill_i = i;
                    fill_j = j;
                    break;
                case 1: // Top-right
                    fill_i = i;
                    fill_j = img_size - 1 - j;
                    break;
                case 2: // Bottom-right
                    fill_i = img_size - 1 - i;
                    fill_j = img_size - 1 - j;
                    break;
                case 3: // Bottom-left
                    fill_i = img_size - 1 - i;
                    fill_j = j;
                    break;
                default:
                    fill_i = i;
                    fill_j = j;
                    break;
                }
                if (fill_i >= 0 && fill_i < img_size && fill_j >= 0 && fill_j < img_size)
                {
                    data[fill_i * img_size + fill_j] = this->color.color.argb_full;
                }
            }
        }

        // Anti-aliasing edge pixel
        float portion = ceilf(boundary[k]) - boundary[k];
        gui_color_t color = this->color;
        color.color.rgba.a = roundf(portion * color.color.rgba.a);

        int edge_i, edge_j;
        switch (corner_type)
        {
        case 0: // Top-left
            edge_i = i;
            edge_j = right + 1;
            break;
        case 1: // Top-right
            edge_i = i;
            edge_j = img_size - 1 - (right + 1);
            break;
        case 2: // Bottom-right
            edge_i = img_size - 1 - i;
            edge_j = img_size - 1 - (right + 1);
            break;
        case 3: // Bottom-left
            edge_i = img_size - 1 - i;
            edge_j = right + 1;
            break;
        default:
            edge_i = i;
            edge_j = right + 1;
            break;
        }

        if (edge_i >= 0 && edge_i < img_size && edge_j >= 0 && edge_j < img_size)
        {
            data[edge_i * img_size + edge_j] = color.color.argb_full;
        }
    }
}

/** Create corner image for specific corner */
static draw_img_t *create_corner_img(gui_lite_round_rect_t *this, gui_obj_t *obj,
                                     int corner_idx, int x, int y)
{
    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL)
    {
        return NULL;
    }
    memset(img, 0x00, sizeof(draw_img_t));

    // Create dedicated image buffer for this corner
    int size = this->radius + 1;
    uint32_t buffer_size = size * size * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *circle_data = gui_malloc(buffer_size);
    if (circle_data == NULL)
    {
        gui_free(img);
        return NULL;
    }
    memset(circle_data, 0x00, buffer_size);

    // Set up image header
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)circle_data;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = size;
    head->h = size;
    head->version = 0;
    head->rsvd2 = 0;

    // Prepare the specific corner
    prepare_arc_img(this, circle_data, corner_idx);

    img->data = circle_data;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = this->opacity_value;
    img->high_quality = 1;

    // Set up transformation matrix
    memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    matrix_translate(x, y, &img->matrix);

    // No rotation needed since we're generating each corner separately
    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

static void gui_lite_round_rect_prepare(gui_obj_t *obj)
{
    gui_lite_round_rect_t *this = (gui_lite_round_rect_t *)obj;

    int radius = this->radius;
    int width = this->w;
    int height = this->h;
    int base_x = this->x;
    int base_y = this->y;
    int inset = 0;

    // Create rectangle parts
    // corner_idx: 0=top-left, 1=top-right, 2=bottom-right, 3=bottom-left
    if (this->color.color.rgba.a == 255)
    {

        this->rect_0 = create_rect_img(this, obj,
                                       base_x + radius, base_y + 1,
                                       width - 2 * radius, radius,
                                       this->color);

        this->rect_1 = create_rect_img(this, obj,
                                       base_x + 1, base_y + radius,
                                       width - 1, height - 2 * radius + 1,
                                       this->color);

        this->rect_2 = create_rect_img(this, obj,
                                       base_x + radius, base_y + height - radius,
                                       width - 2 * radius, radius,
                                       this->color);
        this->circle_00 = create_corner_img(this, obj, 0, base_x, base_y);
        this->circle_01 = create_corner_img(this, obj, 1, base_x + width - radius, base_y);
        this->circle_10 = create_corner_img(this, obj, 3, base_x, base_y + height - radius);
        this->circle_11 = create_corner_img(this, obj, 2, base_x + width - radius,
                                            base_y + height - radius);

    }
    else
    {
        this->rect_0 = create_rect_img(this, obj,
                                       base_x + radius + inset, base_y,
                                       width - 2 * (radius + inset), radius,
                                       this->color);

        this->rect_1 = create_rect_img(this, obj,
                                       base_x, base_y + radius - inset,
                                       width - 2 * inset, height - 2 * radius + 0 * inset,
                                       this->color);

        this->rect_2 = create_rect_img(this, obj,
                                       base_x + radius + inset, base_y + height - radius - inset,
                                       width - 2 * (radius + inset), radius,
                                       this->color);
        this->circle_00 = create_corner_img(this, obj, 0, base_x - 1, base_y - 1);
        this->circle_01 = create_corner_img(this, obj, 1, base_x + width - radius, base_y - 1);
        this->circle_10 = create_corner_img(this, obj, 3, base_x - 1, base_y + height - radius);
        this->circle_11 = create_corner_img(this, obj, 2, base_x + width - radius,
                                            base_y + height - radius);
    }
    gui_fb_change();
}

/** Drawing phase processing */
static void gui_lite_round_rect_draw(gui_obj_t *obj)
{
    gui_lite_round_rect_t *this = (gui_lite_round_rect_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    // Draw all parts
    if (this->rect_0 != NULL) { gui_acc_blit_to_dc(this->rect_0, dc, NULL); }
    if (this->rect_1 != NULL) { gui_acc_blit_to_dc(this->rect_1, dc, NULL); }
    if (this->rect_2 != NULL) { gui_acc_blit_to_dc(this->rect_2, dc, NULL); }
    if (this->circle_00 != NULL) { gui_acc_blit_to_dc(this->circle_00, dc, NULL); }
    if (this->circle_01 != NULL) { gui_acc_blit_to_dc(this->circle_01, dc, NULL); }
    if (this->circle_10 != NULL) { gui_acc_blit_to_dc(this->circle_10, dc, NULL); }
    if (this->circle_11 != NULL) { gui_acc_blit_to_dc(this->circle_11, dc, NULL); }
}

/** End phase processing */
static void gui_lite_round_rect_end(gui_obj_t *obj)
{
    gui_lite_round_rect_t *this = (gui_lite_round_rect_t *)obj;

    // Free all image objects and their data
#define FREE_IMG(img) \
    if (img != NULL) { \
        if (draw_img_acc_end_cb != NULL) { \
            draw_img_acc_end_cb(img); \
        } \
        if (img->data != NULL) { \
            gui_free(img->data); \
        } \
        gui_free(img); \
        img = NULL; \
    }

    FREE_IMG(this->rect_0);
    FREE_IMG(this->rect_1);
    FREE_IMG(this->rect_2);
    FREE_IMG(this->circle_00);
    FREE_IMG(this->circle_01);
    FREE_IMG(this->circle_10);
    FREE_IMG(this->circle_11);

#undef FREE_IMG
}

static void gui_lite_round_rect_destroy(gui_obj_t *obj)
{
    gui_lite_round_rect_t *this = (gui_lite_round_rect_t *)obj;
    GUI_UNUSED(this);
    // Resources are cleaned up in end callback
}

static void gui_lite_round_rect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_lite_round_rect_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_lite_round_rect_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_lite_round_rect_draw(obj);
            break;

        case OBJ_END:
            gui_lite_round_rect_end(obj);
            break;

        case OBJ_DESTROY:
            gui_lite_round_rect_destroy(obj);
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_lite_round_rect_t *gui_lite_round_rect_create(void *parent, const char *name)
{
    GUI_ASSERT(parent != NULL);
    gui_lite_round_rect_t *round_rect = gui_malloc(sizeof(gui_lite_round_rect_t));
    GUI_ASSERT(round_rect != NULL);
    memset(round_rect, 0x00, sizeof(gui_lite_round_rect_t));

    round_rect->opacity_value = UINT8_MAX;
    round_rect->rect_0 = NULL;
    round_rect->rect_1 = NULL;
    round_rect->rect_2 = NULL;
    round_rect->circle_00 = NULL;
    round_rect->circle_01 = NULL;
    round_rect->circle_10 = NULL;
    round_rect->circle_11 = NULL;

    gui_obj_ctor((gui_obj_t *)round_rect, parent, name, 0, 0, 0, 0);
    GET_BASE(round_rect)->obj_cb = gui_lite_round_rect_cb;
    GET_BASE(round_rect)->has_input_prepare_cb = true;
    GET_BASE(round_rect)->has_prepare_cb = true;
    GET_BASE(round_rect)->has_draw_cb = true;
    GET_BASE(round_rect)->has_end_cb = true;
    GET_BASE(round_rect)->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(round_rect)->child_list));
    if ((GET_BASE(round_rect)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(round_rect)->parent)->child_list),
                               &(GET_BASE(round_rect)->brother_list));
    }
    GET_BASE(round_rect)->create_done = true;

    return round_rect;
}

/**
 * @brief Set round_rect geometry
 * @param this Pointer to the round_rect widget
 * @param x Center X coordinate relative to widget
 * @param y Center Y coordinate relative to widget
 * @param w round_rect width
 * @param h round_rect height
 * @param radius round_rect radius
 * @param color round_rect color
 */
void gui_lite_round_rect_set_style(gui_lite_round_rect_t *this,
                                   int x, int y, int w, int h,
                                   int radius, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->radius = radius;
    this->color = color;
    this->opacity_value = color.color.rgba.a;
}

void gui_lite_round_rect_set_position(gui_lite_round_rect_t *this, int x, int y)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
}

void gui_lite_round_rect_set_size(gui_lite_round_rect_t *this, int w, int h)
{
    GUI_ASSERT(this != NULL);
    this->w = w;
    this->h = h;
}

void gui_lite_round_rect_set_radius(gui_lite_round_rect_t *this, int radius)
{
    GUI_ASSERT(this != NULL);
    this->radius = radius;
}

void gui_lite_round_rect_set_color(gui_lite_round_rect_t *this, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    this->color = color;
    this->opacity_value = color.color.rgba.a;
}

void gui_lite_round_rect_on_click(gui_lite_round_rect_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}