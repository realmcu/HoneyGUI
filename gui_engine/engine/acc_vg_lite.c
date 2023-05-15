#include <draw_img.h>
#include <stdio.h>
#include <gui_matrix.h>
#include <vg_lite.h>
#include <acc_engine.h>
#include <stdlib.h>
#include <math.h>

#include "vg_lite.h"
#include "vg_lite_platform.h"

#define PI                           3.141592653589793238462643383279502f
#define SINF(x)                      ((vg_lite_float_t) sin(x))
#define COSF(x)                      ((vg_lite_float_t) cos(x))
#define _UI_MIN(x, y)                (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)                (((x)>(y))?(x):(y))
#define BEZIEL_CIRCLE_FIT_FACTOR     0.552284749831f

typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint8_t A;
        uint8_t R;
        uint8_t G;
        uint8_t B;
    } channel;
} ARGB_struct;

typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint8_t A;
        uint8_t B;
        uint8_t G;
        uint8_t R;
    } channel;
} ABGR_struct;

typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint8_t B;
        uint8_t G;
        uint8_t R;
        uint8_t A;
    } channel;
} BGRA_struct;

typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;
    } channel;
} RGBA_struct;

static float clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }
static float hue(float h, float m1, float m2)
{
    if (h < 0) { h += 1; }
    if (h > 1) { h -= 1; }
    if (h < 1.0f / 6.0f)
    {
        return m1 + (m2 - m1) * h * 6.0f;
    }
    else if (h < 3.0f / 6.0f)
    {
        return m2;
    }
    else if (h < 4.0f / 6.0f)
    {
        return m1 + (m2 - m1) * (2.0f / 3.0f - h) * 6.0f;
    }
    return m1;
}

static BGRA_struct HSLA(float h, float s, float l, unsigned char a)
{
    float m1, m2;
    BGRA_struct col;
    h = fmodf(h, 1.0f);
    if (h < 0.0f) { h += 1.0f; }
    s = clampf(s, 0.0f, 1.0f);
    l = clampf(l, 0.0f, 1.0f);
    m2 = l <= 0.5f ? (l * (1 + s)) : (l + s - l * s);
    m1 = 2 * l - m2;
    col.channel.R = a * clampf(hue(h + 1.0f / 3.0f, m1, m2), 0.0f, 1.0f);
    col.channel.G = a * clampf(hue(h, m1, m2), 0.0f, 1.0f);
    col.channel.B = a * clampf(hue(h - 1.0f / 3.0f, m1, m2), 0.0f, 1.0f);
    col.channel.A = a;
    return col;
}

void hw_draw_arc(canvas_arc_t *a, struct gui_dispdev *dc)
{
    if (a->start_angle == a->end_angle)
    {
        return;
    }
    vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;

    float scale_ratio = 1;
    float stroke_width = a->stroke.stroke_width * 1.0f;
    if (a->stroke.stroke_width > 2.5)
    {
        scale_ratio = a->stroke.stroke_width * 1.0f / 2.5;
        stroke_width = 2.5;
    }

    float radius = a->r / scale_ratio;// + a->stroke.stroke_width * 1.0f;
    float start_angle = a->start_angle - floor(a->start_angle / 360.0f) * 360;
    float end_angle = a->end_angle - floor(a->end_angle / 360.0f) * 360;
    if (start_angle > end_angle)
    {
        end_angle += 360.0f;
    }
    float angle_diff = end_angle - start_angle;
    float end_x = 1.0f * radius * COSF((angle_diff - 90) / 180.0f * PI);
    float end_y = 1.0f * radius * SINF((angle_diff - 90) / 180.0f * PI);

    ARGB_struct in_color = (ARGB_struct)a->stroke.fill.color_data.rgba;
    BGRA_struct out_color = {.d32 = 0};
    out_color.channel.A = in_color.channel.A;
    out_color.channel.R = in_color.channel.R * (1.0f * in_color.channel.A / 255);
    out_color.channel.G = in_color.channel.G * (1.0f * in_color.channel.A / 255);
    out_color.channel.B = in_color.channel.B * (1.0f * in_color.channel.A / 255);

    vg_lite_path_t arc_path;
    memset(&arc_path, 0x00, sizeof(vg_lite_path_t));

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(a->cx, a->cy, &matrix);
    vg_lite_scale(scale_ratio, scale_ratio, &matrix);
    vg_lite_rotate(start_angle, &matrix);

    vg_lite_float_t dash[2] = {0, 0};
    uint8_t circle_cmd[] = {VLC_OP_MOVE,
                            VLC_OP_SCWARC,
                           };
    float l_circle_data[] =
    {
        0, 0 - radius,
        radius, radius, 0, end_x, end_y,
        radius, radius, 0, 0, -radius,
    };
    if (angle_diff > 180)
    {
        circle_cmd[1] = VLC_OP_LCWARC;
    }
    uint32_t data_size = vg_lite_path_calc_length(circle_cmd, sizeof(circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&arc_path, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -target.width,
                      -target.width, target.width, target.width);
    void *path_data = gui_malloc(data_size);
    arc_path.path = path_data;
    vg_lite_path_append(&arc_path, circle_cmd, l_circle_data, sizeof(circle_cmd));
    if ((a->stroke.stroke_width == 0) || (a->stroke.fill.color_data.rgba == 0))
    {
        return;
    }
    else if ((a->stroke.stroke_width == 1))
    {
        vg_lite_draw(&target, &arc_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE,
                     out_color.d32);
    }
    else
    {
        vg_lite_set_stroke(&arc_path, (vg_lite_cap_style_t)a->stroke.stroke_linecap, VG_LITE_JOIN_MITER, \
                           stroke_width, 0, dash, 0, 0, out_color.d32);
        vg_lite_update_stroke(&arc_path);
        vg_lite_set_draw_path_type(&arc_path, VG_LITE_DRAW_STROKE_PATH);
        vg_lite_draw(&target, &arc_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE,
                     out_color.d32);
    }
    vg_lite_finish();
    vg_lite_clear_path(&arc_path);
    gui_free(path_data);
    if (a->counterclockwise)
    {

    }

}

void hw_draw_circle(canvas_circle_t *circle, struct gui_dispdev *dc)
{
    vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;

    float scale_ratio = 1;
    float stroke_width = circle->stroke.stroke_width * 1.0f;
    if (circle->stroke.stroke_width > 2.5)
    {
        scale_ratio = circle->stroke.stroke_width * 1.0f / 2.5;
        stroke_width = 2.5;
    }
    float radius = circle->r / scale_ratio;

    uint8_t circle_cmd[] = {    VLC_OP_MOVE,
                                VLC_OP_SCWARC,
                                VLC_OP_SCWARC,
                                VLC_OP_SCWARC,
                                VLC_OP_SCWARC,
                                VLC_OP_END,
                           };

    float circle_data[] =
    {
        circle->cx, circle->cy - radius,
        radius, radius, 0, radius + circle->cx, 0 + circle->cy,
        radius, radius, 0, 0 + circle->cx, radius + circle->cy,
        radius, radius, 0, -radius + circle->cx, 0 + circle->cy,
        radius, radius, 0, 0 + circle->cx, -radius + circle->cy,
    };

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_scale(scale_ratio, scale_ratio, &matrix);

    ARGB_struct in_color = {.d32 = circle->fill.color_data.rgba};
    BGRA_struct fill_color = {.d32 = 0};
    fill_color.channel.A = in_color.channel.A;
    fill_color.channel.R = in_color.channel.R * (1.0f * in_color.channel.A / 255);
    fill_color.channel.G = in_color.channel.G * (1.0f * in_color.channel.A / 255);
    fill_color.channel.B = in_color.channel.B * (1.0f * in_color.channel.A / 255);

    in_color.d32 = circle->stroke.fill.color_data.rgba;
    BGRA_struct stroke_color = {.d32 = 0};
    stroke_color.channel.A = in_color.channel.A;
    stroke_color.channel.R = in_color.channel.R * (1.0f * in_color.channel.A / 255);
    stroke_color.channel.G = in_color.channel.G * (1.0f * in_color.channel.A / 255);
    stroke_color.channel.B = in_color.channel.B * (1.0f * in_color.channel.A / 255);

    vg_lite_path_t circle_path;
    uint32_t data_len = vg_lite_path_calc_length(circle_cmd, sizeof(circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&circle_path, VG_LITE_FP32, VG_LITE_HIGH, data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *path_data = gui_malloc(data_len);
    circle_path.path = path_data;
    vg_lite_path_append(&circle_path, circle_cmd, circle_data, sizeof(circle_cmd));
    if (((circle->stroke.stroke_width == 0) || (circle->stroke.fill.color_data.rgba == 0)) &&
        (circle->fill.color_data.rgba != 0))
    {
        vg_lite_draw(&target, &circle_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE,
                     fill_color.d32);
    }
    else if ((circle->stroke.stroke_width != 0) && (circle->stroke.fill.color_data.rgba != 0))
    {
        vg_lite_set_stroke(&circle_path, (vg_lite_cap_style_t)circle->stroke.stroke_linecap,
                           (vg_lite_join_style_t)circle->stroke.stroke_linejoin, stroke_width,
                           circle->stroke.miter_limit, circle->stroke.dash, circle->stroke.dash_count,
                           circle->stroke.dash_phase, stroke_color.d32);
        vg_lite_update_stroke(&circle_path);
        if (circle->fill.color_data.rgba != 0)
        {
            vg_lite_set_draw_path_type(&circle_path, VG_LITE_DRAW_FILL_PATH);
            vg_lite_draw(&target, &circle_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE,
                         fill_color.d32);
        }
        vg_lite_set_draw_path_type(&circle_path, VG_LITE_DRAW_STROKE_PATH);
        vg_lite_draw(&target, &circle_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE,
                     stroke_color.d32);
    }
    vg_lite_finish();
    vg_lite_clear_path(&circle_path);
    free(path_data);
}

void hw_draw_line(canvas_line_t *l, struct gui_dispdev *dc)
{
    vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;

    float scale_ratio = 1;
    float stroke_width = l->stroke.stroke_width * 1.0f;
    if (l->stroke.stroke_width > 2.5)
    {
        scale_ratio = l->stroke.stroke_width * 1.0f / 2.5;
        stroke_width = 2.5;
    }

    float x_rel = (l->x2 - l->x1) / scale_ratio;
    float y_rel = (l->y2 - l->y1) / scale_ratio;

    uint8_t line_cmd[] = {    VLC_OP_MOVE,
                              VLC_OP_LINE_REL,
                              VLC_OP_END,
                         };
    float line_data[] =
    {
        0, 0,
        x_rel, y_rel,
    };
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(l->x1, l->y1, &matrix);
    vg_lite_scale(scale_ratio, scale_ratio, &matrix);

    ARGB_struct in_color = {.d32 = in_color.d32 = l->stroke.fill.color_data.rgba};
    BGRA_struct stroke_color = {.d32 = 0};
    stroke_color.channel.A = in_color.channel.A;
    stroke_color.channel.R = in_color.channel.R * (1.0f * in_color.channel.A / 255);
    stroke_color.channel.G = in_color.channel.G * (1.0f * in_color.channel.A / 255);
    stroke_color.channel.B = in_color.channel.B * (1.0f * in_color.channel.A / 255);

    vg_lite_path_t line_path;
    uint32_t data_len = vg_lite_path_calc_length(line_cmd, sizeof(line_cmd), VG_LITE_FP32);
    vg_lite_init_path(&line_path, VG_LITE_FP32, VG_LITE_HIGH, data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *path_data = gui_malloc(data_len);
    line_path.path = path_data;
    vg_lite_path_append(&line_path, line_cmd, line_data, sizeof(line_cmd));
    if ((l->stroke.stroke_width == 0) || (l->stroke.fill.color_data.rgba == 0))
    {
        return;
    }
    else if ((l->stroke.stroke_width == 1))
    {
        vg_lite_draw(&target, &line_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE,
                     stroke_color.d32);
    }
    else
    {
        vg_lite_set_stroke(&line_path, (vg_lite_cap_style_t)l->stroke.stroke_linecap,
                           (vg_lite_join_style_t)l->stroke.stroke_linejoin, stroke_width,
                           stroke_width, l->stroke.dash, l->stroke.dash_count, l->stroke.dash_phase, stroke_color.d32);
        vg_lite_update_stroke(&line_path);
        vg_lite_set_draw_path_type(&line_path, VG_LITE_DRAW_STROKE_PATH);
        vg_lite_draw(&target, &line_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE,
                     stroke_color.d32);
    }
    vg_lite_finish();
    vg_lite_clear_path(&line_path);
    gui_free(path_data);
}

void hw_draw_rectangle(canvas_rectangle_t *r, struct gui_dispdev *dc)
{
    vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;

    float scale_ratio = 1;
    float stroke_width = r->stroke.stroke_width * 1.0f;
    bool has_grad = false;
//    if (r->stroke.stroke_width > 2.5)
//    {
//        scale_ratio = r->stroke.stroke_width * 1.0f / 2.5;
//        stroke_width = 2.5;
//    }

    float x_rel = (r->width) / scale_ratio;
    float y_rel = (r->height) / scale_ratio;
    uint8_t *rect_cmd = gui_malloc(10);
    float *rect_data = gui_malloc(10 * 6 * sizeof(float));
    uint32_t cmd_size = 0, data_size = 0;

    float shortest_side = _UI_MIN(r->width, r->height);
    float final_radius = _UI_MIN(r->rx, shortest_side / 2);
    float diagonal = sqrtf(r->width * r->width + r->height * r->height);
    float grad_angle = r->fill.color_data.linear_gradient.angle;
    if (r->rx > 0)
    {
        /* Get the control point offset for rounded cases */
        int32_t cpoff = (int32_t)((float)final_radius * BEZIEL_CIRCLE_FIT_FACTOR);

        /* Rounded rectangle case */
        /* Starting point */
        rect_cmd[cmd_size++] = VLC_OP_MOVE;
        rect_data[data_size++] = 0 + final_radius;
        rect_data[data_size++] = 0;

        /* Top side */
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = r->width - final_radius;
        rect_data[data_size++] = 0;

        /* Top-right corner */
        rect_cmd[cmd_size++] = VLC_OP_CUBIC_REL;
        rect_data[data_size++] = cpoff;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = final_radius;
        rect_data[data_size++] = final_radius - cpoff;
        rect_data[data_size++] = final_radius;
        rect_data[data_size++] = final_radius;

        /* Right side */
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = r->width;
        rect_data[data_size++] = r->height - final_radius;

        /* Bottom-right corner*/
        rect_cmd[cmd_size++] = VLC_OP_CUBIC_REL;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = cpoff;
        rect_data[data_size++] = cpoff - final_radius;
        rect_data[data_size++] = final_radius;
        rect_data[data_size++] = 0 - final_radius;
        rect_data[data_size++] = final_radius;

        /* Bottom side */
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = 0 + final_radius;
        rect_data[data_size++] = r->height;

        /* Bottom-left corner */
        rect_cmd[cmd_size++] = VLC_OP_CUBIC_REL;
        rect_data[data_size++] = 0 - cpoff;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = 0 - final_radius;
        rect_data[data_size++] = cpoff - final_radius;
        rect_data[data_size++] = 0 - final_radius;
        rect_data[data_size++] = 0 - final_radius;

        /* Left side*/
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = 0 + final_radius;

        /* Top-left corner */
        rect_cmd[cmd_size++] = VLC_OP_CUBIC_REL;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = 0 - cpoff;
        rect_data[data_size++] = final_radius - cpoff;
        rect_data[data_size++] = 0 - final_radius;
        rect_data[data_size++] = final_radius;
        rect_data[data_size++] = 0 - final_radius;

        /* Ending point */
        rect_cmd[cmd_size++] = VLC_OP_END;
    }
    else
    {
        /* Non-rounded rectangle case */
        /* Starting point */
        rect_cmd[cmd_size++] = VLC_OP_MOVE;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = 0;

        /* Top side */
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = r->width;
        rect_data[data_size++] = 0;

        /* Right side */
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = r->width;
        rect_data[data_size++] = r->height;

        /* Bottom side */
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = r->height;

        /* Left side*/
        rect_cmd[cmd_size++] = VLC_OP_LINE;
        rect_data[data_size++] = 0;
        rect_data[data_size++] = 0;

        /* Ending point */
        rect_cmd[cmd_size++] = VLC_OP_END;
    }

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(r->x, r->y, &matrix);
    vg_lite_rotate(r->rotate, &matrix);

    ARGB_struct in_color = {.d32 = r->fill.color_data.rgba};
    BGRA_struct fill_color = {.d32 = 0};
    fill_color.channel.A = in_color.channel.A;
    fill_color.channel.R = in_color.channel.R * (1.0f * in_color.channel.A / 255);
    fill_color.channel.G = in_color.channel.G * (1.0f * in_color.channel.A / 255);
    fill_color.channel.B = in_color.channel.B * (1.0f * in_color.channel.A / 255);

    in_color.d32 = r->stroke.fill.color_data.rgba;
    BGRA_struct stroke_color = {.d32 = 0};
    stroke_color.channel.A = in_color.channel.A;
    stroke_color.channel.R = in_color.channel.R * (1.0f * in_color.channel.A / 255);
    stroke_color.channel.G = in_color.channel.G * (1.0f * in_color.channel.A / 255);
    stroke_color.channel.B = in_color.channel.B * (1.0f * in_color.channel.A / 255);

    vg_lite_path_t rect_path;
    vg_lite_linear_gradient_t rect_grad;
    uint32_t data_len = vg_lite_path_calc_length(rect_cmd, cmd_size, VG_LITE_FP32);
    vg_lite_init_path(&rect_path, VG_LITE_FP32, VG_LITE_HIGH, data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *path_data = gui_malloc(data_len);
    rect_path.path = path_data;
    vg_lite_path_append(&rect_path, rect_cmd, rect_data, cmd_size);
    float dash[] = {0, 0};
    if (r->fill.color_data.linear_gradient.stops_number > 0)
    {
        has_grad = true;
        memset(&rect_grad, 0, sizeof(vg_lite_linear_gradient_t));
        vg_lite_init_grad(&rect_grad);
        vg_lite_set_grad(&rect_grad, r->fill.color_data.linear_gradient.stops_number,
                         r->fill.color_data.linear_gradient.colors, r->fill.color_data.linear_gradient.stops);
        vg_lite_update_grad(&rect_grad);
        vg_lite_matrix_t *grad_matrix = vg_lite_get_grad_matrix(&rect_grad);
        vg_lite_identity(grad_matrix);
        float static_angle = atanf(r->height / r->width) / PI * 180.0f;
        if ((grad_angle >= 0) && (grad_angle <= 90))
        {
            vg_lite_translate(r->x, r->y, grad_matrix);
            vg_lite_rotate(grad_angle + r->rotate, grad_matrix);
            vg_lite_scale(diagonal * cosf((grad_angle - static_angle) / 180 * PI) / 256, 1, grad_matrix);
        }
        else if ((grad_angle > 90) && (grad_angle <= 180))
        {
            vg_lite_translate(r->x + r->width * cosf(r->rotate * PI / 180.0f),
                              r->y + r->width * sinf(r->rotate * PI / 180.0f), grad_matrix);
            vg_lite_rotate(grad_angle + r->rotate, grad_matrix);
            vg_lite_scale(diagonal * cosf((grad_angle + static_angle - 180) / 180 * PI) / 256, 1, grad_matrix);
        }
        else if ((grad_angle > 180) && (grad_angle <= 270))
        {
            vg_lite_translate(r->x + diagonal * cosf((r->rotate + static_angle) * PI / 180.0f),
                              r->y + diagonal * sinf((r->rotate + static_angle) * PI / 180.0f), grad_matrix);
            vg_lite_rotate(grad_angle + r->rotate, grad_matrix);
            vg_lite_scale(diagonal * cosf((grad_angle - static_angle - 180) / 180 * PI) / 256, 1, grad_matrix);
        }
        else if ((grad_angle > 270) && (grad_angle < 360))
        {
            vg_lite_translate(r->x + r->height * cosf((r->rotate + 90) * PI / 180.0f),
                              r->y + r->height * sinf((r->rotate + 90) * PI / 180.0f), grad_matrix);
            vg_lite_rotate(grad_angle + r->rotate, grad_matrix);
            vg_lite_scale(diagonal * cosf((grad_angle + static_angle) / 180 * PI) / 256, 1, grad_matrix);
        }
        vg_lite_draw_gradient(&target, &rect_path, VG_LITE_FILL_NON_ZERO, &matrix, &rect_grad,
                              VG_LITE_BLEND_SRC_OVER);
    }
    else if (r->fill.color_data.rgba != 0)
    {
        vg_lite_draw(&target, &rect_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER,
                     fill_color.d32);
    }

    if ((r->stroke.stroke_width != 0) && (r->stroke.fill.color_data.rgba != 0))
    {
        vg_lite_set_stroke(&rect_path, (vg_lite_cap_style_t)r->stroke.stroke_linecap,
                           (vg_lite_join_style_t)r->stroke.stroke_linejoin, stroke_width,
                           r->stroke.miter_limit, r->stroke.dash, r->stroke.dash_count, r->stroke.dash_phase,
                           stroke_color.d32);
        vg_lite_update_stroke(&rect_path);
        vg_lite_set_draw_path_type(&rect_path, VG_LITE_DRAW_STROKE_PATH);
        vg_lite_draw(&target, &rect_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER,
                     stroke_color.d32);
    }

    vg_lite_finish();
    vg_lite_clear_path(&rect_path);
    if (has_grad)
    {
        vg_lite_clear_grad(&rect_grad);
    }
    gui_free(path_data);
    gui_free(rect_cmd);
    gui_free(rect_data);
}

void hw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, struct rtgui_rect *rect)
{
    vg_lite_matrix_t matrix;
    memcpy(&matrix, image->matrix, sizeof(vg_lite_matrix_t));

    vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));

    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;


    vg_lite_buffer_t source;
    memset(&source, 0x00, sizeof(vg_lite_buffer_t));

    struct gui_rgb_data_head *head = image->data;
    uint8_t source_bytes_per_pixel = 0;
    switch (head->type)
    {
    case RGB565:
        source.format = VG_LITE_BGR565;
        source_bytes_per_pixel = 2;
        break;
    case RGB888:
        source.format = VG_LITE_BGR888;
        source_bytes_per_pixel = 3;
        break;
    case RGBA8888:
        source.format = VG_LITE_BGRA8888;
        source_bytes_per_pixel = 4;
        break;
    default:
        break;
    }
    uint32_t gpu_width = ((image->img_w + 15) >> 4) << 4;
    uint32_t gpu_height = image->img_h;
    source.stride = gpu_width * source_bytes_per_pixel;
    uint8_t *tmp = gui_malloc(gpu_width * gpu_height * source_bytes_per_pixel + 63);
    uint8_t *source_buffer = tmp + 64 - (int)tmp % 64;
    GUI_ASSERT(source_buffer != NULL);
    uint32_t image_off = sizeof(struct gui_rgb_data_head) + (uint32_t)(image->data);
    for (uint32_t i = 0; i < gpu_height; i++)
    {
        memcpy(source_buffer + i * gpu_width * source_bytes_per_pixel,
               (void *)(image_off + i * image->img_w * source_bytes_per_pixel),
               image->img_w * source_bytes_per_pixel);
    }

    source.width = gpu_width;
    source.height = image->img_h;
    source.memory = (void *)source_buffer;
    source.address = (uint32_t)source_buffer;
    source.tiled = VG_LITE_LINEAR;

    source.tiled = VG_LITE_LINEAR;
    switch (image->blend_mode)
    {
    case IMG_BYPASS_MODE:
        break;
    case IMG_FILTER_BLACK:
        {
            vg_lite_color_key4_t color_key;
            color_key[0].alpha = 0x00;
            color_key[0].enable = 1;
            color_key[0].hign_r = 0x00;
            color_key[0].low_r = 0x00;
            color_key[0].hign_b = 0x00;
            color_key[0].low_b = 0x00;
            color_key[0].hign_g = 0x00;
            color_key[0].low_g = 0x00;
            vg_lite_set_color_key(color_key);
            source.transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
        }
        break;
    case IMG_TRANSPARENT_MODE:
        blend_mode = VG_LITE_BLEND_SRC_OVER;
    default:
        break;
    }

    vg_lite_rectangle_t imgae_rect = {.x = 0, .y = 0, .width = image->img_w, .height = image->img_h};
    vg_lite_error_t error_code = vg_lite_blit_rect(&target, &source, &imgae_rect, &matrix,
                                                   VG_LITE_BLEND_NONE, 0, VG_LITE_FILTER_POINT);

    GUI_ASSERT(error_code == VG_LITE_SUCCESS);
    vg_lite_finish();
    gui_free(tmp);
}

#include "nanosvg.h"
extern void list_memheap(void);
typedef struct vg_lite_grad_ptr
{
    vg_lite_linear_gradient_t grad;
    uint32_t *colors;
    uint32_t *stops;
} vg_lite_grad_ptr;

typedef struct vg_lite_path_ptr
{
    vg_lite_path_t path;
    void *path_data;
    vg_lite_grad_ptr *p_grad;
    struct vg_lite_path_ptr *next;      // Pointer to next path, or NULL if last element.
} vg_lite_path_ptr;


void hw_acc_draw_svg(void *svg, uint32_t data_length, struct gui_dispdev *dc, int x, int y,
                     float scale, float rotate_degree, float rotate_center_x, float rotate_center_y)
{
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    if (rotate_degree != 0)
    {
        vg_lite_translate(rotate_center_x, rotate_center_y, &matrix);
        vg_lite_rotate(rotate_degree, &matrix);
        vg_lite_translate(-rotate_center_x, -rotate_center_y, &matrix);
    }
    vg_lite_translate(x, y, &matrix);
    vg_lite_scale(scale, scale, &matrix);

    vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));

    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;


    vg_lite_path_ptr *vg_lite_path_list = gui_malloc(sizeof(vg_lite_path_ptr));
    memset(vg_lite_path_list, 0, sizeof(vg_lite_path_ptr));
    vg_lite_path_ptr *temp_path_record = vg_lite_path_list;
    NSVGimage *image = svg;
    NSVGshape *shape_list = image->shapes;

    while (shape_list)
    {
        NSVGpath *path_list = shape_list->paths;
        while (path_list)
        {
            uint32_t cmd_size = 2, cmd_offset = 1;
            cmd_size += (path_list->npts - 1) / 3;
            uint8_t *cmd_list = gui_malloc(cmd_size);
            float *data_list = gui_malloc(sizeof(float) * (2 + 6 * ((path_list->npts - 1) / 3)));
            cmd_list[0] = VLC_OP_MOVE;
            memcpy(data_list, path_list->pts, 2 * sizeof(float));
            for (int i = 0; i < path_list->npts - 1; i += 3)
            {
                cmd_list[cmd_offset++] = VLC_OP_CUBIC;
                float *p = &path_list->pts[i * 2];
                memcpy(data_list + i * 2 + 2, p + 2, 6 * sizeof(float));
            }
            if ((shape_list->stroke.type == 1) && (path_list->closed == 0))
            {
                cmd_size -= 1;
            }
            cmd_list[cmd_offset++] = VLC_OP_END;
            uint32_t data_len = vg_lite_path_calc_length(cmd_list, cmd_size, VG_LITE_FP32);
            vg_lite_init_path(&temp_path_record->path, VG_LITE_FP32, VG_LITE_HIGH, data_len, NULL, dc->fb_width,
                              dc->fb_height, -dc->fb_width, -dc->fb_height);
            void *path_data = gui_malloc(data_len);
            temp_path_record->path.path = path_data;
            temp_path_record->path_data = path_data;
            vg_lite_path_append(&temp_path_record->path, cmd_list, data_list, cmd_size);

            ARGB_struct in_color = {.d32 = shape_list->fill.d.color};
            BGRA_struct fill_color = {.d32 = 0};
            fill_color.channel.A = 0xFF * shape_list->opacity;
            fill_color.channel.R = in_color.channel.R * shape_list->opacity;
            fill_color.channel.G = in_color.channel.G * shape_list->opacity;
            fill_color.channel.B = in_color.channel.B * shape_list->opacity;

            in_color.d32 = shape_list->stroke.d.color;
            BGRA_struct stroke_color = {.d32 = 0};
            stroke_color.channel.A = 0xFF * shape_list->opacity;
            stroke_color.channel.R = in_color.channel.R * shape_list->opacity;
            stroke_color.channel.G = in_color.channel.G * shape_list->opacity;
            stroke_color.channel.B = in_color.channel.B * shape_list->opacity;

            if (shape_list->fill.type == 1)
            {
                if (shape_list->fill.d.color != 0)
                {
                    vg_lite_set_draw_path_type(&temp_path_record->path, VG_LITE_DRAW_FILL_PATH);
                    vg_lite_draw(&target, &temp_path_record->path, (vg_lite_fill_t)shape_list->fillRule, &matrix,
                                 VG_LITE_BLEND_SRC_OVER,
                                 shape_list->fill.d.color);
                }
            }
            else if (shape_list->fill.type == 2)
            {
                temp_path_record->p_grad = gui_malloc(sizeof(vg_lite_grad_ptr));
                memset(temp_path_record->p_grad, 0, sizeof(vg_lite_grad_ptr));
                vg_lite_init_grad(&temp_path_record->p_grad->grad);
                temp_path_record->p_grad->colors = gui_malloc(sizeof(uint32_t) *
                                                              shape_list->fill.d.gradient->nstops);
                temp_path_record->p_grad->stops = gui_malloc(sizeof(uint32_t) *
                                                             shape_list->fill.d.gradient->nstops);
                NSVGgradientStop *grad_list = shape_list->fill.d.gradient->stops;
                for (int i = 0; i < shape_list->fill.d.gradient->nstops; i++)
                {
                    BGRA_struct before_transfer = {.d32 = grad_list[i].color};
                    RGBA_struct actual_color = {.d32 = 0};
                    actual_color.channel.A = before_transfer.channel.A;
                    actual_color.channel.R = before_transfer.channel.R * (1.0f * before_transfer.channel.A / 0xFF);
                    actual_color.channel.G = before_transfer.channel.G * (1.0f * before_transfer.channel.A / 0xFF);
                    actual_color.channel.B = before_transfer.channel.B * (1.0f * before_transfer.channel.A / 0xFF);
                    temp_path_record->p_grad->colors[i] = actual_color.d32;
                    temp_path_record->p_grad->stops[i] = grad_list[i].offset * 0xFF;
                }
                vg_lite_set_grad(&temp_path_record->p_grad->grad, shape_list->fill.d.gradient->nstops,
                                 temp_path_record->p_grad->colors, temp_path_record->p_grad->stops);
                vg_lite_update_grad(&temp_path_record->p_grad->grad);
                vg_lite_matrix_t *grad_matrix = vg_lite_get_grad_matrix(&temp_path_record->p_grad->grad);
                vg_lite_identity(grad_matrix);
                vg_lite_translate(x, y, grad_matrix);
                vg_lite_scale(VLC_GRADIENT_BUFFER_WIDTH / (temp_path_record->path.bounding_box[2] -
                                                           temp_path_record->path.bounding_box[0])
                              * scale, 1, grad_matrix);
                vg_lite_translate(temp_path_record->path.bounding_box[0], temp_path_record->path.bounding_box[1],
                                  grad_matrix);
                vg_lite_draw_gradient(&target, &temp_path_record->path, (vg_lite_fill_t)shape_list->fillRule,
                                      &matrix, &temp_path_record->p_grad->grad, VG_LITE_BLEND_SRC_OVER);
            }

            if ((shape_list->strokeWidth > 0) && (shape_list->stroke.type == 1) &&
                (shape_list->stroke.d.color != 0))
            {
                vg_lite_set_stroke(&temp_path_record->path, (vg_lite_cap_style_t)shape_list->strokeLineCap,
                                   (vg_lite_join_style_t)shape_list->strokeLineJoin, shape_list->strokeWidth, shape_list->miterLimit,
                                   shape_list->strokeDashArray, shape_list->strokeDashCount, shape_list->strokeDashOffset,
                                   stroke_color.d32);
                vg_lite_update_stroke(&temp_path_record->path);
                vg_lite_set_draw_path_type(&temp_path_record->path, VG_LITE_DRAW_STROKE_PATH);
                vg_lite_draw(&target, &temp_path_record->path, (vg_lite_fill_t)shape_list->fillRule, &matrix,
                             VG_LITE_BLEND_SRC_OVER,
                             stroke_color.d32);
            }

            temp_path_record->next = gui_malloc(sizeof(vg_lite_path_ptr));
            memset(temp_path_record->next, 0, sizeof(vg_lite_path_ptr));
            temp_path_record = temp_path_record->next;
            path_list = path_list->next;
            free(cmd_list);
            cmd_list = NULL;
            free(data_list);
            data_list = NULL;
        }
        shape_list = shape_list->next;
    }
    vg_lite_finish();
    vg_lite_path_ptr *current_ptr;
    while (vg_lite_path_list)
    {
        current_ptr = vg_lite_path_list;
        vg_lite_clear_path(&vg_lite_path_list->path);
        if (vg_lite_path_list->p_grad)
        {
            free(vg_lite_path_list->p_grad->colors);
            free(vg_lite_path_list->p_grad->stops);
            vg_lite_path_list->p_grad->colors = NULL;
            vg_lite_path_list->p_grad->stops = NULL;
            vg_lite_clear_grad(&vg_lite_path_list->p_grad->grad);
            free(vg_lite_path_list->p_grad);
            vg_lite_path_list->p_grad = NULL;
        }
        if (vg_lite_path_list->path_data)
        {
            free(vg_lite_path_list->path_data);
            vg_lite_path_list->path_data = NULL;
        }
        vg_lite_path_list = vg_lite_path_list->next;
        free(current_ptr);
        current_ptr = NULL;
    }
    //nsvgDelete(image);
error:
    return;
}

void (hw_acc_draw_wave)(canvas_wave_t *wave, struct gui_dispdev *dc)
{
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(wave->x, wave->y, &matrix);
    vg_lite_buffer_t target;

    memset(&target, 0x00, sizeof(vg_lite_buffer_t));

    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;

    float w = wave->w;
    float h = wave->h;
    float x = wave->x;
    float y = wave->y;
    float dx = w / 5.0f;
    float *sx = wave->point_x;
    float *sy = wave->point_y;
    // Graph background
    uint32_t cmd_len = 1;
    uint32_t data_len = 2;
    uint8_t *cmd;
    float *data;
    cmd = gui_malloc(1);
    data = gui_malloc(2 * sizeof(float));
    cmd[0] = VLC_OP_MOVE;
    data[0] = sx[0];
    data[1] = sy[0];
    int i;
    for (i = 1; i < wave->point_count; i++)
    {
        float vals[] = {sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]};
        cmd = gui_realloc(cmd, ++cmd_len);
        cmd[i] = VLC_OP_CUBIC;
        data = gui_realloc(data, sizeof(vals) + data_len * sizeof(float));
        memcpy(data + data_len, vals, sizeof(vals));
        data_len += sizeof(vals) / sizeof(float);
    }
    cmd = gui_realloc(cmd, ++cmd_len);
    cmd[cmd_len - 1] = VLC_OP_LINE;
    data = gui_realloc(data, (data_len + 2) * sizeof(float));
    data[data_len++] = x + w;
    data[data_len++] = y + h;
    cmd = gui_realloc(cmd, ++cmd_len);
    cmd[cmd_len - 1] = VLC_OP_LINE;
    data = gui_realloc(data, (data_len + 2) * sizeof(float));
    data[data_len++] = x;
    data[data_len++] = y + h;
    cmd = gui_realloc(cmd, ++cmd_len);
    cmd[cmd_len - 1] = VLC_OP_LINE;
    data = gui_realloc(data, (data_len + 2) * sizeof(float));
    data[data_len++] = sx[0];
    data[data_len++] = sy[0];
    cmd = gui_realloc(cmd, ++cmd_len);
    cmd[cmd_len - 1] = VLC_OP_END;

    ABGR_struct in_color = {.d32 = wave->fill.color_data.rgba};
    BGRA_struct fill_color = {.d32 = 0};
    fill_color.channel.A = in_color.channel.A;
    fill_color.channel.R = in_color.channel.R * (in_color.channel.A * 1.0f / 255.0f);
    fill_color.channel.G = in_color.channel.G * (in_color.channel.A * 1.0f / 255.0f);
    fill_color.channel.B = in_color.channel.B * (in_color.channel.A * 1.0f / 255.0f);
    uint32_t colors[] = {fill_color.d32, 0};
    uint32_t stops[] = {0, 255};

    vg_lite_path_t wave_path, stroke_path;
    vg_lite_linear_gradient_t wave_grad;
    memset(&wave_grad, 0, sizeof(vg_lite_linear_gradient_t));
    vg_lite_init_grad(&wave_grad);
    vg_lite_set_grad(&wave_grad, sizeof(stops) / sizeof(uint32_t), colors, stops);
    vg_lite_update_grad(&wave_grad);
    vg_lite_matrix_t *grad_matrix = vg_lite_get_grad_matrix(&wave_grad);
    vg_lite_identity(grad_matrix);
    vg_lite_translate(454 / 2.0f, 454 / 2.0f, grad_matrix);
    vg_lite_rotate(-90, grad_matrix);
    vg_lite_translate(-454 / 2.0f, -454 / 2.0f, grad_matrix);
    vg_lite_scale(dc->fb_width / 255.0f, dc->fb_height / 255.0f, grad_matrix);

    memset(&wave_path, 0, sizeof(vg_lite_path_t));
    memset(&stroke_path, 0, sizeof(vg_lite_path_t));

    uint32_t path_data_len = vg_lite_path_calc_length(cmd, cmd_len, VG_LITE_FP32);
    vg_lite_init_path(&wave_path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *path_data = gui_malloc(path_data_len);
    wave_path.path = path_data;
    vg_lite_path_append(&wave_path, cmd, data, cmd_len);
    vg_lite_draw_gradient(&target, &wave_path, VG_LITE_FILL_NON_ZERO,
                          &matrix, &wave_grad, VG_LITE_BLEND_SRC_OVER);

    path_data_len = vg_lite_path_calc_length(cmd, cmd_len - 4, VG_LITE_FP32);
    vg_lite_init_path(&stroke_path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *stroke_data = gui_malloc(path_data_len);
    stroke_path.path = stroke_data;
    vg_lite_path_append(&stroke_path, cmd, data, cmd_len - 4);
    vg_lite_set_stroke(&stroke_path, VG_LITE_CAP_ROUND,
                       VG_LITE_JOIN_ROUND, 2.8, 6,
                       NULL, 0, 0, fill_color.d32);
    vg_lite_update_stroke(&stroke_path);
    vg_lite_set_draw_path_type(&stroke_path, VG_LITE_DRAW_STROKE_PATH);
    vg_lite_draw(&target, &stroke_path, VG_LITE_FILL_NON_ZERO, &matrix,
                 VG_LITE_BLEND_NONE,
                 fill_color.d32);

    uint8_t circle_cmd[] = {    VLC_OP_MOVE,
                                VLC_OP_LCWARC,
                                VLC_OP_LCWARC,
                                VLC_OP_END,
                           };

    vg_lite_path_ptr *root_ptr = gui_malloc(sizeof(vg_lite_path_ptr));
    memset(root_ptr, 0, sizeof(vg_lite_path_ptr));
    vg_lite_path_ptr *probe = root_ptr;
    for (i = 0; i < wave->point_count; i++)
    {
        float radius = 4.0f;
        float circle_data[] =
        {
            sx[i], sy[i] - radius,
            radius, radius, 0, 0 + sx[i], radius + sy[i],
            radius, radius, 0, 0 + sx[i], -radius + sy[i],
        };
        uint32_t data_len = vg_lite_path_calc_length(circle_cmd, sizeof(circle_cmd), VG_LITE_FP32);
        vg_lite_init_path(&probe->path, VG_LITE_FP32, VG_LITE_HIGH, data_len, NULL, dc->fb_width,
                          dc->fb_height, -dc->fb_width, -dc->fb_height);
        void *path_data = gui_malloc(data_len);
        probe->path.path = path_data;
        probe->path_data = path_data;
        vg_lite_path_append(&probe->path, circle_cmd, circle_data, sizeof(circle_cmd));
        vg_lite_draw(&target, &probe->path, VG_LITE_FILL_NON_ZERO, &matrix,
                     VG_LITE_BLEND_SRC_OVER, 0xFFc0a000);
        probe->next = gui_malloc(sizeof(vg_lite_path_ptr));
        probe = probe->next;
        memset(probe, 0, sizeof(vg_lite_path_ptr));

        radius = 2.0f;
        float top_circle_data[] =
        {
            sx[i], sy[i] - radius,
            radius, radius, 0, 0 + sx[i], radius + sy[i],
            radius, radius, 0, 0 + sx[i], -radius + sy[i],
        };
        data_len = vg_lite_path_calc_length(circle_cmd, sizeof(circle_cmd), VG_LITE_FP32);
        vg_lite_init_path(&probe->path, VG_LITE_FP32, VG_LITE_HIGH, data_len, NULL, dc->fb_width,
                          dc->fb_height, -dc->fb_width, -dc->fb_height);
        void *top_path_data = gui_malloc(data_len);
        probe->path.path = top_path_data;
        probe->path_data = top_path_data;
        vg_lite_path_append(&probe->path, circle_cmd, top_circle_data, sizeof(circle_cmd));
        vg_lite_draw(&target, &probe->path, VG_LITE_FILL_NON_ZERO, &matrix,
                     VG_LITE_BLEND_SRC_OVER, 0xFFDCDCDC);
        probe->next = gui_malloc(sizeof(vg_lite_path_ptr));
        probe = probe->next;
        memset(probe, 0, sizeof(vg_lite_path_ptr));
    }

    vg_lite_finish();
    gui_free(path_data);
    gui_free(data);
    gui_free(cmd);
    gui_free(stroke_data);
    vg_lite_clear_path(&stroke_path);
    vg_lite_clear_path(&wave_path);
    vg_lite_clear_grad(&wave_grad);
    vg_lite_path_ptr *current_ptr;
    while (root_ptr)
    {
        current_ptr = root_ptr;
        vg_lite_clear_path(&root_ptr->path);
        if (root_ptr->p_grad)
        {
            free(root_ptr->p_grad->colors);
            free(root_ptr->p_grad->stops);
            root_ptr->p_grad->colors = NULL;
            root_ptr->p_grad->stops = NULL;
            vg_lite_clear_grad(&root_ptr->p_grad->grad);
            free(root_ptr->p_grad);
            root_ptr->p_grad = NULL;
        }
        if (root_ptr->path_data)
        {
            free(root_ptr->path_data);
            root_ptr->path_data = NULL;
        }
        root_ptr = root_ptr->next;
        free(current_ptr);
        current_ptr = NULL;
    }
}

void hw_acc_draw_palette_wheel(canvas_palette_wheel_t *pw, struct gui_dispdev *dc)
{
    vg_lite_matrix_t matrix;
    vg_lite_buffer_t target;

    memset(&target, 0x00, sizeof(vg_lite_buffer_t));

    vg_lite_blend_t blend_mode = VG_LITE_BLEND_NONE;
    target.width  = dc->fb_width;
    target.height = dc->fb_height;
    if (dc->bit_depth == 16)
    {
        target.format = VG_LITE_BGR565;
        target.stride = target.width * 2;
    }
    else
    {
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
    }
    target.memory = (void *)dc->frame_buf;
    target.address = (uint32_t)dc->frame_buf;
    target.tiled = VG_LITE_LINEAR;

    int i;
    float r0, r1, r2, ax, ay, bx, by, cx, cy, aeps, r;
    float hue = sinf(pw->selector_radian * 0.12f);
    float rotate_angle = hue * 360.0f;

    cx = pw->x + pw->w * 0.5f;
    cy = pw->y + pw->h * 0.5f;
    r1 = (pw->w < pw->h ? pw->w : pw->h) * 0.5f - 15.0f;
    r0 = r1 - 20.0f;
    r2 = r1 + 9.0f;
    aeps = 0.3f / r1;   // half a pixel arc length in radians (2pi cancels out).

    float a0 = aeps;
    float a1 = 1.0f * PI / 3.0f - aeps;


    vg_lite_path_ptr arc_path[6];
    memset(&arc_path, 0, sizeof(arc_path));
    for (i = 0; i < 6; i++)
    {
        float a0 = (float)i / 6.0f * PI * 2.0f - aeps;
        float a1 = (float)(i + 1.0f) / 6.0f * PI * 2.0f + aeps;

        ax = cx + cosf(a0) * (r0 + r1) * 0.5f;
        ay = cy + sinf(a0) * (r0 + r1) * 0.5f;
        bx = cx + cosf(a1) * (r0 + r1) * 0.5f;
        by = cy + sinf(a1) * (r0 + r1) * 0.5f;

        uint8_t arc_cmd[] =
        {
            VLC_OP_MOVE,
            VLC_OP_SCWARC,
            VLC_OP_LINE,
            VLC_OP_SCCWARC,
            VLC_OP_LINE,
            VLC_OP_END
        };

        float arc_data[] =
        {
            cx + cosf(a0) *r1, cy + sinf(a0) *r1,
            r1, r1, 0, cx + cosf(a1) *r1, cy + sinf(a1) *r1,
            cx + cosf(a1) *r0, cy + sinf(a1) *r0,
            r0, r0, 0, cx + cosf(a0) *r0, cy + sinf(a0) *r0,
            cx + cosf(a0) *r1, cy + sinf(a0) *r1,
        };

        memset(&arc_path[i].path, 0, sizeof(vg_lite_path_t));

        uint32_t path_data_len = vg_lite_path_calc_length(arc_cmd, sizeof(arc_cmd), VG_LITE_FP32);
        vg_lite_init_path(&arc_path[i].path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, -dc->fb_width,
                          -dc->fb_height, dc->fb_width, dc->fb_height);
        void *path_data = gui_malloc(path_data_len);
        arc_path[i].path.path = path_data;
        arc_path[i].path_data = path_data;
        vg_lite_path_append(&arc_path[i].path, arc_cmd, arc_data, sizeof(arc_cmd));

        BGRA_struct from_color = HSLA(a0 / (PI * 2.0f), 1.0f, 0.55f, 255);
        BGRA_struct to_color = HSLA(a1 / (PI * 2.0f), 1.0f, 0.55f, 255);
        uint32_t colors[] = {from_color.d32, to_color.d32};
        uint32_t stops[] = {0, 255};
        arc_path[i].p_grad = gui_malloc(sizeof(vg_lite_grad_ptr));
        memset(arc_path[i].p_grad, 0, sizeof(vg_lite_grad_ptr));
        vg_lite_init_grad(&arc_path[i].p_grad->grad);
        vg_lite_set_grad(&arc_path[i].p_grad->grad, sizeof(stops) / sizeof(uint32_t), colors, stops);
        vg_lite_update_grad(&arc_path[i].p_grad->grad);
        vg_lite_matrix_t *grad_matrix = vg_lite_get_grad_matrix(&arc_path[i].p_grad->grad);
        vg_lite_identity(grad_matrix);
        vg_lite_translate(cx + cosf(a0) * r0, cy + sinf(a0) * r0, grad_matrix);
        vg_lite_rotate(120 + a0 * 180 / PI, grad_matrix);
        vg_lite_scale(r0 / 256.0f, 1, grad_matrix);
        vg_lite_identity(&matrix);
        //vg_lite_translate(pw->x, pw->y, &matrix);
        vg_lite_draw_gradient(&target, &arc_path[i].path, VG_LITE_FILL_EVEN_ODD,
                              &matrix, &arc_path[i].p_grad->grad, VG_LITE_BLEND_SRC_OVER);
    }

    r = r0 - 6;
    ax = cosf(120.0f / 180.0f * PI) * r;
    ay = sinf(120.0f / 180.0f * PI) * r;
    bx = cosf(-120.0f / 180.0f * PI) * r;
    by = sinf(-120.0f / 180.0f * PI) * r;
    uint8_t tri_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_LINE,
        VLC_OP_LINE,
        VLC_OP_END,
    };

    float tri_data[] =
    {
        r, 0,
        ax, ay,
        bx, by,
    };
    BGRA_struct tri_base = HSLA(hue, 1.0f, 0.5f, 255);
    uint32_t tri_white_colors[] = {tri_base.d32, 0xFFFFFFFF};
    uint32_t tri_black_colors[] = {0x00000000, 0xFF000000};
    uint32_t tri_stops[] = {0, 255};

    vg_lite_linear_gradient_t tri_white_grad, tri_black_grad;
    memset(&tri_white_grad, 0, sizeof(vg_lite_linear_gradient_t));
    vg_lite_init_grad(&tri_white_grad);
    vg_lite_set_grad(&tri_white_grad, sizeof(tri_white_colors) / sizeof(uint32_t), tri_white_colors,
                     tri_stops);
    vg_lite_update_grad(&tri_white_grad);
    vg_lite_matrix_t *tri_white_mat = vg_lite_get_grad_matrix(&tri_white_grad);
    vg_lite_identity(tri_white_mat);
    vg_lite_translate(cx + r * cosf(rotate_angle / 180.f * PI), cy + sinf(rotate_angle / 180.f * PI),
                      tri_white_mat);
    vg_lite_rotate(150  + rotate_angle, tri_white_mat);
    vg_lite_scale(r * 1.732051 / 255.0f, 1, tri_white_mat);


    memset(&tri_black_grad, 0, sizeof(vg_lite_linear_gradient_t));
    vg_lite_init_grad(&tri_black_grad);
    vg_lite_set_grad(&tri_black_grad, sizeof(tri_white_colors) / sizeof(uint32_t), tri_black_colors,
                     tri_stops);
    vg_lite_update_grad(&tri_black_grad);
    vg_lite_matrix_t *tri_black_mat = vg_lite_get_grad_matrix(&tri_black_grad);
    vg_lite_identity(tri_black_mat);
    vg_lite_translate(cx + r / 2.0f * cosf((60 + rotate_angle) / 180 * PI),
                      r / 2.0f * sinf((60 + rotate_angle) / 180 * PI) + cy, tri_black_mat);
    vg_lite_scale(r * 1.5 / 255.0f, 1, tri_black_mat);
    vg_lite_rotate(-120 + rotate_angle, tri_black_mat);

    vg_lite_path_t tri_path;
    memset(&tri_path, 0, sizeof(tri_path));
    uint32_t path_data_len = vg_lite_path_calc_length(tri_cmd, sizeof(tri_cmd), VG_LITE_FP32);
    vg_lite_init_path(&tri_path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *tri_path_data = gui_malloc(path_data_len);
    tri_path.path = tri_path_data;
    vg_lite_path_append(&tri_path, tri_cmd, tri_data, sizeof(tri_cmd));
    vg_lite_identity(&matrix);
    vg_lite_translate(cx, cy, &matrix);
    vg_lite_rotate(rotate_angle, &matrix);
    vg_lite_draw_gradient(&target, &tri_path, VG_LITE_FILL_NON_ZERO,
                          &matrix, &tri_white_grad, VG_LITE_BLEND_SRC_OVER);
    vg_lite_draw_gradient(&target, &tri_path, VG_LITE_FILL_NON_ZERO,
                          &matrix, &tri_black_grad, VG_LITE_BLEND_SRC_OVER);

    uint8_t rect_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_LINE_REL,
        VLC_OP_LINE_REL,
        VLC_OP_LINE_REL,
        VLC_OP_END,
    };

    float rect_data[] =
    {
        r0 - 1, -3,
        (r1 - r0 + 2), 0,
        0, 6,
        -(r1 - r0 + 2), 0,
    };

    BGRA_struct rect_color = {.d32 = 0};
    rect_color.channel.A = 255;
    rect_color.channel.R = 255 * (rect_color.channel.A / 255.0f);
    rect_color.channel.G = 255 * (rect_color.channel.A / 255.0f);
    rect_color.channel.B = 255 * (rect_color.channel.A / 255.0f);
    vg_lite_path_t rect_path;
    memset(&rect_path, 0, sizeof(rect_path));
    path_data_len = vg_lite_path_calc_length(rect_cmd, sizeof(rect_cmd), VG_LITE_FP32);
    vg_lite_init_path(&rect_path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *rect_path_data = gui_malloc(path_data_len);
    rect_path.path = rect_path_data;
    vg_lite_path_append(&rect_path, rect_cmd, rect_data, sizeof(rect_cmd));
    vg_lite_set_stroke(&rect_path, VG_LITE_CAP_ROUND,
                       VG_LITE_JOIN_ROUND, 3.0f, 6,
                       NULL, 0, 0, rect_color.d32);
    vg_lite_update_stroke(&rect_path);
    vg_lite_set_draw_path_type(&rect_path, VG_LITE_DRAW_STROKE_PATH);
    vg_lite_draw(&target, &rect_path, VG_LITE_FILL_NON_ZERO, &matrix,
                 VG_LITE_BLEND_NONE,
                 rect_color.d32);


    uint8_t circle_cmd[] = {    VLC_OP_MOVE,
                                VLC_OP_LCWARC,
                                VLC_OP_LCWARC,
                                VLC_OP_END,
                           };

    float cpx = cx + (r0 - 6) / 4, cpy = cy + (r0 - 6) / 3;
    if (((cpx - cx) * (cpx - cx) + (cpy - cy) * (cpy - cy) - (r0 - 6) * (r0 - 6)) >= 0)
    {
        cpx = cx;
        cpy = cy;
    }
    float small_circle_data[] =
    {
        cpx, cpy - 5,
        5, 5, 0, 0 + cpx, 5 + cpy,
        5, 5, 0, 0 + cpx, -5 + cpy,
    };

    float big_circle_data[] =
    {
        cpx, cpy - 7,
        7, 7, 0, 0 + cpx, 7 + cpy,
        7, 7, 0, 0 + cpx, -7 + cpy,
    };

    vg_lite_path_t circle_small_path, circle_big_path, circle_display_path;
    memset(&circle_small_path, 0, sizeof(vg_lite_path_t));
    memset(&circle_big_path, 0, sizeof(vg_lite_path_t));
    vg_lite_identity(&matrix);

    path_data_len = vg_lite_path_calc_length(circle_cmd, sizeof(circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&circle_small_path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL,
                      -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *s_circle_path_data = gui_malloc(path_data_len);
    circle_small_path.path = s_circle_path_data;
    vg_lite_path_append(&circle_small_path, circle_cmd, small_circle_data, sizeof(circle_cmd));
    vg_lite_set_stroke(&circle_small_path, VG_LITE_CAP_ROUND,
                       VG_LITE_JOIN_ROUND, 2.0f, 6,
                       NULL, 0, 0, 0xC0C0C0C0);
    vg_lite_update_stroke(&circle_small_path);
    vg_lite_set_draw_path_type(&circle_small_path, VG_LITE_DRAW_STROKE_PATH);
    vg_lite_draw(&target, &circle_small_path, VG_LITE_FILL_NON_ZERO, &matrix,
                 VG_LITE_BLEND_SRC_OVER,
                 0xC0C0C0C0);

    vg_lite_init_path(&circle_big_path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *b_circle_path_data = gui_malloc(path_data_len);
    circle_big_path.path = b_circle_path_data;
    vg_lite_path_append(&circle_big_path, circle_cmd, big_circle_data, sizeof(circle_cmd));
    vg_lite_set_stroke(&circle_big_path, VG_LITE_CAP_ROUND,
                       VG_LITE_JOIN_ROUND, 2.0f, 6,
                       NULL, 0, 0, 0x40000000);
    vg_lite_update_stroke(&circle_big_path);
    vg_lite_set_draw_path_type(&circle_big_path, VG_LITE_DRAW_STROKE_PATH);
    vg_lite_draw(&target, &circle_big_path, VG_LITE_FILL_NON_ZERO, &matrix,
                 VG_LITE_BLEND_SRC_OVER,
                 0x40000000);

    vg_lite_finish();
    float display_circle_data[] =
    {
        cx, cy - r2 / 3,
        r2 / 3, r2 / 3, 0, 0 + cx, r2 / 3 + cy,
        r2 / 3, r2 / 3, 0, 0 + cx, -r2 / 3 + cy,
    };
    uint32_t gpu_width = ((dc->fb_width + 15) >> 4) << 4;
    uint32_t disp_color = 0;
    if (dc->bit_depth == 16)
    {
        uint16_t color = *(uint16_t *)((uint8_t *)dc->frame_buf + ((int)cpx + (int)cpy * gpu_width) * 2);
    }
    else
    {
        BGRA_struct color = {.d32 = *(uint32_t *)((uint8_t *)dc->frame_buf + ((int)cpx + (int)cpy * gpu_width) * 4)};
        uint8_t temp = color.channel.R;
        color.channel.R = color.channel.B;
        color.channel.B = temp;
        disp_color = color.d32;
    }
    vg_lite_init_path(&circle_display_path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL,
                      -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *disp_circle_path_data = gui_malloc(path_data_len);
    circle_display_path.path = disp_circle_path_data;
    vg_lite_path_append(&circle_display_path, circle_cmd, display_circle_data, sizeof(circle_cmd));
    vg_lite_set_stroke(&circle_display_path, VG_LITE_CAP_ROUND,
                       VG_LITE_JOIN_ROUND, 2.5f, 6,
                       NULL, 0, 0, disp_color);
    vg_lite_update_stroke(&circle_display_path);
    vg_lite_set_draw_path_type(&circle_display_path, VG_LITE_DRAW_STROKE_PATH);
    vg_lite_identity(&matrix);
    vg_lite_translate(cx, cy, &matrix);
    vg_lite_scale(3, 3, &matrix);
    vg_lite_translate(-cx, -cy, &matrix);
    vg_lite_draw(&target, &circle_display_path, VG_LITE_FILL_NON_ZERO, &matrix,
                 VG_LITE_BLEND_SRC_OVER,
                 disp_color);
    vg_lite_finish();

    for (i = 0; i < 6; i++)
    {
        vg_lite_clear_path(&arc_path[i].path);
        if (arc_path[i].p_grad)
        {
            vg_lite_clear_grad(&arc_path[i].p_grad->grad);
            free(arc_path[i].p_grad);
            arc_path[i].p_grad = NULL;
        }
        if (arc_path[i].path_data)
        {
            free(arc_path[i].path_data);
            arc_path[i].path_data = NULL;
        }
    }
    vg_lite_clear_path(&tri_path);
    gui_free(tri_path_data);
    vg_lite_clear_path(&rect_path);
    gui_free(rect_path_data);
    vg_lite_clear_grad(&tri_white_grad);
    vg_lite_clear_grad(&tri_black_grad);
    gui_free(s_circle_path_data);
    gui_free(b_circle_path_data);
    vg_lite_clear_path(&circle_big_path);
    vg_lite_clear_path(&circle_small_path);
    gui_free(disp_circle_path_data);
    vg_lite_clear_path(&circle_display_path);
}
