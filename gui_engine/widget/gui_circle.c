#include "gui_obj.h"
#include "gui_api.h"
#include "gui_circle.h"
#include "math.h"

static void prepare(gui_circle_t *widget)
{
    int last = widget->checksum;
    widget->checksum = 0;
    widget->checksum = gui_obj_checksum(0, (uint8_t *)widget, sizeof(gui_circle_t));
    if (last != widget->checksum || widget->render)
    {
        widget->render = 0;
        gui_fb_change();
    }
}

// Define the sample factor
#define GRID_SAMPLES 4

// Color structure in RGBA format
typedef struct
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;  // Alpha channel for transparency
} color_t;


typedef enum
{
    COLOR_FORMAT_RGBA,
    COLOR_FORMAT_RGB565
} color_format_t;

typedef struct
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;  // Alpha channel for transparency

} rgba_color_t;
typedef struct
{
    uint16_t rgb565;
    uint16_t b;
    uint16_t g;
    uint16_t r;

} rgb565_color_t;
typedef struct
{
    rgba_color_t color;
    rgb565_color_t color_rgb565;
    uint8_t opa;
    int32_t radius;
    int32_t cx;
    int32_t cy;
} draw_circle_dsc_t;

typedef struct
{
    color_format_t format;
    void *buf;
    int32_t buf_width;
    int32_t buf_height;
} draw_ctx_t;


// Circle or sector drawing descriptor
typedef struct
{
    rgba_color_t color;
    rgb565_color_t color_rgb565;
    uint8_t opa;
    int32_t radius;
    int32_t cx;
    int32_t cy;
    float start_angle; // Starting angle of the sector (radians)
    float end_angle;   // Ending angle of the sector (radians)
} sector_draw_shape_dsc_t;
// Rounded rectangle drawing descriptor
typedef struct
{
    rgba_color_t color;
    rgb565_color_t color_rgb565;
    uint8_t opa;
    int32_t radius;
    int32_t rect_x;
    int32_t rect_y;
    int32_t rect_w;
    int32_t rect_h;
} rounded_rect_draw_shape_dsc_t;
// Define the RGB565 bit definitions
#define RGB565(r, g, b)  ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))


// Clipping region structure
typedef struct
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} clip_rect_t;
// Color mixing function
static color_t color_mix(color_t c1, color_t c2, uint8_t ratio)
{
    color_t result;
    if (ratio == 255)
    {
        result.r = c1.r;
        result.g = c1.g;
        result.b = c1.b;
    }
    else
    {
        result.r = (c1.r * ratio + c2.r * (255 - ratio)) / 255;
        result.g = (c1.g * ratio + c2.g * (255 - ratio)) / 255;
        result.b = (c1.b * ratio + c2.b * (255 - ratio)) / 255;
    }

    result.a = (c1.a * ratio + c2.a * (255 - ratio)) / 255;
    return result;
}
static rgba_color_t color_mix_rgba(rgba_color_t c1, rgba_color_t c2, uint8_t ratio)
{
    if (ratio == 255)
    {
        return c1;
    }

    rgba_color_t result;
    uint8_t inv_ratio = 255 - ratio;

    // Precompute some common terms
    uint16_t r_term = ((uint16_t)c1.r * ratio + (uint16_t)c2.r * inv_ratio + 128) >> 8;
    uint16_t g_term = ((uint16_t)c1.g * ratio + (uint16_t)c2.g * inv_ratio + 128) >> 8;
    uint16_t b_term = ((uint16_t)c1.b * ratio + (uint16_t)c2.b * inv_ratio + 128) >> 8;
    uint16_t a_term = ((uint16_t)c1.a * ratio + (uint16_t)c2.a * inv_ratio + 128) >> 8;

    result.r = (uint8_t)r_term;
    result.g = (uint8_t)g_term;
    result.b = (uint8_t)b_term;
    result.a = (uint8_t)a_term;

    return result;
}



static uint16_t color_mix_rgb565(uint16_t c1, uint16_t c2, uint16_t r1, uint16_t g1, uint16_t b1,
                                 uint8_t ratio)
{
    if (ratio == 255)
    {
        return c1;
    }

    uint8_t inv_ratio = 255 - ratio;

    uint32_t r = (r1) * ratio + (c2 & 0xF800) * inv_ratio;
    uint32_t g = (g1) * ratio + (c2 & 0x07E0) * inv_ratio;
    uint32_t b = (b1) * ratio + (c2 & 0x001F) * inv_ratio;

    r = (r >> 8) & 0xF800;
    g = (g >> 8) & 0x07E0;
    b = (b >> 8) & 0x001F;

    return (uint16_t)(r | g | b);
}


// Determining if an angle is within a sector

// Function to draw a circle with clipping
static void draw_circle(draw_ctx_t *ctx, const draw_circle_dsc_t *dsc,
                        const clip_rect_t *clip_rect)
{
    if (ctx == NULL || dsc == NULL) { return; }

    rgba_color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t radius = dsc->radius;
    int32_t cx = dsc->cx;
    int32_t cy = dsc->cy;

    rgba_color_t *buf_rgba = (rgba_color_t *)ctx->buf;
    uint16_t *buf_rgb565 = (uint16_t *)ctx->buf;
    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    // Calculate the interval between each sample point
    float inv_samples = 1.0f / GRID_SAMPLES;

    // Determine the color format of the output buffer
    color_format_t format = ctx->format;

    // Iterate over each pixel in the buffer
    for (int32_t y = 0; y < buf_h; y++)
    {
        for (int32_t x = 0; x < buf_w; x++)
        {
            // Skip the pixel if it's outside the clipping region
            if (clip_rect != NULL && (x < clip_rect->x || x >= clip_rect->x + clip_rect->width ||
                                      y < clip_rect->y || y >= clip_rect->y + clip_rect->height))
            {
                continue;
            }

            // Calculate the distance from the center of this pixel to the center of the circle
            float dx = x - cx;
            float dy = y - cy;
            float distance = sqrtf(dx * dx + dy * dy);

            // If completely inside the circle, fill the color directly, no need for anti-aliasing
            if (distance <= radius - 1.0f)
            {
                int32_t buf_index = y * buf_w + x;
                if (format == COLOR_FORMAT_RGBA)
                {
                    buf_rgba[buf_index] = color_mix_rgba(color, ((rgba_color_t *)buf_rgba)[buf_index], opa);
                }
                else if (format == COLOR_FORMAT_RGB565)
                {
                    buf_rgb565[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                             ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                             opa);
                }
            }
            // Otherwise, perform anti-aliasing
            else if (distance <= radius + 1.0f)
            {
                int inside_count = 0;

                // Perform grid sampling for the current pixel
                for (int i = 0; i < GRID_SAMPLES; i++)
                {
                    for (int j = 0; j < GRID_SAMPLES; j++)
                    {
                        // Calculate the position of each sub-pixel
                        float sub_x = x + (i + 0.5f) * inv_samples;
                        float sub_y = y + (j + 0.5f) * inv_samples;

                        // Calculate the distance between the sub-pixel and the center of the circle
                        float sub_dx = sub_x - cx;
                        float sub_dy = sub_y - cy;
                        if (sub_dx * sub_dx + sub_dy * sub_dy <= radius * radius)
                        {
                            inside_count++;
                        }
                    }
                }

                // Calculate the coverage ratio of sub-pixels
                float coverage_ratio = inside_count * inv_samples * inv_samples;
                if (coverage_ratio > 0)
                {
                    int32_t buf_index = y * buf_w + x;
                    uint8_t mixed_opa = opa * coverage_ratio;

                    // Mix colors based on coverage ratio
                    if (format == COLOR_FORMAT_RGBA)
                    {
                        buf_rgba[buf_index] = color_mix_rgba(color, ((rgba_color_t *)buf_rgba)[buf_index], mixed_opa);
                    }
                    else if (format == COLOR_FORMAT_RGB565)
                    {
                        buf_rgb565[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                 ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                 mixed_opa);
                    }
                }
            }
        }
    }
}

// Function to draw a circle with clipping
static void draw_circle_save(draw_ctx_t *ctx, const draw_circle_dsc_t *dsc
                            )
{
    if (ctx == NULL || dsc == NULL) { return; }

    rgba_color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t radius = dsc->radius;
    int32_t cx = dsc->cx;
    int32_t cy = dsc->cy;

    rgba_color_t *buf_rgba = (rgba_color_t *)ctx->buf;
    uint16_t *buf_rgb565 = (uint16_t *)ctx->buf;
    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    // Calculate the interval between each sample point
    float inv_samples = 1.0f / GRID_SAMPLES;

    // Determine the color format of the output buffer
    color_format_t format = ctx->format;

    // Iterate over each pixel in the buffer
    for (int32_t y = 0; y < buf_h; y++)
    {
        for (int32_t x = 0; x < buf_w; x++)
        {
            // Skip the pixel if it's outside the clipping region

            // Calculate the distance from the center of this pixel to the center of the circle
            float dx = x - cx;
            float dy = y - cy;
            float distance = sqrtf(dx * dx + dy * dy);

            // If completely inside the circle, fill the color directly, no need for anti-aliasing
            if (distance <= radius - 1.0f)
            {
                int32_t buf_index = y * buf_w + x;
                buf_rgba[buf_index] = color;

            }
            // Otherwise, perform anti-aliasing
            else if (distance <= radius + 1.0f)
            {
                int inside_count = 0;

                // Perform grid sampling for the current pixel
                for (int i = 0; i < GRID_SAMPLES; i++)
                {
                    for (int j = 0; j < GRID_SAMPLES; j++)
                    {
                        // Calculate the position of each sub-pixel
                        float sub_x = x + (i + 0.5f) * inv_samples;
                        float sub_y = y + (j + 0.5f) * inv_samples;

                        // Calculate the distance between the sub-pixel and the center of the circle
                        float sub_dx = sub_x - cx;
                        float sub_dy = sub_y - cy;
                        if (sub_dx * sub_dx + sub_dy * sub_dy <= radius * radius)
                        {
                            inside_count++;
                        }
                    }
                }

                // Calculate the coverage ratio of sub-pixels
                float coverage_ratio = inside_count * inv_samples * inv_samples;
                if (coverage_ratio > 0)
                {
                    int32_t buf_index = y * buf_w + x;

                    rgba_color_t color_temp = color;
                    color_temp.a = opa * coverage_ratio;
                    // Mix colors based on coverage ratio
                    {
                        buf_rgba[buf_index] = color_temp;
                    }
                }
            }
        }
    }
}

static void *renderer_save(int r, gui_color_t color)
{
    struct gui_dispdev *screen = gui_get_dc();
    int32_t buf_width = r * 2;
    int32_t buf_height = buf_width;
    uint8_t *file = gui_malloc(buf_width * buf_height * 4 + sizeof(gui_rgb_data_head_t));
    memset(file, 0, buf_width * buf_height * 4 + sizeof(gui_rgb_data_head_t));
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)file;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = buf_width;
    head->h = buf_height;
    color_t *buffer = (color_t *)(file + sizeof(gui_rgb_data_head_t));
    draw_ctx_t draw_ctx = {COLOR_FORMAT_RGBA, buffer, buf_width, buf_height};
    int x = 0, y = 0;

    draw_circle_dsc_t circle_dsc =
    {
        .color = {color.color.rgba.b, color.color.rgba.g, color.color.rgba.r, color.color.rgba.a},   // Circle color
        .opa = 255,                  // Circle opacity
        .radius = r,         // Circle radius
        .cx = r,         // Circle center X coordinate
        .cy = r,      // Circle center Y coordinate

    };


    // Call the function to draw the circle
    draw_circle_save(&draw_ctx, &circle_dsc);
    return file;
}
// Determine if an angle is within a sector's range
static int is_within_sector(float angle, float start_angle, float end_angle)
{
    if (end_angle < start_angle)
    {
        return (angle >= start_angle || angle <= end_angle);
    }
    return (angle >= start_angle && angle <= end_angle);
}

// Function to draw a circle or sector with clipping
void draw_circle_or_sector(draw_ctx_t *ctx, const sector_draw_shape_dsc_t *dsc,
                           const clip_rect_t *clip_rect)
{
    if (ctx == NULL || dsc == NULL) { return; }

    rgba_color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t radius = dsc->radius;
    int32_t cx = dsc->cx;
    int32_t cy = dsc->cy;
    float start_angle = dsc->start_angle;
    float end_angle = dsc->end_angle;

    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    // Calculate the interval between each sample point
    float inv_samples = 1.0f / GRID_SAMPLES;

    // Determine the color format of the output buffer
    color_format_t format = ctx->format;

    // Iterate over each pixel in the buffer
    for (int32_t y = 0; y < buf_h; y++)
    {
        for (int32_t x = 0; x < buf_w; x++)
        {
            // Skip the pixel if it's outside the clipping region
            if (clip_rect != NULL && (x < clip_rect->x || x >= clip_rect->x + clip_rect->width ||
                                      y < clip_rect->y || y >= clip_rect->y + clip_rect->height))
            {
                continue;
            }

            // Calculate the distance from the center of this pixel to the center of the circle
            float dx = x - cx + 0.5f;
            float dy = y - cy + 0.5f;
            float distance = sqrtf(dx * dx + dy * dy);

            // Calculate the angle of this pixel center
            float angle = atan2f(dy, dx);
            if (angle < 0) { angle += 2 * M_PI; }

            // Check if the current pixel is within the sector's range
            int within_sector = is_within_sector(angle, start_angle, end_angle);

            // Process if the pixel is within the circle and sector range
            int32_t buf_index = y * buf_w + x;
            if (distance <= radius - 0.5f && within_sector)
            {
                if (format == COLOR_FORMAT_RGBA)
                {
                    ((rgba_color_t *)ctx->buf)[buf_index] = color_mix_rgba(color, ((rgba_color_t *)ctx->buf)[buf_index],
                                                                           opa);
                }
                else if (format == COLOR_FORMAT_RGB565)
                {
                    ((uint16_t *)ctx->buf)[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                         ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                         opa);
                }
            }
            else if (distance <= radius + 0.5f)
            {
                int inside_count = 0;

                // Perform grid sampling for the current pixel
                for (int i = 0; i < GRID_SAMPLES; i++)
                {
                    for (int j = 0; j < GRID_SAMPLES; j++)
                    {
                        float sub_x = x + (i + 0.5f) * inv_samples;
                        float sub_y = y + (j + 0.5f) * inv_samples;
                        float sub_dx = sub_x - cx;
                        float sub_dy = sub_y - cy;
                        float sub_distance = sqrtf(sub_dx * sub_dx + sub_dy * sub_dy);
                        float sub_angle = atan2f(sub_dy, sub_dx);
                        if (sub_angle < 0) { sub_angle += 2 * M_PI; }

                        if (sub_distance <= radius && is_within_sector(sub_angle, start_angle, end_angle))
                        {
                            inside_count++;
                        }
                    }
                }

                float coverage_ratio = inside_count * inv_samples * inv_samples;
                if (coverage_ratio > 0)
                {
                    uint8_t mixed_opa = opa * coverage_ratio;
                    if (format == COLOR_FORMAT_RGBA)
                    {
                        ((rgba_color_t *)ctx->buf)[buf_index] = color_mix_rgba(color, ((rgba_color_t *)ctx->buf)[buf_index],
                                                                               mixed_opa);
                    }
                    else if (format == COLOR_FORMAT_RGB565)
                    {
                        ((uint16_t *)ctx->buf)[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                             ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                             mixed_opa);
                    }
                }
            }
        }
    }
}

static void renderer(gui_circle_t *widget)
{
    struct gui_dispdev *screen = gui_get_dc();
    int32_t buf_width = screen->fb_width;
    int32_t buf_height = screen->fb_height;
    color_t *buffer = (color_t *)screen->frame_buf;
    draw_ctx_t draw_ctx = {COLOR_FORMAT_RGB565, buffer, buf_width, buf_height};
    if (screen->bit_depth == 32)
    {
        draw_ctx.format = COLOR_FORMAT_RGBA;
    }
    int x = 0, y = 0;
    gui_obj_absolute_xy(GUI_BASE(widget), &x, &y);
    uint16_t color_rgb565 = RGB565(widget->fill.color.rgba.r, widget->fill.color.rgba.g,
                                   widget->fill.color.rgba.b);
    draw_circle_dsc_t circle_dsc =
    {
        .color = {widget->fill.color.rgba.b, widget->fill.color.rgba.g, widget->fill.color.rgba.r, widget->fill.color.rgba.a},   // Circle color
        .opa = 255,                  // Circle opacity
        .radius = GUI_BASE(widget)->w / 2,         // Circle radius
        .cx = GUI_BASE(widget)->w / 2 + x,         // Circle center X coordinate
        .cy = GUI_BASE(widget)->w / 2 + y - screen->fb_height * screen->section_count,      // Circle center Y coordinate
        .color_rgb565 = {
            .rgb565 = color_rgb565,
            .b = color_rgb565 & 0x001F,
            .g = color_rgb565 & 0x07E0,
            .r = color_rgb565 & 0xF800,
        }
    };
    // Set the clipping area
    clip_rect_t clip_area =
    {
        .x = GUI_BASE(widget)->x,   // X-coordinate of the top-left corner of the clipping area
        .y = GUI_BASE(widget)->y,   // Y-coordinate of the top-left corner of the clipping area
        .width = GUI_BASE(widget)->w, // Width of the clipping area
        .height = GUI_BASE(widget)->h  // Height of the clipping area
    };
    if (screen->section_count)
    {
        clip_area.y = 0;
        clip_area.height = screen->fb_height;
    }
    // Call the function to draw the circle
    draw_circle(&draw_ctx, &circle_dsc, &clip_area);
}
typedef struct
{
    rgba_color_t color;
    rgb565_color_t color_rgb565;
    uint8_t opa;
    int32_t outer_radius;   // Outer radius of the ring
    int32_t inner_radius;   // Inner radius of the ring
    int32_t cx;
    int32_t cy;
    float start_angle;      // Starting angle in radians
    float end_angle;        // Ending angle in radians
} draw_arc_dsc_t;
static void draw_arc(draw_ctx_t *ctx, const draw_arc_dsc_t *dsc, const clip_rect_t *clip_rect)
{
    if (ctx == NULL || dsc == NULL) { return; }

    rgba_color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t outer_radius = dsc->outer_radius;
    int32_t inner_radius = dsc->inner_radius;
    int32_t cx = dsc->cx;
    int32_t cy = dsc->cy;
    float start_angle = dsc->start_angle;
    float end_angle = dsc->end_angle;

    // Ensure angles are in the range [0, 2*PI)
    if (start_angle < 0 || start_angle >= 2 * M_PI) { return; }
    if (end_angle < 0 || end_angle >= 2 * M_PI) { return; }

    rgba_color_t *buf_rgba = (rgba_color_t *)ctx->buf;
    uint16_t *buf_rgb565 = (uint16_t *)ctx->buf;
    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    float inv_samples = 1.0f / GRID_SAMPLES;
    color_format_t format = ctx->format;

    for (int32_t y = 0; y < buf_h; y++)
    {
        for (int32_t x = 0; x < buf_w; x++)
        {
            if (clip_rect != NULL && (x < clip_rect->x || x >= clip_rect->x + clip_rect->width ||
                                      y < clip_rect->y || y >= clip_rect->y + clip_rect->height))
            {
                continue;
            }

            float dx = x - cx;
            float dy = y - cy;
            float distance = sqrtf(dx * dx + dy * dy);
            float angle = atan2f(dy, dx);
            if (angle < 0)
            {
                angle += 2 * M_PI;
            }

            if (angle >= start_angle && angle <= end_angle)
            {
                // If the pixel is completely inside the outer radius and outside the inner radius
                if (distance <= outer_radius - 1.0f && distance >= inner_radius + 1.0f)
                {
                    int32_t buf_index = y * buf_w + x;
                    if (format == COLOR_FORMAT_RGBA)
                    {
                        buf_rgba[buf_index] = color_mix_rgba(color, ((rgba_color_t *)buf_rgba)[buf_index], opa);
                    }
                    else if (format == COLOR_FORMAT_RGB565)
                    {
                        buf_rgb565[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                 ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                 opa);
                    }
                }
                // Perform anti-aliasing on the edges of the ring
                else if ((distance > inner_radius - 1.0f && distance < inner_radius + 1.0f) ||
                         (distance > outer_radius - 1.0f && distance < outer_radius + 1.0f) ||
                         (fabsf(angle - start_angle) < 1e-3) || (fabsf(angle - end_angle) < 1e-3))
                {
                    int inside_count = 0;

                    for (int i = 0; i < GRID_SAMPLES; i++)
                    {
                        for (int j = 0; j < GRID_SAMPLES; j++)
                        {
                            float sub_x = x + (i + 0.5f) * inv_samples;
                            float sub_y = y + (j + 0.5f) * inv_samples;
                            float sub_dx = sub_x - cx;
                            float sub_dy = sub_y - cy;
                            float sub_distance = sqrtf(sub_dx * sub_dx + sub_dy * sub_dy);
                            float sub_angle = atan2f(sub_dy, sub_dx);
                            if (sub_angle < 0)
                            {
                                sub_angle += 2 * M_PI;
                            }

                            if (sub_distance <= outer_radius && sub_distance >= inner_radius &&
                                sub_angle >= start_angle && sub_angle <= end_angle)
                            {
                                inside_count++;
                            }
                        }
                    }

                    float coverage_ratio = inside_count * inv_samples * inv_samples;
                    if (coverage_ratio > 0)
                    {
                        int32_t buf_index = y * buf_w + x;
                        uint8_t mixed_opa = opa * coverage_ratio;

                        if (format == COLOR_FORMAT_RGBA)
                        {
                            buf_rgba[buf_index] = color_mix_rgba(color, ((rgba_color_t *)buf_rgba)[buf_index], mixed_opa);
                        }
                        else if (format == COLOR_FORMAT_RGB565)
                        {
                            buf_rgb565[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                     ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                     mixed_opa);
                        }
                    }
                }
            }
        }
    }
}
static void draw_arc_save(draw_ctx_t *ctx, const draw_arc_dsc_t *dsc)
{
    if (ctx == NULL || dsc == NULL) { return; }

    rgba_color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t outer_radius = dsc->outer_radius;
    int32_t inner_radius = dsc->inner_radius;
    int32_t cx = dsc->cx;
    int32_t cy = dsc->cy;
    float start_angle = dsc->start_angle;
    float end_angle = dsc->end_angle;

    // Ensure angles are in the range [0, 2*PI)
    if (start_angle < 0 || start_angle >= 2 * M_PI) { return; }
    if (end_angle < 0 || end_angle >= 2 * M_PI) { return; }

    rgba_color_t *buf_rgba = (rgba_color_t *)ctx->buf;
    uint16_t *buf_rgb565 = (uint16_t *)ctx->buf;
    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    float inv_samples = 1.0f / GRID_SAMPLES;
    color_format_t format = ctx->format;

    for (int32_t y = 0; y < buf_h; y++)
    {
        for (int32_t x = 0; x < buf_w; x++)
        {


            float dx = x - cx;
            float dy = y - cy;
            float distance = sqrtf(dx * dx + dy * dy);
            float angle = atan2f(dy, dx);
            if (angle < 0)
            {
                angle += 2 * M_PI;
            }

            if (angle >= start_angle && angle <= end_angle)
            {
                // If the pixel is completely inside the outer radius and outside the inner radius
                if (distance <= outer_radius - 1.0f && distance >= inner_radius + 1.0f)
                {
                    int32_t buf_index = y * buf_w + x;

                    {
                        buf_rgba[buf_index] = color;
                    }

                }
                // Perform anti-aliasing on the edges of the ring
                else if ((distance > inner_radius - 1.0f && distance < inner_radius + 1.0f) ||
                         (distance > outer_radius - 1.0f && distance < outer_radius + 1.0f)
                        )
                {
                    int inside_count = 0;

                    for (int i = 0; i < GRID_SAMPLES; i++)
                    {
                        for (int j = 0; j < GRID_SAMPLES; j++)
                        {
                            float sub_x = x + (i + 0.5f) * inv_samples;
                            float sub_y = y + (j + 0.5f) * inv_samples;
                            float sub_dx = sub_x - cx;
                            float sub_dy = sub_y - cy;
                            float sub_distance = sqrtf(sub_dx * sub_dx + sub_dy * sub_dy);
                            // float sub_angle = atan2f(sub_dy, sub_dx);
                            // if (sub_angle < 0)
                            // {
                            //     sub_angle += 2 * M_PI;
                            // }

                            if (sub_distance <= outer_radius && sub_distance >= inner_radius)
                            {
                                inside_count++;
                            }
                        }
                    }

                    float coverage_ratio = inside_count * inv_samples * inv_samples;
                    if (coverage_ratio > 0)
                    {
                        int32_t buf_index = y * buf_w + x;

                        rgba_color_t color_temp = color;
                        color_temp.a = opa * coverage_ratio;
                        // Mix colors based on coverage ratio
                        {
                            buf_rgba[buf_index] = color_temp;
                        }


                    }
                }
            }
        }
    }
}
static void *renderer_save_arc(int r_in, int r_out, float start, float end, gui_color_t color)
{
    struct gui_dispdev *screen = gui_get_dc();
    int32_t buf_width = r_out;
    int32_t buf_height = r_out;
    uint8_t *file = gui_malloc(buf_width * buf_height * 4 + sizeof(gui_rgb_data_head_t));
    memset(file, 0, buf_width * buf_height * 4 + sizeof(gui_rgb_data_head_t));
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)file;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = ARGB8888;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = buf_width;
    head->h = buf_height;
    color_t *buffer = (color_t *)(file + sizeof(gui_rgb_data_head_t));
    draw_ctx_t draw_ctx = {COLOR_FORMAT_RGBA, buffer, buf_width, buf_height};
    int x = 0, y = 0;
    draw_arc_dsc_t arc_dsc =
    {
        .color = {color.color.rgba.b, color.color.rgba.g, color.color.rgba.r, color.color.rgba.a},
        .opa = 255,
        .outer_radius = r_out,
        .inner_radius = r_in, // For example, you can pass it through widget too
        .cx = 0,
        .cy = 0,
        .start_angle = start, // Pass start angle
        .end_angle = end      // Pass end angle
    };

    draw_arc_save(&draw_ctx, &arc_dsc);
    return file;
}
static void sector_renderer(gui_sector_t *widget)
{
    struct gui_dispdev *screen = gui_get_dc();
    int32_t buf_width = screen->fb_width;
    int32_t buf_height = screen->fb_height;
    color_t *buffer = (color_t *)screen->frame_buf;
    draw_ctx_t draw_ctx = {COLOR_FORMAT_RGB565, buffer, buf_width, buf_height};
    if (screen->bit_depth == 32)
    {
        draw_ctx.format = COLOR_FORMAT_RGBA;
    }
    int x = 0, y = 0;
    gui_obj_absolute_xy(GUI_BASE(widget), &x, &y);
    uint16_t color_rgb565 = RGB565(widget->base.fill.color.rgba.r, widget->base.fill.color.rgba.g,
                                   widget->base.fill.color.rgba.b);
    sector_draw_shape_dsc_t sector_dsc =
    {
        .color = {widget->base.fill.color.rgba.b, widget->base.fill.color.rgba.g, widget->base.fill.color.rgba.r, widget->base.fill.color.rgba.a},   // Sector color
        .opa = 255,                  // Sector opacity
        .radius = GUI_BASE(widget)->w / 2,        // Sector radius
        .cx = GUI_BASE(widget)->w / 2 + x,        // Circle center X coordinate
        .cy = GUI_BASE(widget)->w / 2 + y - screen->fb_height * screen->section_count,    // Circle center Y coordinate
        .end_angle = widget->end_angle,
        .start_angle = widget->start_angle,
        .color_rgb565 = {
            .rgb565 = color_rgb565,
            .b = color_rgb565 & 0x001F,
            .g = color_rgb565 & 0x07E0,
            .r = color_rgb565 & 0xF800,
        }
    };
// Set the clipping area
    clip_rect_t clip_area =
    {
        .x = GUI_BASE(widget)->x,   // X-coordinate of the top-left corner of the clipping area
        .y = GUI_BASE(widget)->y,   // Y-coordinate of the top-left corner of the clipping area
        .width = GUI_BASE(widget)->w, // Width of the clipping area
        .height = GUI_BASE(widget)->h  // Height of the clipping area
    };
    if (screen->section_count)
    {
        clip_area.y = 0;
        clip_area.height = screen->fb_height;
    }
    // Call the function to draw the sector
    draw_circle_or_sector(&draw_ctx, &sector_dsc, &clip_area);
}

static void pipeline(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            prepare((void *)obj);
            break;

        case OBJ_DRAW:
            renderer((void *)obj);
            break;

        default:
            break;
        }
    }
}

static void sector_pipeline(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            prepare((void *)obj);
            break;

        case OBJ_DRAW:
            sector_renderer((void *)obj);
            break;

        default:
            break;
        }
    }
}



// Check if a point is within a circle
int is_within_round(int32_t x, int32_t y, int32_t cx, int32_t cy, int32_t radius)
{
    float dx = x - cx;
    float dy = y - cy;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// Function to draw rounded rectangle with clipping region
void draw_rounded_rectangle(draw_ctx_t *ctx, const rounded_rect_draw_shape_dsc_t *dsc,
                            const clip_rect_t *clip)
{
    if (ctx == NULL || dsc == NULL || clip == NULL) { return; }

    rgba_color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t radius = dsc->radius;
    int32_t x0 = dsc->rect_x;
    int32_t y0 = dsc->rect_y;
    int32_t w = dsc->rect_w;
    int32_t h = dsc->rect_h;

    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    // Calculate the interval between each sample point
    float inv_samples = 1.0f / GRID_SAMPLES;

    // Define the center coordinates of the four corners
    int32_t cx[] = { x0 + radius,              x0 + w - radius,           x0 + radius,              x0 + w - radius };
    int32_t cy[] = { y0 + radius,              y0 + radius,               y0 + h - radius,          y0 + h - radius };

    // Determine the color format of the output buffer
    color_format_t format = ctx->format;

    // Iterate over each pixel in the buffer
    for (int32_t y = clip->y; y < clip->y + clip->height; y++)
    {
        if (y >= buf_h) { break; }  // Prevent out of bounds
        for (int32_t x = clip->x; x < clip->x + clip->width; x++)
        {
            if (x >= buf_w) { break; }  // Prevent out of bounds
            int32_t buf_index = y * buf_w + x;

            // Check if the current pixel is within the main rectangle area
            if ((x >= x0 + radius && x < x0 + w - radius && y >= y0 && y < y0 + h) ||
                (y >= y0 + radius && y < y0 + h - radius && x >= x0 && x < x0 + w))
            {
                if (format == COLOR_FORMAT_RGBA)
                {
                    if (opa == 255)
                    {
                        ((rgba_color_t *)ctx->buf)[buf_index] = color_mix_rgba(color, color, opa);
                    }
                    else
                    {
                        ((rgba_color_t *)ctx->buf)[buf_index] = color_mix_rgba(color, ((rgba_color_t *)ctx->buf)[buf_index],
                                                                               opa);
                    }
                }
                else if (format == COLOR_FORMAT_RGB565)
                {
                    if (opa == 255)
                    {
                        ((uint16_t *)ctx->buf)[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565, 0,
                                                                             dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b, opa);
                    }
                    else
                    {
                        ((uint16_t *)ctx->buf)[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                             ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                             opa);
                    }

                }
                continue;
            }

            // Check if the current pixel is within the four corner regions
            int within_corner = 0;
            for (int i = 0; i < 4; i++)
            {
                float dx = x - cx[i] + 0.5f;
                float dy = y - cy[i] + 0.5f;
                float distance = sqrtf(dx * dx + dy * dy);

                if (distance <= radius - 0.5f)
                {
                    if (format == COLOR_FORMAT_RGBA)
                    {
                        ((rgba_color_t *)ctx->buf)[buf_index] = color_mix_rgba(color, ((rgba_color_t *)ctx->buf)[buf_index],
                                                                               opa);
                    }
                    else if (format == COLOR_FORMAT_RGB565)
                    {
                        ((uint16_t *)ctx->buf)[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                             ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                             opa);
                    }
                    within_corner = 1;
                    break;
                }
                else if (distance <= radius + 0.5f)
                {
                    int inside_count = 0;

                    // Perform grid sampling for the current pixel
                    for (int m = 0; m < GRID_SAMPLES; m++)
                    {
                        for (int n = 0; n < GRID_SAMPLES; n++)
                        {
                            float sub_x = x + (m + 0.5f) * inv_samples;
                            float sub_y = y + (n + 0.5f) * inv_samples;
                            float sub_dx = sub_x - cx[i];
                            float sub_dy = sub_y - cy[i];
                            float sub_distance = sqrtf(sub_dx * sub_dx + sub_dy * sub_dy);
                            if (sub_distance <= radius)
                            {
                                inside_count++;
                            }
                        }
                    }

                    float coverage_ratio = inside_count * inv_samples * inv_samples;
                    if (coverage_ratio > 0)
                    {
                        uint8_t mixed_opa = opa * coverage_ratio;
                        if (format == COLOR_FORMAT_RGBA)
                        {
                            ((rgba_color_t *)ctx->buf)[buf_index] = color_mix_rgba(color, ((rgba_color_t *)ctx->buf)[buf_index],
                                                                                   mixed_opa);
                        }
                        else if (format == COLOR_FORMAT_RGB565)
                        {
                            ((uint16_t *)ctx->buf)[buf_index] = color_mix_rgb565(dsc->color_rgb565.rgb565,
                                                                                 ((uint16_t *)ctx->buf)[buf_index], dsc->color_rgb565.r, dsc->color_rgb565.g, dsc->color_rgb565.b,
                                                                                 mixed_opa);
                        }
                    }
                    within_corner = 1;
                    break;
                }
            }

            if (within_corner) { continue; }
        }
    }
}


static void rounded_rect_renderer(gui_rounded_rect_t *widget)
{
    struct gui_dispdev *screen = gui_get_dc();
    int32_t buf_width = screen->fb_width;
    int32_t buf_height = screen->fb_height;
    color_t *buffer = (color_t *)screen->frame_buf;
    draw_ctx_t draw_ctx = {COLOR_FORMAT_RGB565, buffer, buf_width, buf_height};
    if (screen->bit_depth == 32)
    {
        draw_ctx.format = COLOR_FORMAT_RGBA;
    }
    int x = 0, y = 0;
    gui_obj_absolute_xy(GUI_BASE(widget), &x, &y);
    uint16_t color_rgb565 = RGB565(widget->base.fill.color.rgba.r, widget->base.fill.color.rgba.g,
                                   widget->base.fill.color.rgba.b);
    rounded_rect_draw_shape_dsc_t rounded_rect_dsc =
    {
        .color = {widget->base.fill.color.rgba.b, widget->base.fill.color.rgba.g, widget->base.fill.color.rgba.r, widget->base.fill.color.rgba.a},   // Color of the rounded rectangle
        .opa = 255,                // Opacity of the rounded rectangle
        .radius = widget->radius,  // Radius of the rounded corners
        .rect_x = GUI_BASE(widget)->x, // X-coordinate of the top-left corner of the rectangle
        .rect_y = GUI_BASE(widget)->y - screen->fb_height * screen->section_count, // Y-coordinate of the top-left corner of the rectangle
        .rect_w = GUI_BASE(widget)->w, // Width of the rectangle
        .rect_h = GUI_BASE(widget)->h,  // Height of the rectangle
        .color_rgb565 = {
            .rgb565 = color_rgb565,
            .b = color_rgb565 & 0x001F,
            .g = color_rgb565 & 0x07E0,
            .r = color_rgb565 & 0xF800,
        }
    };
    // Set the clipping area
    clip_rect_t clip_area =
    {
        .x = GUI_BASE(widget)->x,   // X-coordinate of the top-left corner of the clipping area
        .y = GUI_BASE(widget)->y,   // Y-coordinate of the top-left corner of the clipping area
        .width = GUI_BASE(widget)->w, // Width of the clipping area
        .height = GUI_BASE(widget)->h  // Height of the clipping area
    };
    if (screen->section_count)
    {
        clip_area.y = 0;
        clip_area.height = screen->fb_height;
    }


    // Call the function to draw the rounded rectangle, effective only within the clipping area
    draw_rounded_rectangle(&draw_ctx, &rounded_rect_dsc, &clip_area);
}

static void rounded_rect_pipeline(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            prepare((void *)obj);
            break;

        case OBJ_DRAW:
            rounded_rect_renderer((void *)obj);
            break;


        default:
            break;
        }
    }
}



static void ctor(gui_circle_t *widget, gui_obj_t *parent, int r, int cx, int cy,
                 gui_color_t fill_color)
{
    gui_obj_ctor(GUI_BASE(widget), parent, "calendar", cx - r, cy - r, r * 2, r * 2);
    GET_BASE(widget)->type = MACRO_CIRCLE;
    GET_BASE(widget)->obj_cb = pipeline;
    GET_BASE(widget)->has_prepare_cb = true;
    GET_BASE(widget)->has_draw_cb = true;
    widget->fill = fill_color;
}
static void ctor_base(gui_circle_t *widget, gui_obj_t *parent, int x, int y, int w, int h,
                      gui_color_t fill_color)
{
    gui_obj_ctor(GUI_BASE(widget), parent, "calendar", x, y, w, h);
    GET_BASE(widget)->type = MACRO_CIRCLE;
    GET_BASE(widget)->obj_cb = pipeline;
    GET_BASE(widget)->has_prepare_cb = true;
    GET_BASE(widget)->has_draw_cb = true;
    widget->fill = fill_color;
}
static void sector_ctor(gui_sector_t *widget, gui_obj_t *parent, int r, int cx, int cy,
                        gui_color_t fill_color, float start_angle, float end_angle)
{
    ctor(&widget->base, parent, r, cx, cy, fill_color);
    GET_BASE(widget)->type = MACRO_SECTOR;
    GET_BASE(widget)->obj_cb = sector_pipeline;
    widget->end_angle = end_angle;
    widget->start_angle = start_angle;
}
static void rounded_rect_ctor(gui_rounded_rect_t *widget, gui_obj_t *parent, int x, int y, int w,
                              int h, int radius, gui_color_t fill_color)
{
    ctor_base(&widget->base, parent, x, y, w, h, fill_color);
    GET_BASE(widget)->type = MACRO_ROUNDED_RECT;
    GET_BASE(widget)->obj_cb = rounded_rect_pipeline;
    widget->radius = radius;
}

gui_circle_t *gui_circle_create(gui_obj_t *parent, int r, int cx, int cy, gui_color_t fill_color)
{
#define _GUI_NEW_GUI_CIRCLE_PARAM this, parent, r, cx, cy, fill_color
    GUI_CREATE_HELPER(gui_circle_t, ctor, _GUI_NEW_GUI_CIRCLE_PARAM)
    return this;
}
gui_sector_t *gui_sector_create(gui_obj_t *parent, int r, int cx, int cy, gui_color_t fill_color,
                                float start_angle, float end_angle)
{
#define _GUI_NEW_GUI_SECTOR_PARAM this, parent, r, cx, cy, fill_color, start_angle, end_angle
    GUI_CREATE_HELPER(gui_sector_t, sector_ctor, _GUI_NEW_GUI_SECTOR_PARAM)
    return this;
}
gui_rounded_rect_t *gui_rounded_rect_create(gui_obj_t *parent, int x, int y, int w, int h,
                                            int radius, gui_color_t fill_color)
{
#define _GUI_NEW_GUI_ROUNDED_RECT_PARAM this, parent,  x, y, w, h, radius, fill_color
    GUI_CREATE_HELPER(gui_rounded_rect_t, rounded_rect_ctor, _GUI_NEW_GUI_ROUNDED_RECT_PARAM)
    return this;
}
