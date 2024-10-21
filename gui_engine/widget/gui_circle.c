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

// Drawing context
typedef struct
{
    color_t *buf;
    int32_t buf_width;
    int32_t buf_height;
} draw_ctx_t;

// Circle drawing descriptor
typedef struct
{
    color_t color;
    uint8_t opa;
    int32_t radius;
    int32_t cx;
    int32_t cy;
} draw_circle_dsc_t;

// Circle or sector drawing descriptor
typedef struct
{
    color_t color;
    uint8_t opa;
    int32_t radius;
    int32_t cx;
    int32_t cy;
    float start_angle; // Starting angle of the sector (radians)
    float end_angle;   // Ending angle of the sector (radians)
} sector_draw_shape_dsc_t;

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

// Determining if an angle is within a sector

// Function to draw a circle
static void draw_circle(draw_ctx_t *ctx, const draw_circle_dsc_t *dsc)
{
    if (ctx == NULL || dsc == NULL) { return; }

    color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t radius = dsc->radius;
    int32_t cx = dsc->cx;
    int32_t cy = dsc->cy;

    color_t *buf = ctx->buf;
    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    // Calculate the interval between each sample point
    float inv_samples = 1.0f / GRID_SAMPLES;

    // Iterate over each pixel in the buffer
    for (int32_t y = 0; y < buf_h; y++)
    {
        for (int32_t x = 0; x < buf_w; x++)
        {
            // Calculate the distance from the center of this pixel to the center of the circle
            float dx = x - cx;
            float dy = y - cy;
            float distance = sqrtf(dx * dx + dy * dy);

            // If completely inside the circle, fill the color directly, no need for anti-aliasing
            if (distance <= radius - 1.0f)
            {
                int32_t buf_index = y * buf_w + x;
                buf[buf_index] = color_mix(color, buf[buf_index], opa);
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
                    buf[buf_index] = color_mix(color, buf[buf_index], mixed_opa);
                }
            }
        }
    }
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

// Function to draw a circle or sector
void draw_circle_or_sector(draw_ctx_t *ctx, const sector_draw_shape_dsc_t *dsc)
{
    if (ctx == NULL || dsc == NULL) { return; }

    color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t radius = dsc->radius;
    int32_t cx = dsc->cx;
    int32_t cy = dsc->cy;
    float start_angle = dsc->start_angle;
    float end_angle = dsc->end_angle;

    color_t *buf = ctx->buf;
    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    // Calculate the interval between each sample point
    float inv_samples = 1.0f / GRID_SAMPLES;

    // Iterate over each pixel in the buffer
    for (int32_t y = 0; y < buf_h; y++)
    {
        for (int32_t x = 0; x < buf_w; x++)
        {
            // Calculate the distance from the center of this pixel to the center of the circle
            float dx = x - cx + 0.5f;
            float dy = y - cy + 0.5f;
            float distance = sqrtf(dx * dx + dy * dy);

            // Calculate the angle of this pixel center
            float angle = atan2f(dy, dx);
            if (angle < 0) { angle += 2 * M_PI; }

            // Check if the current pixel is within the sector's range
            int within_sector = is_within_sector(angle, start_angle, end_angle);

            // If completely inside the circle (and within the sector's range), fill the color directly, no need for anti-aliasing
            if (distance <= radius - 0.5f && within_sector)
            {
                int32_t buf_index = y * buf_w + x;
                buf[buf_index] = color_mix(color, buf[buf_index], opa);
            }
            // Otherwise, perform anti-aliasing
            else if (distance <= radius + 0.5f)
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

                        // Calculate the distance and angle of the sub-pixel from the center of the circle
                        float sub_dx = sub_x - cx;
                        float sub_dy = sub_y - cy;
                        float sub_distance = sqrtf(sub_dx * sub_dx + sub_dy * sub_dy);
                        float sub_angle = atan2f(sub_dy, sub_dx);
                        if (sub_angle < 0) { sub_angle += 2 * M_PI; }

                        // Check if the sub-pixel is within the circle and the sector's range
                        if (sub_distance <= radius && is_within_sector(sub_angle, start_angle, end_angle))
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
                    buf[buf_index] = color_mix(color, buf[buf_index], mixed_opa);
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
    draw_ctx_t draw_ctx = {buffer, buf_width, buf_height};
    int x = 0, y = 0;
    gui_obj_absolute_xy(GUI_BASE(widget), &x, &y);
    draw_circle_dsc_t circle_dsc =
    {
        .color = {widget->fill.color.rgba.b, widget->fill.color.rgba.g, widget->fill.color.rgba.r, widget->fill.color.rgba.a},   // Circle color
        .opa = 255,                  // Circle opacity
        .radius = GUI_BASE(widget)->w / 2,         // Circle radius
        .cx = GUI_BASE(widget)->w / 2 + x,         // Circle center X coordinate
        .cy = GUI_BASE(widget)->w / 2 + y          // Circle center Y coordinate
    };
    // Call the function to draw the circle
    draw_circle(&draw_ctx, &circle_dsc);
}

static void sector_renderer(gui_sector_t *widget)
{
    struct gui_dispdev *screen = gui_get_dc();
    int32_t buf_width = screen->fb_width;
    int32_t buf_height = screen->fb_height;
    color_t *buffer = (color_t *)screen->frame_buf;
    draw_ctx_t draw_ctx = {buffer, buf_width, buf_height};
    int x = 0, y = 0;
    gui_obj_absolute_xy(GUI_BASE(widget), &x, &y);
    sector_draw_shape_dsc_t sector_dsc =
    {
        .color = {widget->base.fill.color.rgba.b, widget->base.fill.color.rgba.g, widget->base.fill.color.rgba.r, widget->base.fill.color.rgba.a},   // Sector color
        .opa = 255,                  // Sector opacity
        .radius = GUI_BASE(widget)->w / 2,        // Sector radius
        .cx = GUI_BASE(widget)->w / 2 + x,        // Circle center X coordinate
        .cy = GUI_BASE(widget)->w / 2 + y,        // Circle center Y coordinate
        .end_angle = widget->end_angle,
        .start_angle = widget->start_angle,
    };

    // Call the function to draw the sector
    draw_circle_or_sector(&draw_ctx, &sector_dsc);
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

// Rounded rectangle drawing descriptor
typedef struct
{
    color_t color;
    uint8_t opa;
    int32_t radius;
    int32_t rect_x;
    int32_t rect_y;
    int32_t rect_w;
    int32_t rect_h;
} rounded_rect_draw_shape_dsc_t;

// Clipping region structure
typedef struct
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} clip_rect_t;

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

    color_t color = dsc->color;
    uint8_t opa = dsc->opa;
    int32_t radius = dsc->radius;
    int32_t x0 = dsc->rect_x;
    int32_t y0 = dsc->rect_y;
    int32_t w = dsc->rect_w;
    int32_t h = dsc->rect_h;

    color_t *buf = ctx->buf;
    int32_t buf_w = ctx->buf_width;
    int32_t buf_h = ctx->buf_height;

    // Calculate the interval between each sample point
    float inv_samples = 1.0f / GRID_SAMPLES;

    // Define the center coordinates of the four corners
    int32_t cx[] = { x0 + radius,              x0 + w - radius,           x0 + radius,              x0 + w - radius };
    int32_t cy[] = { y0 + radius,              y0 + radius,               y0 + h - radius,          y0 + h - radius };

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
                buf[buf_index] = color_mix(color, buf[buf_index], opa);
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
                    buf[buf_index] = color_mix(color, buf[buf_index], opa);
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
                        buf[buf_index] = color_mix(color, buf[buf_index], mixed_opa);
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
    draw_ctx_t draw_ctx = {buffer, buf_width, buf_height};
    int x = 0, y = 0;
    gui_obj_absolute_xy(GUI_BASE(widget), &x, &y);
    rounded_rect_draw_shape_dsc_t rounded_rect_dsc =
    {
        .color = {widget->base.fill.color.rgba.b, widget->base.fill.color.rgba.g, widget->base.fill.color.rgba.r, widget->base.fill.color.rgba.a},   // Color of the rounded rectangle
        .opa = 200,                // Opacity of the rounded rectangle
        .radius = widget->radius,  // Radius of the rounded corners
        .rect_x = GUI_BASE(widget)->x, // X-coordinate of the top-left corner of the rectangle
        .rect_y = GUI_BASE(widget)->y, // Y-coordinate of the top-left corner of the rectangle
        .rect_w = GUI_BASE(widget)->w, // Width of the rectangle
        .rect_h = GUI_BASE(widget)->h  // Height of the rectangle
    };
    // Set the clipping area
    clip_rect_t clip_area =
    {
        .x = GUI_BASE(widget)->x,   // X-coordinate of the top-left corner of the clipping area
        .y = GUI_BASE(widget)->y,   // Y-coordinate of the top-left corner of the clipping area
        .width = GUI_BASE(widget)->w, // Width of the clipping area
        .height = GUI_BASE(widget)->h  // Height of the clipping area
    };
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
