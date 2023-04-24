
#include "rtthread.h"
#include "vg_lite.h"
#include "vg_lite_platform.h"
#include <rtdevice.h>
#include "rtl876x.h"

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
        rt_kprintf("[%s: %d] failed.error type is %s\n", __func__, __LINE__,error_type[error]);\
        RT_ASSERT(NULL != NULL); \
    }


#include "string.h"
#include "math.h"
#define PI                           3.141592653589793238462643383279502f
#define SINF(x)                      ((vg_lite_float_t) sin(x))
#define COSF(x)                      ((vg_lite_float_t) cos(x))




static void gpu_clock_entry(void *parameter)
{
    uint8_t minutes_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_LINE,
        VLC_OP_END
    };

    float minutes_data[] =
    {
        0, 0,
        0, 145,
    };

    uint8_t hour_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_LINE,
        VLC_OP_END
    };

    float hour_data[] =
    {
        0, 15,
        0, 80,
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
        0, 160,
        1, 0
    };

    vg_lite_cap_style_t cap_style[] =
    {
        VG_LITE_CAP_BUTT,
        VG_LITE_CAP_ROUND,
        VG_LITE_CAP_SQUARE
    };

    vg_lite_join_style_t join_style[] =
    {
        VG_LITE_JOIN_MITER,
        VG_LITE_JOIN_ROUND,
        VG_LITE_JOIN_BEVEL
    };

    vg_lite_draw_path_type_t draw_type[] =
    {
        VG_LITE_DRAW_FILL_PATH,
        VG_LITE_DRAW_STROKE_PATH
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

    vg_lite_path_t path_minute;
    vg_lite_path_t path_sec, path_hour_1, path_hour_2, in_circile, out_circle;

    vg_lite_matrix_t matrix_sec, matrix_minute, matrix_hour, matrix_circle;


    vg_lite_path_t path1, path2, path3;
    vg_lite_buffer_t target;
    vg_lite_buffer_t *fb;
    rt_memset(&target, 0x00, sizeof(vg_lite_buffer_t));

    vg_lite_matrix_t matrix1, matrix2, matrix3;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    rt_device_t dev = NULL;
    dev = rt_device_find("lcd");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    struct rt_device_graphic_info info;

    rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);

    target.width  = info.width;
    target.height = info.height;
    target.format = VG_LITE_BGRA8888;
    target.memory = (void *)info.framebuffer;
    target.address = (uint32_t)info.framebuffer;
    target.stride = target.width * 4;
    target.tiled = VG_LITE_LINEAR;
    fb = &target;
    uint32_t feature_check = 0;
    vg_lite_filter_t filter;

    uint32_t data_size;
    vg_lite_float_t dash[2] = {0, 0};

    uint32_t cx = info.width / 2;
    uint32_t cy = info.height / 2;

    vg_lite_identity(&matrix_sec);
    vg_lite_translate(cx, cy, &matrix_sec);

    vg_lite_identity(&matrix_minute);
    vg_lite_translate(cx, cy, &matrix_minute);

    vg_lite_identity(&matrix_hour);
    vg_lite_translate(cx, cy, &matrix_hour);

    vg_lite_identity(&matrix_circle);
    vg_lite_translate(cx, cy, &matrix_circle);

    data_size = vg_lite_path_calc_length(out_circle_cmd, sizeof(out_circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&out_circle, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    out_circle.path = malloc(data_size);
    CHECK_ERROR(vg_lite_path_append(&out_circle, out_circle_cmd, out_circle_data,
                                    sizeof(out_circle_cmd)));
    CHECK_ERROR(vg_lite_set_stroke(&out_circle, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 1, 10, dash, 0,
                                   0));
    CHECK_ERROR(vg_lite_update_stroke(&out_circle));
    CHECK_ERROR(vg_lite_set_draw_path_type(&out_circle, VG_LITE_DRAW_FILL_PATH));

    data_size = vg_lite_path_calc_length(inner_circle_cmd, sizeof(inner_circle_cmd), VG_LITE_FP32);
    vg_lite_init_path(&in_circile, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    in_circile.path = malloc(data_size);
    CHECK_ERROR(vg_lite_path_append(&in_circile, inner_circle_cmd, inner_circle_data,
                                    sizeof(inner_circle_cmd)));
    CHECK_ERROR(vg_lite_set_stroke(&in_circile, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 1, 10, dash, 0,
                                   0));
    CHECK_ERROR(vg_lite_update_stroke(&in_circile));
    CHECK_ERROR(vg_lite_set_draw_path_type(&in_circile, VG_LITE_DRAW_FILL_PATH));

    data_size = vg_lite_path_calc_length(minutes_cmd, sizeof(minutes_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_minute, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    path_minute.path = malloc(data_size);
    CHECK_ERROR(vg_lite_path_append(&path_minute, minutes_cmd, minutes_data, sizeof(minutes_cmd)));
    CHECK_ERROR(vg_lite_set_stroke(&path_minute, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 4, 24, dash, 0,
                                   0));
    CHECK_ERROR(vg_lite_update_stroke(&path_minute));
    CHECK_ERROR(vg_lite_set_draw_path_type(&path_minute, VG_LITE_DRAW_STROKE_PATH));

    data_size = vg_lite_path_calc_length(second_cmd, sizeof(second_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_sec, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    path_sec.path = malloc(data_size);
    CHECK_ERROR(vg_lite_path_append(&path_sec, second_cmd, second_data, sizeof(second_cmd)));
    CHECK_ERROR(vg_lite_set_stroke(&path_sec, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 1.5, 12, dash, 0,
                                   0));
    CHECK_ERROR(vg_lite_update_stroke(&path_sec));
    CHECK_ERROR(vg_lite_set_draw_path_type(&path_sec, VG_LITE_DRAW_STROKE_PATH));


    data_size = vg_lite_path_calc_length(hour_cmd, sizeof(hour_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_hour_1, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    path_hour_1.path = malloc(data_size);
    CHECK_ERROR(vg_lite_path_append(&path_hour_1, hour_cmd, hour_data, sizeof(hour_cmd)));
    CHECK_ERROR(vg_lite_set_stroke(&path_hour_1, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 8, 18, dash, 0,
                                   0));
    CHECK_ERROR(vg_lite_update_stroke(&path_hour_1));
    CHECK_ERROR(vg_lite_set_draw_path_type(&path_hour_1, VG_LITE_DRAW_STROKE_PATH));


    data_size = vg_lite_path_calc_length(hour_cmd, sizeof(hour_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path_hour_2, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -20, -20, 480, 480);
    path_hour_2.path = malloc(data_size);
    CHECK_ERROR(vg_lite_path_append(&path_hour_2, hour_cmd, hour_data, sizeof(hour_cmd)));
    CHECK_ERROR(vg_lite_set_stroke(&path_hour_2, VG_LITE_CAP_BUTT, VG_LITE_JOIN_ROUND, 5, 10, dash, 0,
                                   0));
    CHECK_ERROR(vg_lite_update_stroke(&path_hour_2));
    CHECK_ERROR(vg_lite_set_draw_path_type(&path_hour_2, VG_LITE_DRAW_STROKE_PATH));

    uint8_t arc_cmd[] =
    {
        VLC_OP_MOVE,
        VLC_OP_SCWARC,
        VLC_OP_SCWARC,
        VLC_OP_SCWARC,
        VLC_OP_SCWARC,
        VLC_OP_END
    };

    void *path_data1;
    void *path_data2;
    void *path_data3;
    data_size = vg_lite_path_calc_length(arc_cmd, sizeof(arc_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path1, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -cx, -cx, cy, cy);
    path_data1 = malloc(data_size);

    vg_lite_init_path(&path2, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -cx, -cx, cy, cy);
    path_data2 = malloc(data_size);

    vg_lite_init_path(&path3, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -cx, -cx, cy, cy);
    path_data3 = malloc(data_size);

    uint32_t ramps2[] = {0x8F001019, 0x8F011D28, 0x8F093533, 0x8F1E4f41, 0x8F3d5d4b, 0x8F566856, 0x8F566856, 0x8F011D28, 0x3300ff00, 0x330000ff, 0x33ffffff, 0x33ff0000};
    uint32_t stops2[] = {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 255};

    uint32_t ramps[] = {0x8F001019, 0x8F011D28, 0x8F093533, 0x8F12483D, 0x8F093533, 0x8F011D28, 0x8F001019, 0x8F011D28, 0x3300ff00, 0x330000ff, 0x33ffffff, 0x33ff0000};
    uint32_t stops[] = {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 255};

    uint32_t ramps3[] = {0x8F001019, 0x8F011D28, 0x8F093533, 0x8F12483D, 0x8F1E4f41, 0x8F3d5d4b, 0x8F566856, 0x8F607060, 0x3300ff00, 0x330000ff, 0x33ffffff, 0x33ff0000};
    uint32_t stops3[] = {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 255};

    vg_lite_linear_gradient_t grad1, grad2, grad3;

    vg_lite_matrix_t *matGrad1;
    memset(&grad1, 0, sizeof(grad1));
    vg_lite_init_grad(&grad1);
    vg_lite_set_grad(&grad1, 8, ramps, stops);
    vg_lite_update_grad(&grad1);
    matGrad1 = vg_lite_get_grad_matrix(&grad1);


    vg_lite_matrix_t *matGrad2;
    memset(&grad2, 0, sizeof(grad2));
    vg_lite_init_grad(&grad2);
    vg_lite_set_grad(&grad2, 7, ramps2, stops2);
    vg_lite_update_grad(&grad2);
    matGrad2 = vg_lite_get_grad_matrix(&grad2);

    vg_lite_matrix_t *matGrad3;
    memset(&grad3, 0, sizeof(grad3));
    vg_lite_init_grad(&grad3);
    vg_lite_set_grad(&grad3, 8, ramps3, stops3);
    vg_lite_update_grad(&grad3);
    matGrad3 = vg_lite_get_grad_matrix(&grad3);



    vg_lite_identity(matGrad1);
    vg_lite_translate(cx, cy, matGrad1);
    vg_lite_scale(1.25, 1.25, matGrad1);

    vg_lite_identity(matGrad2);
    vg_lite_translate(cx, cy, matGrad2);
    vg_lite_rotate(90, matGrad2);
    vg_lite_scale(1.2, 1.3, matGrad2);;

    vg_lite_identity(matGrad3);
    vg_lite_translate(cx, cy, matGrad3);
    vg_lite_rotate(133, matGrad3);
    vg_lite_scale(1.5, 1.3, matGrad3);
    int32_t i = 0, j = 0, k = 0, m = 0;
    float angle = -10;

    while (1)
    {
        rt_device_control(dev, RTGRAPHIC_CTRL_GET_INFO, &info);
        target.memory = (void *)info.framebuffer;
        target.address = (uint32_t)info.framebuffer;

        float delta_angle = angle * angle * angle;
        vg_lite_rotate((0.00075f * delta_angle), &matrix1);
        vg_lite_rotate((0.00075f * delta_angle), &grad1.matrix);
        vg_lite_rotate((0.0009f * delta_angle), &matrix2);
        vg_lite_rotate((- 0.001f * delta_angle), &grad2.matrix);
        vg_lite_rotate((- 0.0013f * delta_angle), &grad3.matrix);

        float angle_path_1 = 1.0f * j / 180.0f * PI;
        float cos_value = COSF(angle_path_1);
        float sin_value = SINF(angle_path_1);
        float offset_x = cos_value * 20;
        float offset_y = sin_value * 20;
        float offset_x2 = cos_value * 15;
        float offset_y2 = sin_value * 15;
        float offset_x3 = cos_value * 18;
        float offset_y3 = -sin_value * 18;
        int32_t m_value = (m - 110) * (m - 110);
        int32_t i_value = (i - 100) * (i - 100);
        int32_t j_value = (j - 180) * (j - 180);
        int32_t k_value = (k - 150) * (k - 150);
        float arc_data[] =
        {
            0 + offset_x, -50 - 0.0018f * m_value + offset_y,
            50 + 0.0012f * j_value, 50 + 0.0018f * m_value, 0, 50 + 0.0012f * j_value + offset_x, 0 + offset_y,
            50 + 0.0012f * j_value, 50 + 0.002f * i_value, 0, 0 + offset_x, 50 + 0.002f * i_value + offset_y,
            50 - 0.0015f * k_value, 50 + 0.002f * i_value, 0, -50 + 0.0015f * k_value + offset_x, 0 + offset_y,
            50 - 0.0015f * k_value, 50 + 0.0018f * m_value, 0, 0 + offset_x, -50 - 0.0018f * m_value + offset_y,
        };

        float arc_data2[] =
        {
            0 + offset_x2, -60 + 0.0018f * m_value + offset_y2,
            60 - 0.0011f * j_value, 60 + 0.0018f * m_value, 0, 60 - 0.0011f * j_value + offset_x2, 0 + offset_y2,
            60 - 0.0011f * j_value, 60 + 0.0014f * i_value, 0, 0 + offset_x2, 60 + 0.0014f * i_value + offset_y2,
            60 - 0.0009f * k_value, 60 + 0.0014f * i_value, 0, -60 + 0.0009f * k_value + offset_x2, 0 + offset_y2,
            60 - 0.0009f * k_value, 60 - 0.0018f * m_value, 0, 0 + offset_x2, -60 + 0.0018f * m_value + offset_y2,
        };

        float arc_data3[] =
        {
            0 + offset_x3, -60 + 0.0021f * m_value + offset_y3,
            70 - 0.0011f * j_value, 60 + 0.0021f * m_value, 0, 70 - 0.0011f * j_value + offset_x3, 0 + offset_y3,
            70 - 0.0011f * j_value, 60 + 0.002f * i_value, 0, 0 + offset_x3, 60 + 0.002f * i_value + offset_y3,
            70 - 0.0009f * k_value, 60 + 0.002f * i_value, 0, -70 + 0.0009f * k_value + offset_x3, 0 + offset_y3,
            70 - 0.0009f * k_value, 60 - 0.0021f * m_value, 0, 0 + offset_x3, -60 + 0.0021f * m_value + offset_y3,
        };

        data_size = vg_lite_path_calc_length(out_circle_cmd, sizeof(out_circle_cmd), VG_LITE_FP32);
        vg_lite_init_path(&path1, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -cx, -cx, cy, cy);
        path1.path = path_data1;
        CHECK_ERROR(vg_lite_path_append(&path1, arc_cmd, arc_data, sizeof(arc_cmd)));

        vg_lite_init_path(&path2, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -cx, -cx, cy, cy);
        path2.path = path_data2;
        CHECK_ERROR(vg_lite_path_append(&path2, arc_cmd, arc_data2, sizeof(arc_cmd)));

        vg_lite_init_path(&path3, VG_LITE_FP32, VG_LITE_HIGH, data_size, NULL, -cx, -cx, cy, cy);
        path3.path = path_data3;
        CHECK_ERROR(vg_lite_path_append(&path3, arc_cmd, arc_data3, sizeof(arc_cmd)));


        CHECK_ERROR(vg_lite_clear(fb, NULL, 0x00000000));
        vg_lite_draw_gradient(&target, &path2, VG_LITE_FILL_NON_ZERO, &matrix2, &grad2,
                              VG_LITE_BLEND_SRC_OVER);
        vg_lite_draw_gradient(&target, &path3, VG_LITE_FILL_NON_ZERO, &matrix3, &grad3,
                              VG_LITE_BLEND_SRC_OVER);

        vg_lite_draw_gradient(&target, &path1, VG_LITE_FILL_NON_ZERO, &matrix1, &grad1,
                              VG_LITE_BLEND_SRC_OVER);

        vg_lite_rotate(6.0f / 40, &matrix_sec);
        vg_lite_rotate(1.0f / 400, &matrix_minute);
        vg_lite_rotate(1.0f / 24000, &matrix_hour);



        CHECK_ERROR(vg_lite_draw(fb, &path_hour_1, VG_LITE_FILL_NON_ZERO, &matrix_hour, VG_LITE_BLEND_NONE,
                                 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(fb, &path_hour_2, VG_LITE_FILL_NON_ZERO, &matrix_hour, VG_LITE_BLEND_NONE,
                                 0xFF999999));
        CHECK_ERROR(vg_lite_draw(fb, &path_minute, VG_LITE_FILL_NON_ZERO, &matrix_minute,
                                 VG_LITE_BLEND_NONE,
                                 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(fb, &path_sec, VG_LITE_FILL_NON_ZERO, &matrix_sec, VG_LITE_BLEND_NONE,
                                 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(fb, &out_circle, VG_LITE_FILL_NON_ZERO, &matrix_circle, VG_LITE_BLEND_NONE,
                                 0xFFE0E0E0));
        CHECK_ERROR(vg_lite_draw(fb, &in_circile, VG_LITE_FILL_NON_ZERO, &matrix_circle, VG_LITE_BLEND_NONE,
                                 0xFF000000));


        CHECK_ERROR(vg_lite_finish());


        rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
        rt_device_control(dev, RTGRAPHIC_CTRL_RECT_UPDATE, NULL);
        rt_device_close(dev);
        rt_thread_delay(10);
        i++;
        j++;
        k++;
        m++;
        j = j % 360;
        i = i % 200;
        k = k % 300;
        m = m % 220;
        angle += 0.1;
        vg_lite_clear_path(&path1);
        vg_lite_clear_path(&path2);
        vg_lite_clear_path(&path3);
        if (angle >= 10)
        {
            angle = -10;
        }

    }

}

static int gpu_clock_test(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("gpu_demo",
                           gpu_clock_entry, RT_NULL,
                           4096 * 4, 25, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}

MSH_CMD_EXPORT(gpu_clock_test, lcd display test);
