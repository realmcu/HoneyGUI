/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_circle.c
  * @brief circle widget
  * @details circle widget is used to draw circle shapes on the screen
  * @author
  * @date 2025/12/03
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
#include "gui_circle.h"

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

/** Check if a point is inside the circle's bounding circle */
static bool is_point_in_circle(gui_circle_t *circle, int x, int y)
{
    int dx = x - circle->x;
    int dy = y - circle->y;
    int distance_sq = dx * dx + dy * dy;
    int radius_sq = circle->radius * circle->radius;

    return (distance_sq <= radius_sq);
}

static void gui_circle_input_prepare(gui_obj_t *obj)
{
    gui_circle_t *this = (gui_circle_t *)obj;

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
        }
    }
}

/** Create a complete circle in a single buffer to avoid seams */
static draw_img_t *create_circle_buffer(gui_circle_t *this, gui_obj_t *obj)
{
    int diameter = this->radius * 2;
    int r = this->radius;

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    // Create pixel buffer
    uint32_t buffer_size = diameter * diameter * 4 + sizeof(gui_rgb_data_head_t);
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
    head->w = diameter;
    head->h = diameter;
    head->version = 0;
    head->rsvd2 = 0;

    uint32_t *pixels = (uint32_t *)(buffer + sizeof(gui_rgb_data_head_t));
    uint32_t solid_color = this->color.color.argb_full;

    // Fill circle with anti-aliasing
    float center = (float)r;
    for (int y = 0; y < diameter; y++)
    {
        for (int x = 0; x < diameter; x++)
        {
            float dx = x + 0.5f - center;
            float dy = y + 0.5f - center;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist <= r - 0.5f)
            {
                // Fully inside
                pixels[y * diameter + x] = solid_color;
            }
            else if (dist < r + 0.5f)
            {
                // Anti-aliasing edge
                float alpha = r + 0.5f - dist;
                gui_color_t color = this->color;
                color.color.rgba.a = (uint8_t)(alpha * color.color.rgba.a);
                pixels[y * diameter + x] = color.color.argb_full;
            }
            // else: outside, leave transparent
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
static uint8_t *create_solid_color_buffer_circle(uint16_t w, uint16_t h, gui_color_t color)
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
static void set_rect_img(gui_circle_t *this, draw_img_t **input_img, int16_t x,
                         int16_t y, uint16_t w, uint16_t h)
{
    gui_obj_t *obj = (gui_obj_t *)this;

    // Clean up previous image if exists
    if (*input_img != NULL)
    {
        if ((*input_img)->data != NULL)
        {
            gui_free((*input_img)->data);
            (*input_img)->data = NULL;
        }
        gui_free(*input_img);
        *input_img = NULL;
    }

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return; }

    memset(img, 0x00, sizeof(draw_img_t));

    // Check if transformation is applied
    bool has_transform = (this->degrees != 0.0f || this->scale_x != 1.0f || this->scale_y != 1.0f);

    if (has_transform)
    {
        // Use IMG_SRC_OVER_MODE with actual pixel buffer for anti-aliasing
        img->blend_mode = IMG_SRC_OVER_MODE;
        img->high_quality = 1;
        img->data = create_solid_color_buffer_circle(w, h, this->color);
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
        if (rect_data == NULL)
        {
            gui_free(img);
            return;
        }

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
/** create vertical arc strip */
static draw_img_t *create_vertical_arc_strip(gui_circle_t *this, gui_obj_t *obj)
{
    if (this->radius <= 0) { return NULL; }

    int inner_half = (int)(this->radius * M_SQRT1_2);
    int inner_size = inner_half * 2;

    // arc width = r - inner_half
    int arc_width = this->radius - inner_half;
    if (arc_width < 1) { arc_width = 1; }

    int arc_height = inner_size;

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }

    memset(img, 0x00, sizeof(draw_img_t));

    // buffer size = arc_width * arc_height * 4 + sizeof(gui_rgb_data_head_t)
    uint32_t buffer_size = arc_width * arc_height * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *arc_data = gui_malloc(buffer_size);
    if (arc_data == NULL)
    {
        gui_free(img);
        return NULL;
    }
    memset(arc_data, 0x00, buffer_size);

    // set image header
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)arc_data;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->w = arc_width;
    head->h = arc_height;
    head->version = 0;
    head->rsvd2 = 0;

    uint32_t *pixels = (uint32_t *)(arc_data + sizeof(gui_rgb_data_head_t));
    uint32_t solid_color = this->color.color.argb_full;

    // radius center (0, arc_height/2)
    float center_y = (float)arc_height / 2.0f;
    float radius = (float)this->radius;
    float radius_sq = radius * radius;

    // pre-calculate exact boundary for each row
    float *exact_boundaries = gui_malloc(arc_height * sizeof(float));
    if (exact_boundaries == NULL)
    {
        gui_free(arc_data);
        gui_free(img);
        return NULL;
    }

    // calculate exact boundary for each row
    for (int y = 0; y < arc_height; y++)
    {
        float py = y + 0.5f - center_y;
        float py_sq = py * py;

        if (py_sq > radius_sq)
        {
            exact_boundaries[y] = -1000.0f; // out of circle
        }
        else
        {
            float dx = sqrtf(radius_sq - py_sq);
            exact_boundaries[y] = radius - dx;
        }
    }

    // fill pixel
    for (int y = 0; y < arc_height; y++)
    {
        float exact_boundary = exact_boundaries[y];

        if (exact_boundary < -500.0f)
        {
            for (int x = 0; x < arc_width; x++)
            {
                pixels[y * arc_width + x] = solid_color;
            }
            continue;
        }

        for (int x = 0; x < arc_width; x++)
        {
            float pixel_center_x = x + 0.5f;

            // calculate distance to boundary
            float distance_to_boundary = pixel_center_x - exact_boundary;

            float coverage;

            if (distance_to_boundary >= 0.5f)
            {
                coverage = 1.0f;
            }
            else if (distance_to_boundary <= -0.5f)
            {
                coverage = 0.0f;
            }
            else
            {
                float t = (distance_to_boundary + 0.5f);
                coverage = 0.5f - 0.5f * cosf(M_PI * t);
            }

            if (coverage > 0.999f)
            {
                pixels[y * arc_width + x] = solid_color;
            }
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

    img->data = arc_data;
    img->blend_mode = IMG_SRC_OVER_MODE;
    img->opacity_value = this->opacity_value;
    img->high_quality = 1;

    // Initialize with identity matrix
    matrix_identity(&img->matrix);

    // Apply user's transformation matrix if exists
    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }

    memcpy(&img->inverse, &img->matrix, sizeof(struct gui_matrix));
    matrix_inverse(&img->inverse);

    draw_img_load_scale(img, IMG_SRC_MEMADDR);
    draw_img_new_area(img, NULL);

    return img;
}

/** Create other three arc segments through transformation*/
static draw_img_t *create_transformed_arc(gui_circle_t *this, gui_obj_t *obj,
                                          draw_img_t *base_img,
                                          int pos_x, int pos_y,
                                          bool mirror_x, bool mirror_y,
                                          bool is_top_bottom)
{
    GUI_UNUSED(this);
    if (base_img == NULL || base_img->data == NULL)
    {
        gui_log("create_transformed_arc: base_img is NULL\n");
        return NULL;
    }

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL)
    {
        gui_log("create_transformed_arc: failed to allocate img\n");
        return NULL;
    }

    memcpy(img, base_img, sizeof(draw_img_t));

    // share pixel data with base_img
    img->data = base_img->data;

    // get base image size
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)base_img->data;
    int base_width = head->w;
    int base_height = head->h;

    // Start with user's transformation matrix if exists, otherwise identity
    if (obj->matrix != NULL)
    {
        memcpy(&img->matrix, obj->matrix, sizeof(struct gui_matrix));
    }
    else
    {
        matrix_identity(&img->matrix);
    }

    // Apply position translation (after user's transformation)
    matrix_translate((float)pos_x, (float)pos_y, &img->matrix);

    if (is_top_bottom)
    {
        // top bottom arc need rotate 90 degree
        if (mirror_y)
        {
            // bottom arc: rotate -90 degree (counterclockwise)
            matrix_rotate(-90.0f, &img->matrix);
        }
        else
        {
            // top arc: rotate 90 degree (clockwise)
            matrix_rotate(90.0f, &img->matrix);
        }
    }
    else if (mirror_x)
    {
        // right arc need horizontal mirror

        // right arc: horizontal mirror (x-axis reverse)

        // first translate to image center
        matrix_translate((float)base_width / 2.0f, (float)base_height / 2.0f, &img->matrix);

        // horizontal mirror (x-axis reverse)
        matrix_scale(-1.0f, 1.0f, &img->matrix);

        // mirror back to original position
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
    uint8_t last = this->checksum;

    // Initialize obj->matrix if not already done
    if (obj->matrix == NULL)
    {
        obj->matrix = gui_malloc(sizeof(gui_matrix_t));
        GUI_ASSERT(obj->matrix != NULL);
        matrix_identity(obj->matrix);
    }

    // Apply transformations (like gui_img_prepare does)
    float center_x = (float)this->radius;
    float center_y = (float)this->radius;

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

    // Check if we need single buffer (for transparency or small circles)
    int diameter = this->radius * 2;
    bool need_single_buffer = (this->color.color.rgba.a < 255) ||
                              (diameter * diameter < 10000);

    if (need_single_buffer)
    {
        // Use single buffer for transparency or small circles to avoid seams
        this->center_rect = create_circle_buffer(this, obj);
    }
    else
    {
        // Use optimized multi-part rendering for large opaque circles
        // calculate inner square parameters
        int inner_half = (int)floorf(this->radius * M_SQRT1_2);
        int inner_size = inner_half * 2;
        int arc_width = this->radius - inner_half;

        if (inner_size < 1) { inner_size = 1; }
        if (arc_width < 1) { arc_width = 1; }

        // ensure inner square size not greater than diameter
        if (inner_size > diameter) { inner_size = diameter; }

        // inner square position: start from (arc_width, arc_width)
        int inner_x = arc_width;
        int inner_y = arc_width;

        // center square (inner square)
        if (inner_size > 0)
        {
            set_rect_img(this, &this->center_rect, inner_x, inner_y, inner_size, inner_size);
        }

        // create left vertical arc strip (base image)
        this->arc_left = create_vertical_arc_strip(this, obj);

        if (this->arc_left != NULL)
        {
            // left vertical arc position: touch center square left edge
            int left_x = 0;          // x from 0
            int left_y = arc_width;  // y align with center square top edge

            // set left vertical arc position
            matrix_identity(&this->arc_left->matrix);
            matrix_translate((float)left_x, (float)left_y, &this->arc_left->matrix);

            // combine with object matrix
            struct gui_matrix obj_matrix;
            memcpy(&obj_matrix, obj->matrix, sizeof(struct gui_matrix));
            matrix_multiply(&this->arc_left->matrix, &obj_matrix);

            memcpy(&this->arc_left->inverse, &this->arc_left->matrix, sizeof(struct gui_matrix));
            matrix_inverse(&this->arc_left->inverse);
            draw_img_new_area(this->arc_left, NULL);

            // right vertical arc position: touch center square right edge
            int right_x = arc_width + inner_size - 1;  // x align with center square right edge
            int right_y = arc_width;
            this->arc_right = create_transformed_arc(this, obj, this->arc_left,
                                                     right_x, right_y,
                                                     true, false, false);
            // top vertical arc position: touch center square top edge
            int top_x = arc_width + inner_size;                  // x align with center square left edge
            int top_y = 0;                          // y in top
            this->arc_top = create_transformed_arc(this, obj, this->arc_left,
                                                   top_x, top_y,
                                                   false, false, true);

            // bottom vertical arc position: touch center square bottom edge
            int bottom_x = arc_width;               // x align with center square left edge
            int bottom_y = arc_width * 2 + inner_size - 1; // y align with center square bottom edge
            this->arc_bottom = create_transformed_arc(this, obj, this->arc_left,
                                                      bottom_x, bottom_y,
                                                      false, true, true);
        }
    }

    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_circle_t));

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

    // in center square
    if (this->center_rect != NULL)
    {
        gui_acc_blit_to_dc(this->center_rect, dc, NULL);
    }

    // in vertical arc strip
    if (this->arc_left != NULL) { gui_acc_blit_to_dc(this->arc_left, dc, NULL); }
    if (this->arc_right != NULL) { gui_acc_blit_to_dc(this->arc_right, dc, NULL); }
    if (this->arc_top != NULL) { gui_acc_blit_to_dc(this->arc_top, dc, NULL); }
    if (this->arc_bottom != NULL) { gui_acc_blit_to_dc(this->arc_bottom, dc, NULL); }
}

/** End phase processing - Memory management */
static void gui_circle_end(gui_circle_t *this)
{
#define SAFE_FREE_IMG(img) \
    do { \
        if ((img) != NULL) { \
            if ((img)->data != NULL) { \
                gui_free((img)->data); \
                (img)->data = NULL; \
            } \
            gui_free(img); \
            (img) = NULL; \
        } \
    } while(0)

    SAFE_FREE_IMG(this->center_rect);
    if (this->arc_left != NULL)
    {
        if (this->arc_left->data != NULL)
        {
            gui_free(this->arc_left->data);
            this->arc_left->data = NULL;
        }
        gui_free(this->arc_left);
        this->arc_left = NULL;
    }

    // other vertical arc strip
    if (this->arc_right != NULL)
    {
        this->arc_right->data = NULL;
        gui_free(this->arc_right);
        this->arc_right = NULL;
    }

    if (this->arc_top != NULL)
    {
        this->arc_top->data = NULL;
        gui_free(this->arc_top);
        this->arc_top = NULL;
    }

    if (this->arc_bottom != NULL)
    {
        this->arc_bottom->data = NULL;
        gui_free(this->arc_bottom);
        this->arc_bottom = NULL;
    }
#undef SAFE_FREE_IMG
}

static void gui_circle_destroy(gui_circle_t *this)
{
    GUI_UNUSED(this);
}

static void gui_circle_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_circle_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_circle_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_circle_draw(obj);
            break;

        case OBJ_END:
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
    GUI_ASSERT(parent != NULL);

    gui_circle_t *circle = gui_malloc(sizeof(gui_circle_t));
    if (circle == NULL)
    {
        gui_log("Failed to allocate circle widget\n");
        return NULL;
    }

    memset(circle, 0x00, sizeof(gui_circle_t));

    circle->opacity_value = color.color.rgba.a;

    circle->center_rect = NULL;
    circle->arc_left = NULL;
    circle->arc_right = NULL;
    circle->arc_top = NULL;
    circle->arc_bottom = NULL;

    gui_obj_ctor((gui_obj_t *)circle, parent, name, x - radius, y - radius, radius * 2, radius * 2);
    GET_BASE(circle)->obj_cb = gui_circle_cb;
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
    circle->x = radius;
    circle->y = radius;
    circle->radius = radius;
    circle->color = color;
    circle->checksum = 0;

    // Initialize transformation parameters
    circle->degrees = 0.0f;
    circle->scale_x = 1.0f;
    circle->scale_y = 1.0f;
    circle->offset_x = 0.0f;
    circle->offset_y = 0.0f;

    return circle;
}

void gui_circle_set_style(gui_circle_t *this,
                          int x, int y,
                          int radius, gui_color_t color)
{
    GUI_ASSERT(this != NULL);
    this->x = x;
    this->y = y;
    this->radius = radius;
    this->color = color;
    this->opacity_value = color.color.rgba.a;
}

void gui_circle_set_position(gui_circle_t *this, int x, int y)
{
    GUI_ASSERT(this != NULL);

    if (this->x != x || this->y != y)
    {
        this->x = x;
        this->y = y;
    }
}

void gui_circle_set_radius(gui_circle_t *this, int radius)
{
    GUI_ASSERT(this != NULL);

    if (this->radius != radius)
    {
        this->radius = radius;
    }
}

void gui_circle_set_color(gui_circle_t *this, gui_color_t color)
{
    GUI_ASSERT(this != NULL);

    if (this->color.color.argb_full != color.color.argb_full)
    {
        this->color = color;
        this->opacity_value = color.color.rgba.a;
    }
}

void gui_circle_on_click(gui_circle_t *this, void *callback, void *parameter)
{
    gui_obj_add_event_cb((gui_obj_t *)this, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED,
                         parameter);
}

void gui_circle_rotate(gui_circle_t *this, float degrees)
{
    GUI_ASSERT(this != NULL);
    this->degrees = degrees;
}

void gui_circle_scale(gui_circle_t *this, float scale_x, float scale_y)
{
    GUI_ASSERT(this != NULL);
    this->scale_x = scale_x;
    this->scale_y = scale_y;
}

void gui_circle_translate(gui_circle_t *this, float tx, float ty)
{
    GUI_ASSERT(this != NULL);
    this->offset_x = tx;
    this->offset_y = ty;
}
