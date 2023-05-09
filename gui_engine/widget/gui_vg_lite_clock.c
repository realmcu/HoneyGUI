/*
 * File      : gui_vg_lite_clock.c
 * This file is part of GUI Engine
 */
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <gui_kb.h>
#include "gui_vg_lite_clock.h"

static char *error_type[] =
{
    "VG_LITE_SUCCESS",
    "VG_LITE_INVALID_ARGUMENT",
    "VG_LITE_OUT_OF_MEMORY",
    "VG_LITE_NO_CONTEXT",
    "VG_LITE_TIMEOUT",
    "VG_LITE_OUT_OF_RESOURCES",
    "VG_LITE_GENERIC_IO",
    "VG_LITE_NOT_SUPPORT",
};

#define IS_ERROR(status)         (status > 0)
#define CHECK_ERROR(Function) \
    error = Function; \
    if (IS_ERROR(error)) \
    { \
        gui_log("[%s: %d] failed.error type is %s\n", __func__, __LINE__,error_type[error]);\
        GUI_ASSERT(NULL != NULL); \
    }

#include "string.h"
#include "math.h"
#define PI                           3.141592653589793238462643383279502f
#define SINF(x)                      ((vg_lite_float_t) sin(x))
#define COSF(x)                      ((vg_lite_float_t) cos(x))

static int32_t i = 0;
static int32_t j = 0;
static int32_t k = 0;
static int32_t m = -10;
static float degrees = 0;
static vg_lite_matrix_t matrix_sec, matrix_minute, matrix_hour, matrix_circle, matrix1, matrix2,
       matrix3, matrix_sun, matrix_sunshine1, matrix_temp1, matrix_temp2, matrix_c_letter, matrix_mark;
static vg_lite_linear_gradient_t grad1, grad2, grad3;
static vg_lite_path_t path_minute, path_sec, path_hour_1, path_hour_2, in_circle, out_circle, path1,
       path2, path3, path_sunshine, path_c_letter, path_mark;
static vg_lite_matrix_t *matGrad1, *matGrad2, *matGrad3;
static void *path_data1, *path_data2, *path_data3;

static float angle = 1;
float rate = 0.4;
float rotate = -10;
static float accelerate = 2;
static bool revert = false;

static void vg_lite_clock_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);


    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;

    vg_lite_identity(&matrix_sec);
    vg_lite_identity(&matrix_minute);
    vg_lite_identity(&matrix_hour);
    vg_lite_identity(&matrix_circle);
    vg_lite_identity(&matrix1);
    vg_lite_identity(&matrix2);
    vg_lite_identity(&matrix3);
    vg_lite_identity(&matrix_sun);
    vg_lite_identity(&matrix_sunshine1);
    vg_lite_identity(&matrix_temp1);
    vg_lite_identity(&matrix_temp2);
    vg_lite_identity(&matrix_c_letter);
    vg_lite_identity(&matrix_mark);

    vg_lite_translate(root->dx, root->dy, &matrix_sec);
    vg_lite_translate(root->dx, root->dy, &matrix_minute);
    vg_lite_translate(root->dx, root->dy, &matrix_hour);
    vg_lite_translate(root->dx, root->dy, &matrix_circle);
    vg_lite_translate(root->dx, root->dy, &matrix1);
    vg_lite_translate(root->dx, root->dy, &matrix2);
    vg_lite_translate(root->dx, root->dy, &matrix3);
    vg_lite_translate(root->dx, root->dy, &matrix_sun);
    vg_lite_translate(root->dx, root->dy, &matrix_sunshine1);
    vg_lite_translate(root->dx, root->dy, &matrix_temp1);
    vg_lite_translate(root->dx, root->dy, &matrix_temp2);
    vg_lite_translate(root->dx, root->dy, &matrix_c_letter);
    vg_lite_translate(root->dx, root->dy, &matrix_mark);

    vg_lite_translate(cx, cy, &matrix1);
    vg_lite_translate(cx, cy, &matrix2);
    vg_lite_translate(cx, cy, &matrix3);
    vg_lite_translate(cx, cy, &matrix_sec);
    vg_lite_translate(cx, cy, &matrix_minute);
    vg_lite_translate(cx, cy, &matrix_hour);
    vg_lite_translate(cx, cy, &matrix_circle);
}

static void vg_lite_clock_draw_cb(gui_obj_t *obj)
{

    gui_vg_lite_clock_t *this = (gui_vg_lite_clock_t *)obj;


    gui_dispdev_t *dc = gui_get_dc();
    vg_lite_error_t error = VG_LITE_SUCCESS;

    uint8_t arc_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_END,
    };

    uint8_t arc_cmd_bottom[] =
    {
        VLC_OP_MOVE,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_END,
    };

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;

    //dynamic change
    {
        vg_lite_buffer_t target;
        memset(&target, 0x00, sizeof(vg_lite_buffer_t));
        target.width  = dc->fb_width;
        target.height = dc->fb_height;
        target.format = VG_LITE_BGRA8888;
        target.stride = target.width * 4;
        target.tiled = VG_LITE_LINEAR;
        target.memory = (void *)dc->frame_buf;
        target.address = (uint32_t)dc->frame_buf;

        float vary_anlge = accelerate * angle / 180 * PI;

        float dx1 = 25 * cos(1.5 * vary_anlge);
        float dy1 = 25 * sin(1.5 * vary_anlge);
        float dx2 = 30 * cos(-vary_anlge);
        float dy2 = 30 * sin(-vary_anlge);
        float dx3 = 32 * cos(2 * vary_anlge);
        float dy3 = 25 * sin(2 * vary_anlge);
        float dx4 = 20 * cos(vary_anlge);
        float dy4 = 20 * sin(vary_anlge);
        float dx5 = 30 * sin(3 * vary_anlge);
        float dy5 = 10 * sin(3 * vary_anlge);

        float cx1 = 60 + dx1 - cx, cy1 = 60 + dy1 - cy;
        float cx2 = 394 + dx2 - cx, cy2 = 60 + dy2 - cy;
        float cx3 = 394 + dx3 - cx, cy3 = 384 + dy3 - cy;
        float cx4 = 70 + dx4 - cx, cy4 = 384 + dy4 - cy;
        float cx5 = 70 + 1.5 * dx5 - cx, cy5 = 227 + dy5 - cy;

        float cx6 = 68 - 0.5 * dx3 - cx, cy6 = 55 + dy3 - cy;
        float cx7 = 344 + dx1 - cx, cy7 = 55 + 0.8 * dy1 - cy;
        float cx8 = 385 + 0.77 * dx5 - cx, cy8 = 377 - dy5 - cy;
        float cx9 = 75 + dx4 - cx, cy9 = 380 + dy4 - cy;

        float cx10 = 80 + 1.5 * dx2 - cx, cy10 = 90 + dy2 - cy;
        float cx11 = 337 + dx3 - cx, cy11 = 130 + 1.5 * dy3 - cy;
        float cx12 = 350 + 0.77 * dx1 - cx, cy12 = 350 - dy1 - cy;
        float cx13 = 80 + dx5 - cx, cy13 = 366 + dy5 - cy;

        float arc_data[] =
        {
            (cx1 + cx5) / 2, (cy1 + cy5) / 2,
            cx1, cy1, (cx1 + cx2) / 2, (cy1 + cy2) / 2,
            cx2, cy2, (cx2 + cx3) / 2, (cy2 + cy3) / 2,
            cx3, cy3, (cx4 + cx3) / 2, (cy3 + cy4) / 2,
            cx4, cy4, (cx4 + cx5) / 2, (cy4 + cy5) / 2,
            cx5, cy5, (cx1 + cx5) / 2, (cy1 + cy5) / 2,
        };

        float arc_data2[] =
        {
            (cx6 + cx9) / 2, (cy6 + cy9) / 2,
            cx6, cy6, (cx6 + cx7) / 2, (cy6 + cy7) / 2,
            cx7, cy7, (cx7 + cx8) / 2, (cy7 + cy8) / 2,
            cx8, cy8, (cx8 + cx9) / 2, (cy8 + cy9) / 2,
            cx9, cy9, (cx9 + cx6) / 2, (cy9 + cy6) / 2,
        };

        float arc_data3[] =
        {
            (cx10 + cx13) / 2, (cy10 + cy13) / 2,
            cx10, cy10, cx10 * 0.4 + cx11 * 0.6, cy10 * 0.4 + cy11 * 0.6,
            cx11, cy11, (cx11 + cx12) / 2, (cy11 + cy12) / 2,
            cx12, cy12, (cx12 + cx13) / 2, (cy12 + cy13) / 2,
            cx13, cy13, (cx13 + cx10) / 2, (cy13 + cy10) / 2,
        };


        uint32_t data_size = vg_lite_path_calc_length(arc_cmd_bottom, sizeof(arc_cmd_bottom), VG_LITE_FP32);
        vg_lite_init_path(&path1, VG_LITE_FP32, VG_LITE_HIGH, data_size, path_data1, 454, 454, -454, -454);
        CHECK_ERROR(vg_lite_path_append(&path1, arc_cmd_bottom, arc_data, sizeof(arc_cmd_bottom)));

        vg_lite_init_path(&path2, VG_LITE_FP32, VG_LITE_HIGH, data_size, path_data2, 454, 454, -454, -454);
        CHECK_ERROR(vg_lite_path_append(&path2, arc_cmd, arc_data2, sizeof(arc_cmd)));

        vg_lite_init_path(&path3, VG_LITE_FP32, VG_LITE_HIGH, data_size, path_data3, 454, 454, -454, -454);
        CHECK_ERROR(vg_lite_path_append(&path3, arc_cmd, arc_data3, sizeof(arc_cmd)));


        CHECK_ERROR(vg_lite_clear(&target, NULL, 0x00000000));

        degrees = degrees + 0.333;
        vg_lite_rotate(degrees, &matrix_sec);
        vg_lite_rotate(degrees / 60.0, &matrix_minute);
        vg_lite_rotate((degrees / 60) / 12.0, &matrix_hour);

        vg_lite_scale(this->scale_x, this->scale_y, &matrix1);
        vg_lite_translate(this->t_x, this->t_y,     &matrix1);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix2);
        vg_lite_translate(this->t_x, this->t_y,     &matrix2);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix3);
        vg_lite_translate(this->t_x, this->t_y,     &matrix3);

        vg_lite_scale(this->scale_x, this->scale_y, &matrix_hour);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_hour);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix_minute);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_minute);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix_sec);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_sec);

        vg_lite_scale(this->scale_x, this->scale_y, &matrix_circle);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_circle);

        vg_lite_translate(cx * (1 - this->scale_x) + (this->scale_x) * (cx - 20),
                          cy * (1 - this->scale_y) + (this->scale_y) * (cy + 160), &matrix_sun);
        vg_lite_scale(2, 2, &matrix_sun);
        vg_lite_translate(cx * (1 - this->scale_x) + (this->scale_x) * (cx - 20),
                          cy * (1 - this->scale_y) + (this->scale_y) * (cy + 160), &matrix_sunshine1);
        vg_lite_translate(cx * (1 - this->scale_x) + (this->scale_x) * (cx + 5),
                          cy * (1 - this->scale_y) + (this->scale_y) * (cy + 163), &matrix_temp1);
        vg_lite_rotate(90, &matrix_temp1);
        vg_lite_scale(0.7, 0.05, &matrix_temp1);
        vg_lite_translate(cx * (1 - this->scale_x) + (this->scale_x) * (cx + 16.5),
                          cy * (1 - this->scale_y) + (this->scale_y) * (cy + 163), &matrix_temp2);
        vg_lite_rotate(90, &matrix_temp2);
        vg_lite_scale(0.7, 0.048, &matrix_temp2);
        vg_lite_translate(cx * (1 - this->scale_x) + (this->scale_x) * (cx + 62),
                          cy * (1 - this->scale_y) + (this->scale_y) * (cy + 160), &matrix_c_letter);
        vg_lite_translate(cx * (1 - this->scale_x) + (this->scale_x) * (cx + 35),
                          cy * (1 - this->scale_y) + (this->scale_y) * (cy + 155), &matrix_mark);

        vg_lite_scale(this->scale_x, this->scale_y, &matrix_sun);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_sun);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix_sunshine1);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_sunshine1);

        vg_lite_scale(this->scale_x, this->scale_y, &matrix_temp1);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_temp1);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix_temp2);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_temp2);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix_c_letter);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_c_letter);
        vg_lite_scale(this->scale_x, this->scale_y, &matrix_mark);
        vg_lite_translate(this->t_x, this->t_y,     &matrix_mark);

        vg_lite_identity(matGrad1);
        vg_lite_translate(path1.bounding_box[0], path1.bounding_box[1], matGrad1);
        vg_lite_translate(cx, cy, matGrad1);
        vg_lite_scale((path1.bounding_box[2] - path1.bounding_box[0]) / 256,
                      (path1.bounding_box[3] - path1.bounding_box[1]) / 256, matGrad1);
        vg_lite_translate((path1.bounding_box[2] - path1.bounding_box[0]) / 2,
                          (path1.bounding_box[3] - path1.bounding_box[1]) / 2, matGrad1);
        vg_lite_rotate((180 - angle) * (180 - angle) / 15, matGrad1);
        vg_lite_translate(-(path1.bounding_box[2] - path1.bounding_box[0]) / 2,
                          -(path1.bounding_box[3] - path1.bounding_box[1]) / 2, matGrad1);

        vg_lite_identity(matGrad2);
        vg_lite_translate(path2.bounding_box[0], path2.bounding_box[1], matGrad2);
        vg_lite_translate(cx, cy, matGrad3);
        vg_lite_scale((path2.bounding_box[2] - path2.bounding_box[0]) / 256,
                      (path2.bounding_box[3] - path2.bounding_box[1]) / 256, matGrad2);
        vg_lite_translate((path2.bounding_box[2] - path2.bounding_box[0]) / 2,
                          (path2.bounding_box[3] - path2.bounding_box[1]) / 2, matGrad2);
        vg_lite_rotate(angle, matGrad2);
        vg_lite_translate(-(path2.bounding_box[2] - path2.bounding_box[0]) / 2,
                          -(path2.bounding_box[3] - path2.bounding_box[1]) / 2, matGrad2);

        vg_lite_identity(matGrad3);
        vg_lite_translate(path3.bounding_box[0], path3.bounding_box[1], matGrad3);
        vg_lite_translate(cx, cy, matGrad3);
        vg_lite_scale((path3.bounding_box[2] - path3.bounding_box[0]) / 256,
                      (path3.bounding_box[3] - path3.bounding_box[1]) / 256, matGrad3);
        vg_lite_translate((path3.bounding_box[2] - path3.bounding_box[0]) / 2,
                          (path3.bounding_box[3] - path3.bounding_box[1]) / 2, matGrad3);
        vg_lite_rotate(-angle, matGrad3);
        vg_lite_translate(-(path3.bounding_box[2] - path3.bounding_box[0]) / 2,
                          -(path3.bounding_box[3] - path3.bounding_box[1]) / 2, matGrad3);

        CHECK_ERROR(vg_lite_draw_gradient(&target, &path1, VG_LITE_FILL_NON_ZERO, &matrix1, &grad1,
                                          VG_LITE_BLEND_SRC_OVER));
        CHECK_ERROR(vg_lite_draw_gradient(&target, &path2, VG_LITE_FILL_NON_ZERO, &matrix2, &grad2,
                                          VG_LITE_BLEND_SRC_OVER));
        CHECK_ERROR(vg_lite_draw_gradient(&target, &path3, VG_LITE_FILL_NON_ZERO, &matrix3, &grad3,
                                          VG_LITE_BLEND_SRC_OVER));
        CHECK_ERROR(vg_lite_draw(&target, &path_hour_1, VG_LITE_FILL_NON_ZERO, &matrix_hour,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &path_hour_2, VG_LITE_FILL_NON_ZERO, &matrix_hour,
                                 VG_LITE_BLEND_NONE, 0xFF999999));
        CHECK_ERROR(vg_lite_draw(&target, &path_minute, VG_LITE_FILL_NON_ZERO, &matrix_minute,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &path_sec, VG_LITE_FILL_NON_ZERO, &matrix_sec, VG_LITE_BLEND_NONE,
                                 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &out_circle, VG_LITE_FILL_NON_ZERO, &matrix_circle,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &in_circle, VG_LITE_FILL_NON_ZERO, &matrix_circle,
                                 VG_LITE_BLEND_NONE, 0xFF000000));
        CHECK_ERROR(vg_lite_draw(&target, &out_circle, VG_LITE_FILL_NON_ZERO, &matrix_sun,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_sunshine, VG_LITE_FILL_NON_ZERO, &matrix_sunshine1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        vg_lite_rotate(45, &matrix_sunshine1);
        CHECK_ERROR(vg_lite_draw(&target, &path_minute, VG_LITE_FILL_NON_ZERO, &matrix_temp1,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &path_minute, VG_LITE_FILL_NON_ZERO, &matrix_temp2,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &path_c_letter, VG_LITE_FILL_NON_ZERO, &matrix_c_letter,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(&target, &path_mark, VG_LITE_FILL_NON_ZERO, &matrix_mark,
                                 VG_LITE_BLEND_NONE, 0xFFE0E0E0));
        (vg_lite_finish());


        angle = angle + 1;
        if (angle > 180)
        {
            angle = angle - 360;
        }

    }

}
static void vg_lite_clock_end(gui_obj_t *obj)
{

}
static void vg_lite_clock_destory(gui_obj_t *obj)
{

}

void gui_vg_lite_scale(gui_vg_lite_clock_t *this, float scale_x, float scale_y)
{
    GUI_ASSERT(this != NULL);
    this->scale_x = scale_x;
    this->scale_y = scale_y;
}
void gui_vg_lite_translate(gui_vg_lite_clock_t *this, float t_x, float t_y)
{
    GUI_ASSERT(this != NULL);
    this->t_x = t_x;
    this->t_y = t_y;
}




void gui_vg_lite_clock_ctor(gui_vg_lite_clock_t *this, gui_obj_t *parent, const char *name,
                            void *addr,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = VG_LITE_CLOCK;
    root->obj_prepare = vg_lite_clock_prepare;
    root->obj_draw = vg_lite_clock_draw_cb;
    root->obj_end = vg_lite_clock_end;
    root->obj_destory = vg_lite_clock_destory;
    //for self
    this->scale_x = 1.0f;
    this->scale_y = 1.0f;

}



gui_vg_lite_clock_t *gui_vg_lite_clock_create(void *parent,  const char *name, void *data,
                                              int16_t x,
                                              int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_IMG";
    }
    gui_vg_lite_clock_t *vg_lite_clock = gui_malloc(sizeof(gui_vg_lite_clock_t));
    GUI_ASSERT(vg_lite_clock != NULL);
    memset(vg_lite_clock, 0x00, sizeof(gui_vg_lite_clock_t));

    gui_vg_lite_clock_ctor(vg_lite_clock, (gui_obj_t *)parent, name, data, x, y, w, h);
    gui_list_init(&(GET_BASE(vg_lite_clock)->child_list));
    if ((GET_BASE(vg_lite_clock)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(vg_lite_clock)->parent)->child_list),
                               &(GET_BASE(vg_lite_clock)->brother_list));
    }




    uint8_t line_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_LINE,
        VLC_OP_END
    };

    float hour_data[] =
    {
        0, -15,
        0, -100,
    };

    float minutes_data[] =
    {
        0, 0,
        0, -188
        ,
    };

    float sunshine_data[] =
    {
        0, 14,
        0, 18,
    };

    uint8_t second_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_LINE,
        VLC_OP_LINE,
        VLC_OP_END
    };

    float second_data[] =
    {
        -1, 0,
        0, -203,
        1, 0
    };

    uint8_t out_circle_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_SCCWARC,
        VLC_OP_SCCWARC,
        VLC_OP_END
    };

    float out_circle_data[] =
    {
        0, -5,
        5, 5, 0, 0, 5,
        5, 5, 0, 0, -5,
    };

    float mark_data[] =
    {
        0, -3,
        3, 3, 0, 0, 3,
        3, 3, 0, 0, -3,
    };

    uint8_t inner_circle_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_SCCWARC,
        VLC_OP_SCCWARC,
        VLC_OP_END
    };

    float inner_circle_data[] =
    {
        0, -2,
        2, 2, 0, 0, 2,
        2, 2, 0, 0, -2,
    };

    uint8_t arc_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_END,
    };

    uint8_t arc_cmd_bottom[] =
    {
        VLC_OP_MOVE,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_QUAD,
        VLC_OP_END,
    };


    uint8_t c_letter_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_LCCWARC,
    };


    float c_letter_data[] =
    {
        -9.16515 * 0.7, -6 * 0.7,
        10 * 0.7, 15 * 0.7, 0, -9.16515 * 0.7, 6 * 0.7,
    };



    uint32_t data_size;
    vg_lite_float_t dash[2] = {0, 0};


    data_size = vg_lite_path_calc_length(out_circle_cmd, sizeof(out_circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&out_circle, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *out_circle_path_data = gui_malloc(data_size);
    out_circle.path = out_circle_path_data;
    (vg_lite_path_append(&out_circle, out_circle_cmd, out_circle_data,
                         sizeof(out_circle_cmd)));

    data_size = vg_lite_path_calc_length(inner_circle_cmd, sizeof(inner_circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&in_circle, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *in_circle_path_data = gui_malloc(data_size);
    in_circle.path = in_circle_path_data;
    (vg_lite_path_append(&in_circle, inner_circle_cmd, inner_circle_data,
                         sizeof(inner_circle_cmd)));

    data_size = vg_lite_path_calc_length(line_cmd, sizeof(line_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_minute, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *path_minute_data = gui_malloc(data_size);
    path_minute.path = path_minute_data;
    (vg_lite_path_append(&path_minute, line_cmd, minutes_data, sizeof(line_cmd)));
    (vg_lite_set_stroke(&path_minute, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 4, 24, dash, 0,
                        0, 0xFFE0E0E0));
    (vg_lite_update_stroke(&path_minute));
    (vg_lite_set_draw_path_type(&path_minute, VG_LITE_DRAW_STROKE_PATH));

    data_size = vg_lite_path_calc_length(second_cmd, sizeof(second_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_sec, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *path_sec_data = gui_malloc(data_size);
    path_sec.path = path_sec_data;
    (vg_lite_path_append(&path_sec, second_cmd, second_data, sizeof(second_cmd)));
    (vg_lite_set_stroke(&path_sec, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 1.5, 12, dash, 0,
                        0, 0xFFE0E0E0));
    (vg_lite_update_stroke(&path_sec));
    (vg_lite_set_draw_path_type(&path_sec, VG_LITE_DRAW_STROKE_PATH));


    data_size = vg_lite_path_calc_length(line_cmd, sizeof(line_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_hour_1, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *path_hour_1_data = gui_malloc(data_size);
    path_hour_1.path = path_hour_1_data;
    (vg_lite_path_append(&path_hour_1, line_cmd, hour_data, sizeof(line_cmd)));
    (vg_lite_set_stroke(&path_hour_1, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 9, 18, dash, 0,
                        0, 0xFFE0E0E0));
    (vg_lite_update_stroke(&path_hour_1));
    (vg_lite_set_draw_path_type(&path_hour_1, VG_LITE_DRAW_STROKE_PATH));


    data_size = vg_lite_path_calc_length(line_cmd, sizeof(line_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_hour_2, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *path_hour_2_data = gui_malloc(data_size);
    path_hour_2.path = path_hour_2_data;
    (vg_lite_path_append(&path_hour_2, line_cmd, hour_data, sizeof(line_cmd)));
    (vg_lite_set_stroke(&path_hour_2, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 6, 10, dash, 0,
                        0, 0xFF999999));
    (vg_lite_update_stroke(&path_hour_2));
    (vg_lite_set_draw_path_type(&path_hour_2, VG_LITE_DRAW_STROKE_PATH));

    data_size = vg_lite_path_calc_length(line_cmd, sizeof(line_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_sunshine, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *path_sunshine_data = gui_malloc(data_size);
    path_sunshine.path = path_sunshine_data;
    (vg_lite_path_append(&path_sunshine, line_cmd, sunshine_data, sizeof(line_cmd)));
    (vg_lite_set_stroke(&path_sunshine, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 2, 18, dash, 0,
                        0, 0xFFE0E0E0));
    (vg_lite_update_stroke(&path_sunshine));
    (vg_lite_set_draw_path_type(&path_sunshine, VG_LITE_DRAW_STROKE_PATH));

    data_size = vg_lite_path_calc_length(arc_cmd_bottom, sizeof(arc_cmd_bottom), VG_LITE_FP32);
    vg_lite_init_path(&path1, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -240, -240, 240, 240);
    void *path_data1_data = gui_malloc(data_size);
    path_data1 = path_data1_data;

    vg_lite_init_path(&path2, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -240, -240, 240, 240);
    void *path_data2_data = gui_malloc(data_size);
    path_data2 = path_data2_data;

    vg_lite_init_path(&path3, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -240, -240, 240, 240);
    void *path_data3_data = gui_malloc(data_size);
    path_data3 = path_data3_data;

    data_size = vg_lite_path_calc_length(c_letter_cmd, sizeof(c_letter_cmd), VG_LITE_FP32);
    void *path_c_letter_data = gui_malloc(data_size);
    vg_lite_init_path(&path_c_letter, VG_LITE_FP32, VG_LITE_HIGH, data_size, path_c_letter_data, -50,
                      -50, 50, 50);
    (vg_lite_path_append(&path_c_letter, c_letter_cmd, c_letter_data, sizeof(c_letter_cmd)));
    (vg_lite_set_stroke(&path_c_letter, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 4, 18, dash, 0,
                        0, 0xFFE0E0E0));
    (vg_lite_update_stroke(&path_c_letter));
    (vg_lite_set_draw_path_type(&path_c_letter, VG_LITE_DRAW_STROKE_PATH));

    data_size = vg_lite_path_calc_length(out_circle_cmd, sizeof(out_circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_mark, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    void *mark_path_data = gui_malloc(data_size);
    path_mark.path = mark_path_data;
    (vg_lite_path_append(&path_mark, out_circle_cmd, mark_data,
                         sizeof(out_circle_cmd)));
    (vg_lite_set_stroke(&path_mark, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 2, 18, dash, 0,
                        0, 0xFFE0E0E0));
    (vg_lite_update_stroke(&path_mark));
    (vg_lite_set_draw_path_type(&path_mark, VG_LITE_DRAW_STROKE_PATH));

    uint32_t ramps[] = {0x8F000C12, 0x8F1E4f41, 0x8F3d5d4b, 0x8F1E4f41, 0x8F000C12};
    uint32_t stops[] = {0, 100, 120, 150, 255};

    uint32_t ramps2[] = {0x90001019, 0x90011D28, 0x90093533, 0x9012483D, 0x901E4f41, 0x903d5d4b, 0x90566856, 0x90607060};
    uint32_t stops2[] = {0, 30, 60, 90, 120, 160, 200, 255};

    uint32_t ramps3[] = {0x90001019, 0x90011D28, 0x90093533, 0x9012483D, 0x901E4f41, 0x903d5d4b, 0x90566856, 0x90607060};
    uint32_t stops3[] = {0, 30, 60, 120, 180, 220, 240, 255};


    memset(&grad1, 0, sizeof(grad1));
    vg_lite_init_grad(&grad1);
    vg_lite_set_grad(&grad1, 5, ramps, stops);
    vg_lite_update_grad(&grad1);
    matGrad1 = vg_lite_get_grad_matrix(&grad1);

    memset(&grad2, 0, sizeof(grad2));
    vg_lite_init_grad(&grad2);
    vg_lite_set_grad(&grad2, 8, ramps2, stops2);
    vg_lite_update_grad(&grad2);
    matGrad2 = vg_lite_get_grad_matrix(&grad2);

    memset(&grad3, 0, sizeof(grad3));
    vg_lite_init_grad(&grad3);
    vg_lite_set_grad(&grad3, 8, ramps3, stops3);
    vg_lite_update_grad(&grad3);
    matGrad3 = vg_lite_get_grad_matrix(&grad3);

    free(in_circle_path_data);
    free(out_circle_path_data);
    free(path_c_letter_data);
    free(mark_path_data);

    GET_BASE(vg_lite_clock)->create_done = true;
    return vg_lite_clock;
}



