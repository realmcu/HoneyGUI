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

void hw_draw_arc(canvas_arc_t *a, struct gui_dispdev *dc)
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
                           stroke_width, 0, dash, 0, 0);
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
//    vg_lite_scale(circle->scale, circle->scale, &matrix);
    vg_lite_scale(scale_ratio, scale_ratio, &matrix);

    vg_lite_translate(227 + 17.197916 * circle->scale, 227 + 17.197916 * circle->scale, &matrix);
    vg_lite_rotate(90, &matrix);
    vg_lite_translate(-227 - 17.197916 * circle->scale, -227 - 17.197916 * circle->scale, &matrix);
    vg_lite_translate(227, 227, &matrix);
    vg_lite_scale(circle->scale, circle->scale, &matrix);

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
                           circle->stroke.dash_phase);
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
                           stroke_width, l->stroke.dash, l->stroke.dash_count, l->stroke.dash_phase);
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
    if (r->stroke.stroke_width > 2.5)
    {
        scale_ratio = r->stroke.stroke_width * 1.0f / 2.5;
        stroke_width = 2.5;
    }

    float x_rel = (r->width) / scale_ratio;
    float y_rel = (r->height) / scale_ratio;

    uint8_t rect_cmd[] = {    VLC_OP_MOVE,
                              VLC_OP_LINE_REL,
                              VLC_OP_LINE_REL,
                              VLC_OP_LINE_REL,
                              VLC_OP_LINE_REL,
                              VLC_OP_END,
                         };
    float rect_data[] =
    {
        0, 0,
        0, y_rel,
        x_rel, 0,
        0, -y_rel,
        -x_rel, 0,
    };
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_scale(scale_ratio, scale_ratio, &matrix);
    vg_lite_rotate(r->rotate, &matrix);
    vg_lite_translate(r->x, r->y, &matrix);

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
    uint32_t data_len = vg_lite_path_calc_length(rect_cmd, sizeof(rect_cmd), VG_LITE_FP32);
    vg_lite_init_path(&rect_path, VG_LITE_FP32, VG_LITE_HIGH, data_len, NULL, -dc->fb_width,
                      -dc->fb_height, dc->fb_width, dc->fb_height);
    void *path_data = gui_malloc(data_len);
    rect_path.path = path_data;
    vg_lite_path_append(&rect_path, rect_cmd, rect_data, sizeof(rect_cmd));
    float dash[] = {0, 0};
    if (((r->stroke.stroke_width == 0) || (r->stroke.fill.color_data.rgba == 0)) &&
        (r->fill.color_data.rgba != 0))
    {
        vg_lite_draw(&target, &rect_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER,
                     fill_color.d32);
    }
    else if ((r->stroke.stroke_width != 0) && (r->stroke.fill.color_data.rgba != 0))
    {
        vg_lite_set_stroke(&rect_path, (vg_lite_cap_style_t)r->stroke.stroke_linecap,
                           (vg_lite_join_style_t)r->stroke.stroke_linejoin, stroke_width,
                           r->stroke.miter_limit, r->stroke.dash, r->stroke.dash_count, r->stroke.dash_phase);
        vg_lite_update_stroke(&rect_path);
        if (r->fill.color_data.rgba != 0)
        {
            vg_lite_set_draw_path_type(&rect_path, VG_LITE_DRAW_FILL_PATH);
            vg_lite_draw(&target, &rect_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER,
                         fill_color.d32);
        }
        vg_lite_set_draw_path_type(&rect_path, VG_LITE_DRAW_STROKE_PATH);
        vg_lite_draw(&target, &rect_path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER,
                     stroke_color.d32);
    }
    vg_lite_finish();
    vg_lite_clear_path(&rect_path);
    gui_free(path_data);
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

    uint32_t imgae_rect[4] = {0, 0, image->img_w, image->img_h};
    vg_lite_error_t error_code = vg_lite_blit_rect(&target, &source, imgae_rect, &matrix,
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
                vg_lite_scale((temp_path_record->path.bounding_box[2] - temp_path_record->path.bounding_box[0]) /
                              VLC_GRADBUFFER_WIDTH * scale,
                              (temp_path_record->path.bounding_box[3] - temp_path_record->path.bounding_box[1]) /
                              VLC_GRADBUFFER_WIDTH * scale, grad_matrix);
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
                                   shape_list->strokeDashArray, shape_list->strokeDashCount, shape_list->strokeDashOffset);
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
    uint32_t i = 0;
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