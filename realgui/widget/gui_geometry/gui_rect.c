/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_rect.c
  * @brief rect widget with direct rendering
  * @details rect widget using direct rectangle data
  * @author
  * @date 2025/11/26
  * @version 2.0
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
#include "gui_fb.h"
#include "acc_api.h"
#include "tp_algo.h"
#include "gui_rect.h"

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

/** Create a solid color image buffer for better anti-aliasing with transformations */
static uint8_t *create_solid_color_buffer(uint16_t w, uint16_t h, gui_color_t color)
{
    uint32_t buffer_size = w * h * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = gui_malloc(buffer_size);
    if (buffer == NULL) { return NULL; }

    // Set header
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)buffer;
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

    // Fill with solid color
    uint32_t *pixels = (uint32_t *)(buffer + sizeof(gui_rgb_data_head_t));
    uint32_t argb = color.color.argb_full;
    for (int i = 0; i < w * h; i++)
    {
        pixels[i] = argb;
    }

    return buffer;
}

/** Create a rectangle image object */
static void set_rect_img(gui_rounded_rect_t *this, draw_img_t **input_img, int16_t x,
                         int16_t y, uint16_t w, uint16_t h)
{
    gui_obj_t *obj = (gui_obj_t *)this;
    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    memset(img, 0x00, sizeof(draw_img_t));

    // Check if transformation is applied
    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f);

    if (has_transform)
    {
        // Use IMG_SRC_OVER_MODE with actual pixel buffer for anti-aliasing
        img->blend_mode = IMG_SRC_OVER_MODE;
        img->high_quality = 1;
        img->data = create_solid_color_buffer(w, h, this->color);
        if (img->data == NULL)
        {
            gui_free(img);
            *input_img = NULL;
            return;
        }
    }
    else
    {
        // Use IMG_RECT for better performance when no transformation
        img->blend_mode = IMG_RECT;
        gui_rect_file_head_t *rect_data = gui_malloc(sizeof(gui_rect_file_head_t));
        set_rect_header((gui_rgb_data_head_t *)rect_data, w, h, this->color);
        img->data = rect_data;
    }

    img->opacity_value = this->opacity_value;

    // Start with user's transformation matrix if exists, otherwise identity
    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    // Apply translation for this rect part (after user's transformation)
    matrix_translate(x, y, &img->matrix);

    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);
    *input_img = img;
}
/** Prepare arc image data for a specific corner */
static void prepare_arc_img(gui_rounded_rect_t *this, uint8_t *circle_data, int corner_type)
{
    if (this->radius == 0) { return; }
    uint32_t *data = (uint32_t *)(circle_data + sizeof(gui_rgb_data_head_t));
    uint16_t img_size = this->radius + 1;

    // Clear the image data
    memset(data, 0, img_size * img_size * 4);

    // Calculate boundary points for anti-aliasing (including row 0)
    float boundary[img_size];
    memset(boundary, 0, sizeof(boundary));
    for (int i = 0; i < img_size; i++)
    {
        float y = i + 0.5 - this->radius;
        float x_boundary = this->radius - sqrtf(this->radius * this->radius - y * y);
        boundary[i] = (x_boundary < 0) ? 0 : x_boundary;
    }

    // Fill the corner based on corner type
    for (int i = 0; i < img_size; i++)
    {
        uint16_t right = (int)boundary[i];

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
        float portion = ceilf(boundary[i]) - boundary[i];
        gui_color_t color = this->color;
        color.color.rgba.a = (uint8_t)(portion * color.color.rgba.a);

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

/** Create a complete rounded rectangle in a single buffer to avoid seams */
static draw_img_t *create_rounded_rect_buffer(gui_rounded_rect_t *this, gui_obj_t *obj)
{
    int w = this->base.w;
    int h = this->base.h;
    int r = this->radius;

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    // Create pixel buffer
    uint32_t buffer_size = w * h * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = gui_malloc(buffer_size);
    if (buffer == NULL)
    {
        gui_free(img);
        return NULL;
    }
    memset(buffer, 0x00, buffer_size);

    // Set header
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)buffer;
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

    uint32_t *pixels = (uint32_t *)(buffer + sizeof(gui_rgb_data_head_t));
    uint32_t solid_color = this->color.color.argb_full;

    // Fill rounded rectangle with anti-aliasing
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            // Check if pixel is in corner region
            bool in_corner = false;
            float dx = 0, dy = 0;

            // Top-left corner
            if (x < r && y < r)
            {
                dx = r - x - 0.5f;
                dy = r - y - 0.5f;
                in_corner = true;
            }
            // Top-right corner
            else if (x >= w - r && y < r)
            {
                dx = x - (w - r) + 0.5f;
                dy = r - y - 0.5f;
                in_corner = true;
            }
            // Bottom-left corner
            else if (x < r && y >= h - r)
            {
                dx = r - x - 0.5f;
                dy = y - (h - r) + 0.5f;
                in_corner = true;
            }
            // Bottom-right corner
            else if (x >= w - r && y >= h - r)
            {
                dx = x - (w - r) + 0.5f;
                dy = y - (h - r) + 0.5f;
                in_corner = true;
            }

            if (in_corner)
            {
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist <= r - 0.5f)
                {
                    // Fully inside
                    pixels[y * w + x] = solid_color;
                }
                else if (dist < r + 0.5f)
                {
                    // Anti-aliasing edge
                    float alpha = r + 0.5f - dist;
                    gui_color_t color = this->color;
                    color.color.rgba.a = (uint8_t)(alpha * color.color.rgba.a);
                    pixels[y * w + x] = color.color.argb_full;
                }
                // else: outside, leave transparent
            }
            else
            {
                // Inside main rectangle area
                pixels[y * w + x] = solid_color;
            }
        }
    }

    img->data = buffer;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = this->opacity_value;
    img->high_quality = 1;

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

/** Create corner image for specific corner */
static draw_img_t *create_corner_img(gui_rounded_rect_t *this, gui_obj_t *obj,
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

    // Start with user's transformation matrix if exists, otherwise identity
    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    // Apply translation for this corner (after user's transformation)
    matrix_translate(x, y, &img->matrix);

    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

static void gui_rect_prepare(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
    uint8_t last;

    // Initialize obj->matrix if not already done
    if (obj->matrix == NULL)
    {
        obj->matrix = gui_malloc(sizeof(gui_matrix_t));
        GUI_ASSERT(obj->matrix != NULL);
        matrix_identity(obj->matrix);
    }

    // Apply transformations (like gui_img_prepare does)
    float center_x = this->base.w / 2.0f;
    float center_y = this->base.h / 2.0f;

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

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);

    // Check if we need single buffer (for transparency or small size)
    bool need_single_buffer = (this->color.color.rgba.a < 255) ||
                              (this->base.w * this->base.h <= 40000);

    if (this->radius == 0)
    {
        // Simple rectangle without rounded corners
        set_rect_img(this, &this->rect_0, 0, 0, this->base.w, this->base.h);
    }
    else if (need_single_buffer)
    {
        // Use single buffer for transparency or small rectangles to avoid seams
        this->rect_0 = create_rounded_rect_buffer(this, obj);
    }
    else
    {
        // Use optimized multi-part rendering for large opaque rectangles
        // Top rectangle: between the two top corners
        set_rect_img(this, &this->rect_0, \
                     this->radius + 1,  \
                     0,
                     this->base.w - 2 * (this->radius + 1), \
                     this->radius + 1);

        // Middle rectangle: full width, between top and bottom strips
        set_rect_img(this, &this->rect_1, \
                     0, \
                     this->radius + 1, \
                     this->base.w, \
                     this->base.h - 2 * (this->radius + 1));

        // Bottom rectangle: between the two bottom corners
        set_rect_img(this, &this->rect_2, \
                     this->radius + 1,  \
                     this->base.h - this->radius - 1,
                     this->base.w - 2 * (this->radius + 1), \
                     this->radius + 1);

        // Four corners: each is (radius+1) x (radius+1)
        this->circle_00 = create_corner_img(this, obj, 0, 0, 0);
        this->circle_01 = create_corner_img(this, obj, 1, this->base.w - this->radius - 1, 0);
        this->circle_10 = create_corner_img(this, obj, 3, 0, this->base.h - this->radius - 1);
        this->circle_11 = create_corner_img(this, obj, 2, this->base.w - this->radius - 1,
                                            this->base.h - this->radius - 1);
    }
    last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_rounded_rect_t));

    if (last != this->checksum)
    {
        gui_fb_change();
    }
}

/** Drawing phase processing */
static void gui_rect_draw(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
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
static void gui_rect_end(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
#define SAFE_FREE_IMG(img) \
    if (img != NULL) { \
        if (img->data != NULL) { \
            gui_free(img->data); \
            img->data = NULL; \
        } \
        gui_free(img); \
        img = NULL; \
    }

    SAFE_FREE_IMG(this->rect_0);
    SAFE_FREE_IMG(this->rect_1);
    SAFE_FREE_IMG(this->rect_2);
    SAFE_FREE_IMG(this->circle_00);
    SAFE_FREE_IMG(this->circle_01);
    SAFE_FREE_IMG(this->circle_10);
    SAFE_FREE_IMG(this->circle_11);

#undef SAFE_FREE_IMG
}

static void gui_rect_destroy(gui_obj_t *obj)
{
    gui_rounded_rect_t *this = (gui_rounded_rect_t *)obj;
    GUI_UNUSED(this);
    // Resources are cleaned up in end callback
}

static void gui_rect_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_rect_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_rect_draw(obj);
            break;

        case OBJ_END:
            gui_rect_end(obj);
            break;

        case OBJ_DESTROY:
            gui_rect_destroy(obj);
            break;

        default:
            break;
        }
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

gui_rounded_rect_t *gui_rect_create(void *parent, const char *name, int x, int y,
                                    int w, int h,
                                    int radius, gui_color_t color)
{
    GUI_ASSERT(parent != NULL);
    gui_rounded_rect_t *round_rect = gui_malloc(sizeof(gui_rounded_rect_t));
    GUI_ASSERT(round_rect != NULL);
    memset(round_rect, 0x00, sizeof(gui_rounded_rect_t));

    round_rect->opacity_value = UINT8_MAX;
    round_rect->rect_0 = NULL;
    round_rect->rect_1 = NULL;
    round_rect->rect_2 = NULL;
    round_rect->circle_00 = NULL;
    round_rect->circle_01 = NULL;
    round_rect->circle_10 = NULL;
    round_rect->circle_11 = NULL;

    gui_obj_ctor((gui_obj_t *)round_rect, parent, name, x, y, w, h);
    GET_BASE(round_rect)->obj_cb = gui_rect_cb;
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
    round_rect->radius = radius;
    round_rect->color = color;

    // Initialize transformation parameters
    round_rect->degrees = 0.0f;
    round_rect->scale_x = 1.0f;
    round_rect->scale_y = 1.0f;
    round_rect->offset_x = 0.0f;
    round_rect->offset_y = 0.0f;

    return round_rect;
}

void gui_rect_set_style(gui_rounded_rect_t *this,
                        int x, int y, int w, int h,
                        int radius, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    this->base.x = x;
    this->base.y = y;
    this->base.w = w;
    this->base.h = h;
    this->radius = radius;
    this->color = color;
    this->opacity_value = color.color.rgba.a;
}
void gui_rect_set_opacity(gui_rounded_rect_t *this, uint8_t opacity)
{
    GUI_ASSERT(this != NULL);
    this->opacity_value = opacity;
}
void gui_rect_set_position(gui_rounded_rect_t *this, int x, int y)
{
    GUI_ASSERT(this != NULL);
    this->base.x = x;
    this->base.y = y;
}

void gui_rect_set_size(gui_rounded_rect_t *this, int w, int h)
{
    GUI_ASSERT(this != NULL);
    this->base.w = w;
    this->base.h = h;
}

void gui_rect_set_radius(gui_rounded_rect_t *this, int radius)
{
    GUI_ASSERT(this != NULL);
    this->radius = radius;
}

void gui_rect_set_color(gui_rounded_rect_t *this, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    this->color = color;
    this->opacity_value = color.color.rgba.a;
}

void gui_rect_on_click(gui_rounded_rect_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}

void gui_rect_rotate(gui_rounded_rect_t *this, float degrees)
{
    GUI_ASSERT(this != NULL);
    this->degrees = degrees;
}

void gui_rect_scale(gui_rounded_rect_t *this, float scale_x, float scale_y)
{
    GUI_ASSERT(this != NULL);
    this->scale_x = scale_x;
    this->scale_y = scale_y;
}

void gui_rect_translate(gui_rounded_rect_t *this, float tx, float ty)
{
    GUI_ASSERT(this != NULL);
    this->offset_x = tx;
    this->offset_y = ty;
}
