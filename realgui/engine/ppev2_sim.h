/*
 * File      : draw_img.h
 */
#ifndef __PPEV2_SIM_H__
#define __PPEV2_SIM_H__




#ifdef __cplusplus
extern "C" {
#endif
#include "math.h"

#define SHM_DATA_SECTION

typedef struct
{
    int x;
    int y;
    uint32_t w;
    uint32_t h;
} ppe_rect_t;

typedef enum
{
    PPEV2_COLOR_KEY_DISABLE     = 0X0,
    PPEV2_COLOR_KEY_INSIDE         = 0X2,
    PPEV2_COLOR_KEY_OUTSIDE   = 0X3,
} PPEV2_COLOR_KEY_MODE;

typedef enum
{
    PPEV2_ABGR8888 = 0x0,
    PPEV2_ARGB8888,
    PPEV2_XBGR8888,
    PPEV2_XRGB8888,
    PPEV2_BGRA8888,
    PPEV2_RGBA8888,
    PPEV2_BGRX8888,
    PPEV2_RGBX8888,
    PPEV2_ABGR4444,
    PPEV2_ARGB4444,
    PPEV2_XBGR4444,
    PPEV2_XRGB4444,
    PPEV2_BGRA4444,
    PPEV2_RGBA4444,
    PPEV2_BGRX4444,
    PPEV2_RGBX4444,
    PPEV2_ABGR2222,
    PPEV2_ARGB2222,
    PPEV2_XBGR2222,
    PPEV2_XRGB2222,
    PPEV2_BGRA2222,
    PPEV2_RGBA2222,
    PPEV2_BGRX2222,
    PPEV2_RGBX2222,
    PPEV2_ABGR8565,
    PPEV2_ARGB8565,
    PPEV2_XBGR8565,
    PPEV2_XRGB8565,
    PPEV2_BGRA5658,
    PPEV2_RGBA5658,
    PPEV2_BGRX5658,
    PPEV2_RGBX5658,
    PPEV2_ABGR1555,
    PPEV2_ARGB1555,
    PPEV2_XBGR1555,
    PPEV2_XRGB1555,
    PPEV2_BGRA5551,
    PPEV2_RGBA5551,
    PPEV2_BGRX5551,
    PPEV2_RGBX5551,
    PPEV2_BGR888,
    PPEV2_RGB888,
    PPEV2_BGR565,
    PPEV2_RGB565,
    PPEV2_A8,
    PPEV2_X8,
    PPEV2_ABGR8666 = 0x32,
    PPEV2_ARGB8666,
    PPEV2_XBGR8666,
    PPEV2_XRGB8666,
    PPEV2_BGRA6668,
    PPEV2_RGBA6668,
    PPEV2_BGRX6668,
    PPEV2_RGBX6668,
} PPEV2_PIXEL_FORMAT;

typedef enum
{
    PPEV2_SUCCESS = 0x0,
    PPEV2_ERR_NULL_TARGET,
    PPEV2_ERR_NULL_SOURCE,
    PPEV2_ERR_INVALID_MATRIX,
    PPEV2_ERR_INVALID_RANGE,
} PPEV2_err;

typedef enum
{
    PPEV2_BYPASS_MODE,
    PPEV2_TRANSPARENT_MODE,
    PPEV2_SRC_OVER_MODE,
    PPEV2_CONST_MASK_MODE,
} PPEV2_BLEND_MODE;

typedef struct
{
    struct
    {
        uint8_t algorithm: 2;
        uint8_t feature_1: 2;
        uint8_t feature_2: 2;
        uint8_t pixel_bytes: 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} IMDC_file_header;

typedef struct
{
    float m[3][3];    /*! The 3x3 matrix itself, in [row][column] order. */
} ppe_matrix_t;

typedef struct
{
    uint32_t address;
    uint16_t width;
    uint16_t height;
    uint32_t const_color;
    uint16_t win_x_min;
    uint16_t win_x_max;
    uint16_t win_y_min;
    uint16_t win_y_max;
    uint32_t color_key_max;
    uint32_t color_key_min;
    PPEV2_COLOR_KEY_MODE color_key_enable;
    PPEV2_PIXEL_FORMAT format;
    uint8_t opacity;
    bool high_quality;
} ppe_buffer_t;

typedef struct
{
    float p[3];
} ppe_pox_t;

static void pos_transfer(ppe_matrix_t *matrix, ppe_pox_t *pox)
{
    ppe_pox_t temp;
    int row;

    /* Process all rows. */
    for (row = 0; row < 3; row++)
    {
        temp.p[row] = (matrix->m[row][0] * pox->p[0])
                      + (matrix->m[row][1] * pox->p[1])
                      + (matrix->m[row][2] * pox->p[2]);
    }
    temp.p[0] = temp.p[0] / temp.p[2];
    temp.p[1] = temp.p[1] / temp.p[2];
    temp.p[2] = 1;
    memcpy(pox, &temp, sizeof(temp));
}

static bool ppe_get_area(ppe_rect_t *result_rect, ppe_rect_t *source_rect, ppe_matrix_t *matrix,
                         ppe_buffer_t *buffer)
{
    ppe_pox_t pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = source_rect->x * 1.0f;
    pox.p[1] = source_rect->y * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];

    pox.p[0] = (source_rect->x + source_rect->w - 1) * 1.0f;
    pox.p[1] = source_rect->y * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = source_rect->x * 1.0f;
    pox.p[1] = (source_rect->y + source_rect->h - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = (source_rect->x + source_rect->w - 1) * 1.0f;
    pox.p[1] = (source_rect->y + source_rect->h - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    if (x_max <= -1 || x_min >= buffer->width || y_max <= -1 || y_min >= buffer->height)
    {
        result_rect->x = 0;
        result_rect->y = 0;
        result_rect->w = 0;
        result_rect->h = 0;
        return false;
    }
    if (x_min < 0)
    {
        x_min = 0;
    }
    else if (x_min > buffer->width - 1)
    {
        x_min = buffer->width - 1;
    }
    if (y_min < 0)
    {
        y_min = 0;
    }
    else if (y_min > buffer->height - 1)
    {
        y_min = buffer->height - 1;
    }
    result_rect->x = (int16_t)x_min;
    result_rect->y = (int16_t)y_min;

    if (x_max >= buffer->width)
    {
        x_max = buffer->width - 1;
    }
    else if (x_max < 0)
    {
        x_max = 0;
    }

    if (y_max >= buffer->height)
    {
        y_max = buffer->height - 1;
    }
    else if (y_max < 0)
    {
        y_max = 0;
    }

    result_rect->w = ceil(x_max) - result_rect->x + 1;
    result_rect->h = ceil(y_max) - result_rect->y + 1;

    if (result_rect->x + result_rect->w > buffer->width)
    {
        result_rect->w = buffer->width - result_rect->x;
    }
    if (result_rect->y + result_rect->h > buffer->height)
    {
        result_rect->h = buffer->height - result_rect->y;
    }
    if (isnan(y_min) || isnan(y_max) || isnan(x_min) || isnan(x_max) || result_rect->h == 0 ||
        result_rect->w == 0)
    {
        return false;
    }
    return true;
}

uint32_t os_lock(void)
{
    return 0;
}

void os_unlock(uint32_t s)
{
    return;
}

PPEV2_err PPEV2_Mask(ppe_buffer_t *dst, uint32_t color, ppe_rect_t *rect)
{
    GUI_ASSERT(NULL != NULL);
}

static void ppe_get_identity(ppe_matrix_t *matrix)
{
    /* Set identify matrix. */
    matrix->m[0][0] = 1.0f;
    matrix->m[0][1] = 0.0f;
    matrix->m[0][2] = 0.0f;
    matrix->m[1][0] = 0.0f;
    matrix->m[1][1] = 1.0f;
    matrix->m[1][2] = 0.0f;
    matrix->m[2][0] = 0.0f;
    matrix->m[2][1] = 0.0f;
    matrix->m[2][2] = 1.0f;
}

void ppe_mat_multiply(ppe_matrix_t *matrix, ppe_matrix_t *mult)
{
    ppe_matrix_t temp;
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

void ppe_translate(float x, float y, ppe_matrix_t *matrix)
{
    /* Set translation matrix. */
    ppe_matrix_t t = { { {1.0f, 0.0f, x},
            {0.0f, 1.0f, y},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    ppe_mat_multiply(matrix, &t);
}
PPEV2_err PPEV2_Blit_Inverse(ppe_buffer_t *dst, ppe_buffer_t *src, ppe_matrix_t *inverse,
                             ppe_rect_t *rect, PPEV2_BLEND_MODE mode)
{
    GUI_ASSERT(NULL != NULL);
}


#ifdef __cplusplus
}
#endif

#endif

