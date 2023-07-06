#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <nanovg.h>
#include <trace.h>
#include "vg_lite.h"

#define NANOVG_VGLITE_DEBUG
#ifdef NANOVG_VGLITE_DEBUG

#define NANOVG_LOG(format, ...) DBG_DIRECT(format, ##__VA_ARGS__)

#else

#define NANOVG_LOG(format, ...)

#endif

#define PATH_CMD_LEN 512
static uint8_t path_cmd[PATH_CMD_LEN];
static vg_lite_float_t path_data[PATH_CMD_LEN * 2];

static void multiply(vg_lite_matrix_t *matrix, vg_lite_matrix_t *mult)
{
    vg_lite_matrix_t temp;
    int row, column;

    /* Process all rows. */
    for (row = 0; row < 3; row++)
    {
        /* Process all columns. */
        for (column = 0; column < 3; column++)
        {
            /* Compute matrix entry. */
            temp.m[row][column] = (matrix->m[row][0] * mult->m[0][column])
                                  + (matrix->m[row][1] * mult->m[1][column])
                                  + (matrix->m[row][2] * mult->m[2][column]);
        }
    }

    /* Copy temporary matrix into result. */
    memcpy(matrix, &temp, sizeof(temp));
}

static int vglite_nvg_renderCreate(void *uptr)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" uptr = 0x%x\n", uptr);
    return 1;
}
static int vglite_nvg_renderCreateTexture(void *uptr, int type, int w, int h, int imageFlags,
                                          const unsigned char *data)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, type = %d, w = %d, h = %d, imageFlags = %d, data = 0x%x\n", uptr, type, w,
               h, imageFlags, data);
    return 0;
}
static int vglite_nvg_renderDeleteTexture(void *uptr, int image)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, image = %d\n", uptr, image);
    return 0;
}
static int vglite_nvg_renderUpdateTexture(void *uptr, int image, int x, int y, int w, int h,
                                          const unsigned char *data)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, image = %d, x = %d, y = %d, w = %d, h = %d, data = 0x%x\n", uptr, image,
               x, y, w, h, data);
    return 0;
}
static void vglite_nvg_renderViewport(void *uptr, float width, float height, float devicePixelRatio)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, width = %f, height = %f, devicePixelRatio = %f\n", uptr, width, height,
               devicePixelRatio);
    NVG_NOTUSED(uptr);
    NVG_NOTUSED(width);
    NVG_NOTUSED(height);
    NVG_NOTUSED(devicePixelRatio);
}
static int vglite_nvg_renderGetTextureSize(void *uptr, int image, int *w, int *h)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x, image = %d, w = 0x%x, h = 0x%x\n", uptr, image, w, h);
    return 0;
}
static void vglite_nvg_renderCancel(void *uptr)
{
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x\n", uptr);
}

static void vglite_nvg_renderFlush(void *uptr)
{
    NVG_NOTUSED(uptr);
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    NANOVG_LOG(" uptr = 0x%x\n", uptr);

    vg_lite_finish();
}
static void vglite_nvg_renderDelete(void *uptr)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
}

void vglite_nvg_renderStroke(void *uptr, NVGpaint *paint,
                             NVGcompositeOperationState compositeOperation,
                             NVGscissor *scissor, float fringe, float strokeWidth, const NVGpath *paths,
                             int npaths)
{
    NANOVG_LOG(" %s %d\n", __func__, __LINE__);
    vg_lite_buffer_t *target = (vg_lite_buffer_t *)uptr;

    uint32_t cmd_cnt = 0;
    uint32_t data_cnt = 0;

    for (int i = 0; i < npaths; i++)
    {
        const NVGpath *p = paths + i;
        for (int j = 0; j < p->nstroke; j++)
        {
            const NVGvertex *v = p->stroke + j;
            if (j == 0)
            {
                path_cmd[cmd_cnt++] = VLC_OP_MOVE;
                path_data[data_cnt++] = v->x;
                path_data[data_cnt++] = v->y;
            }
            else
            {
                path_cmd[cmd_cnt++] = VLC_OP_LINE;
                path_data[data_cnt++] = v->x;
                path_data[data_cnt++] = v->y;
            }
        }
        //path_cmd[cmd_cnt++] = VLC_OP_END;
    }
    if (cmd_cnt >= PATH_CMD_LEN)
    {
        NANOVG_LOG(" ERROR !%s %d, cmd_cnt >= PATH_CMD_LEN\n", __func__, __LINE__);
        *(uint32_t *)0xFFFFFFFF = 0;
        while (1);
    }


    vg_lite_path_t path;
    memset(&path, 0, sizeof(vg_lite_path_t));
    uint32_t path_data_len = vg_lite_path_calc_length(path_cmd, sizeof(path_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, 0, 0, 0, 0);
    vg_lite_path_append(&path, path_cmd, path_data, cmd_cnt);
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);


    uint8_t a = paint->innerColor.a * 0xff;
    uint8_t r = paint->innerColor.r * 0xff * paint->innerColor.a;
    uint8_t g = paint->innerColor.g * 0xff * paint->innerColor.a;
    uint8_t b = paint->innerColor.b * 0xff * paint->innerColor.a;


    vg_lite_color_t color = (a << 24) | (b << 16) | (g << 8) | r;

    vg_lite_set_stroke(&path, VG_LITE_CAP_ROUND, VG_LITE_JOIN_ROUND, strokeWidth, 60, NULL, 0, 0,
                       color);
    vg_lite_update_stroke(&path);
    vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_STROKE_PATH);
    vg_lite_draw(target, &path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_SRC_OVER, color);
    vg_lite_finish();
    vg_lite_clear_path(&path);

}



void vglite_nvg_renderFill(void *uptr, NVGpaint *paint,
                           NVGcompositeOperationState compositeOperation,
                           NVGscissor *scissor, float fringe, const float *bounds, const NVGpath *paths,
                           int npaths)
{
    vg_lite_buffer_t *target = (vg_lite_buffer_t *)uptr;

    uint32_t cmd_cnt = 0;
    uint32_t data_cnt = 0;

    for (int i = 0; i < npaths; i++)
    {
        const NVGpath *p = paths + i;
        for (int j = 0; j < p->nfill; j++)
        {
            const NVGvertex *v = p->fill + j;
            if (j == 0)
            {
                path_cmd[cmd_cnt++] = VLC_OP_MOVE;
                path_data[data_cnt++] = v->x;
                path_data[data_cnt++] = v->y;
            }
            else
            {
                path_cmd[cmd_cnt++] = VLC_OP_LINE;
                path_data[data_cnt++] = v->x;
                path_data[data_cnt++] = v->y;
            }
            NANOVG_LOG(" %s j = %d\n", __func__, j);
        }
        path_cmd[cmd_cnt++] = VLC_OP_END;
    }
    if (cmd_cnt >= PATH_CMD_LEN)
    {
        NANOVG_LOG(" ERROR !%s %d, cmd_cnt >= PATH_CMD_LEN\n", __func__, __LINE__);
        *(uint32_t *)0xFFFFFFFF = 0;
        while (1);
    }

    vg_lite_path_t path;
    memset(&path, 0, sizeof(vg_lite_path_t));
    uint32_t path_data_len = vg_lite_path_calc_length(path_cmd, sizeof(path_cmd), VG_LITE_FP32);
    vg_lite_init_path(&path, VG_LITE_FP32, VG_LITE_HIGH, path_data_len, NULL, 0, 0, 0, 0);
    vg_lite_path_append(&path, path_cmd, path_data, cmd_cnt);
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);


    uint8_t inner_r = paint->innerColor.r * 0xff;
    uint8_t inner_g = paint->innerColor.g * 0xff;
    uint8_t inner_b = paint->innerColor.b * 0xff;
    uint8_t inner_a = paint->innerColor.a * 0xff;
    uint8_t outer_r = paint->outerColor.r * 0xff;
    uint8_t outer_g = paint->outerColor.g * 0xff;
    uint8_t outer_b = paint->outerColor.b * 0xff;
    uint8_t outer_a = paint->outerColor.a * 0xff;

    uint32_t inner_color = (inner_a << 24) | (inner_b << 0) | (inner_g << 8) | (inner_r << 16);
    uint32_t outer_color = (outer_a << 24) | (outer_b << 0) | (outer_g << 8) | (outer_r << 16);

    if (memcmp(&(paint->innerColor), &(paint->outerColor), sizeof(paint->outerColor)) == 0)
    {
        vg_lite_draw(target, &path, VG_LITE_FILL_NON_ZERO, &matrix, VG_LITE_BLEND_NONE, inner_color);
    }
    else if (paint->radius == 0)
    {
        NANOVG_LOG(" %s %d\n", __func__, __LINE__);
        const float large = 1e5;
        float dx = paint->xform[2];
        float dy = paint->xform[3];
        float d = (paint->extent[1] - large) * 2;
        float sx = paint->xform[4] + dx * large;
        float sy = paint->xform[5] + dy * large;
        float ex = sx + d * dx;
        float ey = sy + d * dy;

        vg_lite_linear_gradient_t grad;
        memset(&grad, 0, sizeof(vg_lite_linear_gradient_t));
        vg_lite_init_grad(&grad);


        uint32_t colors[] = {inner_color, outer_color};
        uint32_t stops[] = {0, VLC_GRADIENT_BUFFER_WIDTH - 1};
        vg_lite_set_grad(&grad, 2, colors, stops);
        vg_lite_update_grad(&grad);
        vg_lite_matrix_t *gradMatrix = vg_lite_get_grad_matrix(&grad);
        vg_lite_identity(gradMatrix);

        vg_lite_float_t cos_angle = dy;
        vg_lite_float_t sin_angle = dx;
        /* Set rotation matrix. */
        vg_lite_matrix_t r = { { {cos_angle, -sin_angle, 0.0f},
                {sin_angle, cos_angle, 0.0f},
                {0.0f, 0.0f, 1.0f}
            }
        };
        multiply(gradMatrix, &r);

        vg_lite_scale(d / VLC_GRADIENT_BUFFER_WIDTH, 1, gradMatrix);
        vg_lite_draw_gradient(target, &path, VG_LITE_FILL_EVEN_ODD, &matrix, &grad, VG_LITE_BLEND_NONE);
    }
    else
    {
        float cx = paint->xform[4];
        float cy = paint->xform[5];
        float inr = paint->radius - paint->feather / 2;
        float outr = paint->radius + paint->feather / 2;
    }

    vg_lite_finish();
    vg_lite_clear_path(&path);
}


NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                          uint8_t *data)
{
    NVGparams params;
    NVGcontext *ctx = NULL;
    static vg_lite_buffer_t target;
    memset(&target, 0x00, sizeof(vg_lite_buffer_t));
    target.width  = w;
    target.height = h;
    target.format = VG_LITE_RGBA8888;
    target.memory = (void *)data;
    target.address = (uint32_t)data;
    target.stride = target.width * 4;


    memset(&params, 0, sizeof(params));
    params.renderCreate = vglite_nvg_renderCreate;
    params.renderCreateTexture = vglite_nvg_renderCreateTexture;
    params.renderDeleteTexture = vglite_nvg_renderDeleteTexture;
    params.renderUpdateTexture = vglite_nvg_renderUpdateTexture;
    params.renderGetTextureSize = vglite_nvg_renderGetTextureSize;
    params.renderViewport = vglite_nvg_renderViewport;
    params.renderCancel = vglite_nvg_renderCancel;
    params.renderFlush = vglite_nvg_renderFlush;
    params.renderDelete = vglite_nvg_renderDelete;
    params.renderTriangles = NULL;
    params.renderStroke = vglite_nvg_renderStroke;
    params.renderFill = vglite_nvg_renderFill;
    params.userPtr = (void *)&target;
    params.edgeAntiAlias = 1;


    ctx = nvgCreateInternal(&params);
    if (ctx == NULL) { goto error; }

    return ctx;

error:
    if (ctx != NULL) { nvgDeleteInternal(ctx); }
    return NULL;
}

void nvgDeleteAGGE(NVGcontext *ctx)
{
    nvgDeleteInternal(ctx);
}
