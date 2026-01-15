/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_circle.c
  * @brief circle widget with hyper-optimized rendering
  * @details Quadrant-based rendering with branchless dithering
  * @author
  * @date 2025/12/03
  * @version 3.0
  ***************************************************************************************
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

/*============================================================================*
 *                           Configuration
 *============================================================================*/
// 1 = Enable Dither (Better gradients), 0 = Disable (Max speed)
#define GUI_CIRCLE_ENABLE_DITHER 1

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/** Safely free a draw_img_t and its data */
static void free_draw_img_circle(draw_img_t **img)
{
    if (img == NULL || *img == NULL) { return; }

    if ((*img)->data != NULL)
    {
        gui_free((void *)(*img)->data);
        (*img)->data = NULL;
    }
    gui_free(*img);
    *img = NULL;
}

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

/**
 * Branchless Dither (Optimized)
 * Uses bitwise logic to clamp values between 0-255 without 'if' statements
 */
static inline uint32_t fast_dither(uint32_t color, int x, int y)
{
    static const int8_t dither_table[16] =
    {
        -8,  0, -6,  2,
        4, -4,  6, -2,
        -5,  3, -7,  1,
        7, -1,  5, -3
    };

    // Get dither value
    int d = dither_table[(y & 3) * 4 + (x & 3)];

    // Unpack
    int a = (color >> 24) & 0xFF;
    int r = (color >> 16) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = color & 0xFF;

    // Add & Clamp (Branchless)
    r += d; r = (r < 0) ? 0 : (r > 255 ? 255 : r);
    g += d; g = (g < 0) ? 0 : (g > 255 ? 255 : g);
    b += d; b = (b < 0) ? 0 : (b > 255 ? 255 : b);

    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/** Create a complete circle in a single buffer with Symmetry Optimization */
static draw_img_t *create_circle_buffer(gui_circle_t *this, gui_obj_t *obj, draw_img_t **old_img)
{
    // Free old buffer first to prevent memory leak
    free_draw_img_circle(old_img);

    int r = this->radius;
    int diameter = r * 2; // Keep diameter even for perfect symmetry logic

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
        gradient_lut = gui_malloc(lut_size * sizeof(uint32_t));
        if (gradient_lut)
        {
            for (int i = 0; i < lut_size; i++)
            {
                float t = (float)i / (float)r;
                gradient_lut[i] = gradient_get_color(this->gradient, t);
            }
        }
    }
    else if (use_angular)
    {
        lut_size = 360;
        gradient_lut = gui_malloc(lut_size * sizeof(uint32_t));
        if (gradient_lut)
        {
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
    }

    // --- High Performance Quadrant Rendering ---
    // We iterate only the Top-Left quadrant (0 to r-1)
    // and map the results to the other 3 quadrants.

    float center = (float)r;
    bool dither = GUI_CIRCLE_ENABLE_DITHER;

    // Optimization: Calculate r_sq once
    float r_in = r - 0.5f;
    float r_out = r + 0.5f;
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
        if(dither) c = fast_dither(c, t_x, t_y); \
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
                // Radial or Solid: Color is identical for all 4 quadrants
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

    gui_rect_file_head_t *rect_head = (gui_rect_file_head_t *)head;
    rect_head->color = color;
}

/** Create a solid color image buffer for better anti-aliasing with transformations */
static uint8_t *create_solid_color_buffer_circle(uint16_t w, uint16_t h, gui_color_t color)
{
    uint32_t buffer_size = w * h * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *buffer = gui_malloc(buffer_size);
    if (buffer == NULL) { return NULL; }

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

/** create vertical arc strip (Legacy/Fallback) */
static draw_img_t *create_vertical_arc_strip(gui_circle_t *this, gui_obj_t *obj,
                                             draw_img_t **old_img)
{
    // Free old buffer first to prevent memory leak
    free_draw_img_circle(old_img);

    if (this->radius <= 0) { return NULL; }

    int inner_half = (int)(this->radius * M_SQRT1_2);
    int inner_size = inner_half * 2;
    int arc_width = this->radius - inner_half;
    if (arc_width < 1) { arc_width = 1; }
    int arc_height = inner_size;

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }
    memset(img, 0x00, sizeof(draw_img_t));

    uint32_t buffer_size = arc_width * arc_height * 4 + sizeof(gui_rgb_data_head_t);
    uint8_t *arc_data = gui_malloc(buffer_size);
    if (arc_data == NULL)
    {
        gui_free(img);
        return NULL;
    }
    memset(arc_data, 0x00, buffer_size);

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

    float center_y = (float)arc_height / 2.0f;
    float radius = (float)this->radius;
    float radius_sq = radius * radius;

    float *exact_boundaries = gui_malloc(arc_height * sizeof(float));
    if (exact_boundaries == NULL)
    {
        gui_free(arc_data);
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
            exact_boundaries[y] = radius - dx;
        }
    }

    for (int y = 0; y < arc_height; y++)
    {
        float exact_boundary = exact_boundaries[y];
        if (exact_boundary < -500.0f)
        {
            for (int x = 0; x < arc_width; x++) { pixels[y * arc_width + x] = solid_color; }
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

            if (coverage > 0.999f) { pixels[y * arc_width + x] = solid_color; }
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

    matrix_identity(&img->matrix);
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

    // Free old img structure (but not data, as it's shared with base_img)
    if (old_img != NULL && *old_img != NULL)
    {
        // Don't free data - it's shared with base_img
        gui_free(*old_img);
        *old_img = NULL;
    }

    draw_img_t *img = gui_malloc(sizeof(draw_img_t));
    if (img == NULL) { return NULL; }

    memcpy(img, base_img, sizeof(draw_img_t));
    img->data = base_img->data;

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
    uint8_t last = this->checksum;

    if (obj->matrix == NULL)
    {
        obj->matrix = gui_malloc(sizeof(gui_matrix_t));
        GUI_ASSERT(obj->matrix != NULL);
        matrix_identity(obj->matrix);
    }

    float center_x = (float)this->radius;
    float center_y = (float)this->radius;

    matrix_translate(this->offset_x, this->offset_y, obj->matrix);
    matrix_translate(center_x, center_y, obj->matrix);
    matrix_scale(this->scale_x, this->scale_y, obj->matrix);
    matrix_rotate(this->degrees, obj->matrix);
    matrix_translate(-center_x, -center_y, obj->matrix);

    // Calculate checksum only for key properties (exclude pointers)
    // Manually calculate checksum for critical fields only
    uint8_t new_checksum = 0;
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->x, sizeof(this->x));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->y, sizeof(this->y));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->radius, sizeof(this->radius));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->color, sizeof(this->color));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->opacity_value,
                                    sizeof(this->opacity_value));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->degrees, sizeof(this->degrees));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->scale_x, sizeof(this->scale_x));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->scale_y, sizeof(this->scale_y));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->offset_x, sizeof(this->offset_x));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->offset_y, sizeof(this->offset_y));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->use_gradient,
                                    sizeof(this->use_gradient));
    new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)&this->gradient_type,
                                    sizeof(this->gradient_type));

    // Include gradient data if present
    if (this->gradient != NULL)
    {
        new_checksum = gui_obj_checksum(new_checksum, (uint8_t *)this->gradient, sizeof(Gradient));
    }

    // Only regenerate buffers if properties changed
    bool need_regenerate = (last != new_checksum);

    // Force single buffer for Gradient or Alpha or Small Circles
    int diameter = this->radius * 2;
    bool need_single_buffer = (this->color.color.rgba.a < 255) ||
                              (diameter * diameter < 10000) ||
                              (this->use_gradient && this->gradient != NULL);

    if (need_single_buffer)
    {
        if (need_regenerate || this->center_rect == NULL)
        {
            this->center_rect = create_circle_buffer(this, obj, &this->center_rect);
        }
    }
    else
    {
        // Use optimized multi-part rendering for large opaque circles
        if (need_regenerate || this->center_rect == NULL)
        {
            int inner_half = (int)floorf(this->radius * M_SQRT1_2);
            int inner_size = inner_half * 2;
            int arc_width = this->radius - inner_half;

            if (inner_size < 1) { inner_size = 1; }
            if (arc_width < 1) { arc_width = 1; }
            if (inner_size > diameter) { inner_size = diameter; }

            int inner_x = arc_width;
            int inner_y = arc_width;

            if (inner_size > 0)
            {
                set_rect_img(this, &this->center_rect, inner_x, inner_y, inner_size, inner_size);
            }

            this->arc_left = create_vertical_arc_strip(this, obj, &this->arc_left);

            if (this->arc_left != NULL)
            {
                int left_x = 0;
                int left_y = arc_width;

                matrix_identity(&this->arc_left->matrix);
                matrix_translate((float)left_x, (float)left_y, &this->arc_left->matrix);

                struct gui_matrix obj_matrix;
                memcpy(&obj_matrix, obj->matrix, sizeof(struct gui_matrix));
                matrix_multiply(&this->arc_left->matrix, &obj_matrix);

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
        }
    }

    this->checksum = new_checksum;

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

    if (this->center_rect != NULL) { gui_acc_blit_to_dc(this->center_rect, dc, NULL); }
    if (this->arc_left != NULL) { gui_acc_blit_to_dc(this->arc_left, dc, NULL); }
    if (this->arc_right != NULL) { gui_acc_blit_to_dc(this->arc_right, dc, NULL); }
    if (this->arc_top != NULL) { gui_acc_blit_to_dc(this->arc_top, dc, NULL); }
    if (this->arc_bottom != NULL) { gui_acc_blit_to_dc(this->arc_bottom, dc, NULL); }
}

/** End phase processing - Memory management */
static void gui_circle_end(gui_circle_t *this)
{
    // DO NOT free buffers here - they are cached for reuse
    // Buffers will be freed in gui_circle_destroy() when widget is destroyed
    (void)this; // Suppress unused parameter warning
}

static void gui_circle_destroy(gui_circle_t *this)
{
    // Free gradient data
    if (this->gradient != NULL)
    {
        gui_free(this->gradient);
        this->gradient = NULL;
    }

    // Free cached buffers
    // Note: For transformed arcs (arc_right, arc_top, arc_bottom),
    // they share pixel data with arc_left, so we only free the structure
    if (this->arc_right != NULL)
    {
        gui_free(this->arc_right);
        this->arc_right = NULL;
    }
    if (this->arc_top != NULL)
    {
        gui_free(this->arc_top);
        this->arc_top = NULL;
    }
    if (this->arc_bottom != NULL)
    {
        gui_free(this->arc_bottom);
        this->arc_bottom = NULL;
    }

    // Free arc_left (which owns the pixel data)
    free_draw_img_circle(&this->arc_left);

    // Free center_rect
    free_draw_img_circle(&this->center_rect);
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
    circle->degrees = 0.0f;
    circle->scale_x = 1.0f;
    circle->scale_y = 1.0f;
    circle->offset_x = 0.0f;
    circle->offset_y = 0.0f;
    circle->gradient = NULL;
    circle->use_gradient = false;
    circle->gradient_type = CIRCLE_GRADIENT_RADIAL;

    return circle;
}

void gui_circle_set_style(gui_circle_t *circle, int x, int y, int radius, gui_color_t color)
{
    GUI_ASSERT(circle != NULL);
    circle->x = x;
    circle->y = y;
    circle->radius = radius;
    circle->color = color;
    circle->opacity_value = color.color.rgba.a;
}

void gui_circle_set_position(gui_circle_t *circle, int x, int y)
{
    GUI_ASSERT(circle != NULL);
    if (circle->x != x || circle->y != y)
    {
        circle->x = x;
        circle->y = y;
    }
}

void gui_circle_set_radius(gui_circle_t *circle, int radius)
{
    GUI_ASSERT(circle != NULL);
    if (circle->radius != radius)
    {
        circle->radius = radius;
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
    if (circle->color.color.argb_full != color.color.argb_full)
    {
        circle->color = color;
        circle->opacity_value = color.color.rgba.a;
    }
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
