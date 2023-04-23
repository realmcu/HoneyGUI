/****************************************************************************
*
*    Copyright 2012 - 2022 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#include "vg_lite_context.h"

#define MATRIX_FP_ABS(x)            (((x) < 0) ? -(x) : (x))
#define MATRIX_FP_EPS               2.2204460492503131e-14


/* Get the plane memory pointer and strides info. */
static uint32_t get_buffer_planes(vg_lite_buffer_t *buffer,
                              uint8_t **memory,
                              uint32_t *strides)
{
    uint32_t count = 1;
    
    switch (buffer->format) {
        case VG_LITE_RGBA8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBX8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGB565:
        case VG_LITE_BGR565:
        case VG_LITE_RGBA4444:
        case VG_LITE_BGRA4444:
        case VG_LITE_BGRA5551:
        case VG_LITE_A8:
        case VG_LITE_L8:
        case VG_LITE_A4:
        case VG_LITE_INDEX_1:
        case VG_LITE_INDEX_2:
        case VG_LITE_INDEX_4:
        case VG_LITE_INDEX_8:
        case VG_LITE_YUYV:
        case VG_LITE_YUY2:
        case VG_LITE_RGBA2222:
            count = 1;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = memory[2] = ((uint8_t*)0);
            strides[0] = buffer->stride;
            strides[1] = strides[2] = 0;
            break;
            
        case VG_LITE_NV12:
        case VG_LITE_NV16:
            count = 2;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = (uint8_t *)buffer->yuv.uv_memory;
            memory[2] = 0;
            strides[0] = buffer->stride;
            strides[1] = buffer->yuv.uv_stride;
            strides[2] = 0;
            break;
            
        case VG_LITE_AYUY2:
            count = 2;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = 0;
            memory[2] = (uint8_t *)buffer->yuv.v_memory;
            strides[0] = buffer->stride;
            strides[1] = 0;
            strides[2] = buffer->yuv.alpha_stride;
            break;

        case VG_LITE_ANV12:
            count = 3;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = (uint8_t *)buffer->yuv.uv_memory;
            memory[2] = (uint8_t *)buffer->yuv.v_memory;
            strides[0] = buffer->stride;
            strides[1] = buffer->yuv.uv_stride;
            strides[2] = buffer->yuv.alpha_stride;
            break;
            
        case VG_LITE_YV12:
        case VG_LITE_YV24:
        case VG_LITE_YV16:
            count = 3;
            memory[0] = (uint8_t *)buffer->memory;
            memory[1] = (uint8_t *)buffer->yuv.uv_memory;
            memory[2] = (uint8_t *)buffer->yuv.v_memory;
            strides[0] = buffer->stride;
            strides[1] = buffer->yuv.uv_stride;
            strides[2] = buffer->yuv.v_stride;
            break;
            
        case VG_LITE_YUY2_TILED:
        case VG_LITE_NV12_TILED:
        case VG_LITE_ANV12_TILED:
        case VG_LITE_AYUY2_TILED:
        default:
            count = 0;
            
            break;
    }
    return count;
}

vg_lite_error_t vg_lite_upload_buffer(vg_lite_buffer_t  *buffer,
                                      vg_lite_uint8_t *data[3],
                                      vg_lite_uint32_t stride[3])
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    int32_t plane_count;
    uint8_t  *buffer_memory[3] = {((uint8_t*)0)};
    uint32_t  buffer_strides[3] = {0};
    uint8_t  *pdata;
    int32_t i, j;

    /* Get buffer memory info. */
    plane_count = get_buffer_planes(buffer, buffer_memory, buffer_strides);

    if (plane_count > 0 && plane_count <= 3) {
        /* Copy the data to buffer. */
        for (i = 0; i < plane_count;  i++) {
            pdata = data[i];
            for (j = 0; j < buffer->height; j++) {
                memcpy(buffer_memory[i], pdata, buffer_strides[i]);
                buffer_memory[i] += buffer_strides[i];
                pdata += stride[i];
            }
        }
    }
    else {
        error = VG_LITE_INVALID_ARGUMENT;
    }

    return error;
}

static vg_lite_error_t swap(float* a, float* b)
{
    float temp;
    if (a == NULL || b == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    temp = *a;
    *a = *b;
    *b = temp;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_get_transform_matrix(vg_lite_point4_t src, vg_lite_point4_t dst, vg_lite_matrix_t* mat)
{
    float a[8][8], b[9], A[64];
    int i, j, k, m = 8, n = 1;
    int astep = 8, bstep = 1;
    float d;

    if (src == NULL || dst == NULL || mat == NULL)
        return  VG_LITE_INVALID_ARGUMENT;

    for (i = 0; i < 4; ++i)
    {
        a[i][0] = a[i + 4][3] = (float)src[i].x;
        a[i][1] = a[i + 4][4] = (float)src[i].y;
        a[i][2] = a[i + 4][5] = 1.0f;
        a[i][3] = a[i][4] = a[i][5] =
            a[i + 4][0] = a[i + 4][1] = a[i + 4][2] = 0.0f;
        a[i][6] = (float)(-src[i].x * dst[i].x);
        a[i][7] = (float)(-src[i].y * dst[i].x);
        a[i + 4][6] = (float)(-src[i].x * dst[i].y);
        a[i + 4][7] = (float)(-src[i].y * dst[i].y);
        b[i] = (float)dst[i].x;
        b[i + 4] = (float)dst[i].y;
    }
    for (i = 0; i < 8; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            A[8 * i + j] = a[i][j];
        }
    }

    for (i = 0; i < m; i++)
    {
        k = i;
        for (j = i + 1; j < m; j++)
            if (MATRIX_FP_ABS(A[j * astep + i]) > MATRIX_FP_ABS(A[k * astep + i]))
                k = j;
        if (MATRIX_FP_ABS(A[k * astep + i]) < MATRIX_FP_EPS)
            return VG_LITE_INVALID_ARGUMENT;
        if (k != i)
        {
            for (j = i; j < m; j++)
                swap(&A[i * astep + j], &A[k * astep + j]);
            for (j = 0; j < n; j++)
                swap(&b[i * bstep + j], &b[k * bstep + j]);
        }
        d = -1 / A[i * astep + i];
        for (j = i + 1; j < m; j++)
        {
            float alpha = A[j * astep + i] * d;
            for (k = i + 1; k < m; k++)
                A[j * astep + k] += alpha * A[i * astep + k];
            for (k = 0; k < n; k++)
                b[j * bstep + k] += alpha * b[i * bstep + k];
        }
    }

    for (i = m - 1; i >= 0; i--)
        for (j = 0; j < n; j++)
        {
            float s = b[i * bstep + j];
            for (k = i + 1; k < m; k++)
                s -= A[i * astep + k] * b[k * bstep + j];
            b[i * bstep + j] = s / A[i * astep + i];
        }

    b[8] = 1;

    for (i = 0; i < 3; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            mat->m[i][j] = b[i * 3 + j];
        }
    }

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_scissor(vg_lite_int32_t x, vg_lite_int32_t y, vg_lite_int32_t right, vg_lite_int32_t bottom)
{
#if gcFEATURE_VG_SCISSOR
    vg_lite_error_t error = VG_LITE_SUCCESS;

    /* Save scissor Box States. */
    s_context.scissor[0] = x;
    s_context.scissor[1] = y;
    s_context.scissor[2] = right;
    s_context.scissor[3] = bottom;

    /* Scissor dirty. */
    s_context.scissor_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_enable_scissor()
{
#if gcFEATURE_VG_SCISSOR
    /* Enable scissor Mode. */
    s_context.scissor_enabled = 1;

    /* Scissor dirty. */
    s_context.scissor_dirty = 1;
#if gcFEATURE_VG_MASK
    s_context.scissor_enable = 1 << 4;
#endif

    return VG_LITE_SUCCESS;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_disable_scissor()
{
#if gcFEATURE_VG_SCISSOR
    /* disable scissor Mode. */
    s_context.scissor_enabled = 0;

    /* Scissor dirty. */
    s_context.scissor_dirty = 1;
#if gcFEATURE_VG_MASK
    s_context.scissor_enable = 0;
#endif

    return VG_LITE_SUCCESS;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_CLUT(vg_lite_uint32_t count, vg_lite_uint32_t* colors)
{
#if gcFEATURE_VG_IM_INDEX_FORMAT
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t addr = 0x0B00;

#if gcFEATURE_VG_NEW_IMAGE_INDEX
    {
        switch (count) {
        case 256:
        case 16:
        case 4:
        case 2:
            addr = 0x0B00;
            break;
        default:
            error = VG_LITE_INVALID_ARGUMENT;
            return error;
            break;
        }
    }
#else
    {
        switch (count) {
        case 256:
            addr = 0x0B00;
            break;
        case 16:
            addr = 0x0AA0;
            break;
        case 4:
            addr = 0x0A9C;
            break;
        case 2:
            addr = 0x0A98;
            break;
        default:
            error = VG_LITE_INVALID_ARGUMENT;
            return error;
            break;
        }
    }
#endif

    VG_LITE_RETURN_ERROR(push_clut(&s_context, addr, count, colors));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_source_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value)
{
#if gcFEATURE_VG_GLOBAL_ALPHA
    uint32_t image_alpha_mode;
    uint32_t image_alpha_value;

    image_alpha_mode = (uint8_t)alpha_mode;
    image_alpha_value = alpha_value << 2;

    s_context.src_alpha_mode = image_alpha_mode;
    s_context.src_alpha_value = image_alpha_value;

    return VG_LITE_SUCCESS;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_dest_global_alpha(vg_lite_global_alpha_t alpha_mode, vg_lite_uint8_t alpha_value)
{
#if gcFEATURE_VG_GLOBAL_ALPHA
    uint32_t dest_alpha_mode;
    uint32_t dest_alpha_value;

    dest_alpha_mode = (alpha_mode == VG_LITE_NORMAL) ? 0 : (alpha_mode == VG_LITE_GLOBAL) ? 0x00000400 : 0x00000800;
    dest_alpha_value = alpha_value << 12;

    s_context.dst_alpha_mode = dest_alpha_mode;
    s_context.dst_alpha_value = dest_alpha_value;

    return VG_LITE_SUCCESS;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_color_key(vg_lite_color_key4_t colorkey)
{
#if gcFEATURE_VG_COLOR_KEY
    uint8_t i;
    uint32_t value_low = 0;
    uint32_t value_high = 0;
    uint8_t r, g, b, a, e;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    /* Set color key states. */
    for (i = 0; i < 4; i++)
    {
        /* Set gcregVGPEColorKeyLow. Layout "E/R/G/B". */
        r = colorkey[i].low_r;
        g = colorkey[i].low_g;
        b = colorkey[i].low_b;
        e = colorkey[i].enable;
        value_low = (e << 24) | (r << 16) | (g << 8) | b;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A90 + i, value_low));

        /* Set gcregVGPEColorKeyHigh. Layout "A/R/G/B". */
        r = colorkey[i].hign_r;
        g = colorkey[i].hign_g;
        b = colorkey[i].hign_b;
        a = colorkey[i].alpha;
        value_high = (a << 24) | (r << 16) | (g << 8) | b;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A94 + i, value_high));
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_enable_dither()
{
#if gcFEATURE_VG_DITHER
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t table_low = 0x7B48F3C0;
    uint32_t table_high = 0x596AD1E2;

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5A, table_low));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5B, table_high));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_disable_dither()
{
#if gcFEATURE_VG_DITHER
    vg_lite_error_t error = VG_LITE_SUCCESS;

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5A, 0xFFFFFFFF));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5B, 0xFFFFFFFF));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_enable_masklayer()
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

    s_context.enable_mask = (1 << 20);

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_disable_masklayer()
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

    s_context.enable_mask = 0;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_create_masklayer(vg_lite_buffer_t* masklayer, vg_lite_uint32_t width, vg_lite_uint32_t height)
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

    memset(masklayer, 0, sizeof(vg_lite_buffer_t));
    masklayer->width = width;
    masklayer->height = height;
    masklayer->format = VG_LITE_A8;
    error = vg_lite_allocate(masklayer);
    error = vg_lite_clear(masklayer, NULL, 0xFF << 24);

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_fill_masklayer(vg_lite_buffer_t* masklayer, vg_lite_rectangle_t* rect, vg_lite_uint8_t value)
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

    error = vg_lite_clear(masklayer, rect, value << 24);

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_blend_masklayer(
    vg_lite_buffer_t* dst_masklayer,
    vg_lite_buffer_t* src_masklayer,
    vg_lite_mask_operation_t Operation,
    vg_lite_rectangle_t* rect
)
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;

    vg_lite_identity(&matrix);

    switch (Operation)
    {
    case VG_LITE_CLEAR_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_clear(dst_masklayer, rect, 0x0));
        break;
    case VG_LITE_FILL_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_clear(dst_masklayer, rect, 0xFF << 24));
        break;
    case VG_LITE_SET_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, rect, &matrix, VG_LITE_BLEND_NONE, 0, filter));
        break;
    case VG_LITE_UNION_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, rect, &matrix, VG_LITE_BLEND_SCREEN, 0, filter));
        break;
    case VG_LITE_INTERSECT_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, rect, &matrix, VG_LITE_BLEND_DST_IN, 0, filter));
        break;
    case VG_LITE_SUBTRACT_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_blit_rect(dst_masklayer, src_masklayer, rect, &matrix, VG_LITE_BLEND_SUBTRACT, 0, filter));
        break;
    default:
        break;
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_masklayer(vg_lite_buffer_t* masklayer)
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A14, masklayer->address));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A15, masklayer->stride));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00000010));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_render_masklayer(
    vg_lite_buffer_t* masklayer,
    vg_lite_mask_operation_t operation,
    vg_lite_path_t* path,
    vg_lite_fill_t fill_rule,
    vg_lite_color_t color,
    vg_lite_matrix_t* matrix
)
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

    switch (operation)
    {
    case VG_LITE_CLEAR_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_NONE, 0));
        break;
    case VG_LITE_FILL_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_NONE, 0xFF << 24));
        break;
    case VG_LITE_SET_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_NONE, color << 24));
        break;
    case VG_LITE_UNION_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_SCREEN, color << 24));
        break;
    case VG_LITE_INTERSECT_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_DST_IN, color << 24));
        break;
    case VG_LITE_SUBTRACT_MASK:
        VG_LITE_RETURN_ERROR(vg_lite_draw(masklayer, path, fill_rule, matrix, VG_LITE_BLEND_SUBTRACT, color << 24));
        break;
    default:
        break;
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_destroy_masklayer(vg_lite_buffer_t* masklayer)
{
#if gcFEATURE_VG_MASK
    vg_lite_error_t error = VG_LITE_SUCCESS;

    VG_LITE_RETURN_ERROR(vg_lite_free(masklayer));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_pixel_matrix(vg_lite_pixel_matrix_t matrix, vg_lite_pixel_channel_enable_t* channel)
{
#if gcFEATURE_VG_PIXEL_MATRIX
    vg_lite_error_t error = VG_LITE_SUCCESS;
    short pix_matrix[20] = { 0 };
    int i = 0;

    s_context.matrix_enable = (channel->enable_a ? (1 << 17) : 0) |
        (channel->enable_r ? (1 << 23) : 0) |
        (channel->enable_g ? (1 << 22) : 0) |
        (channel->enable_b ? (1 << 21) : 0);

    if (s_context.matrix_enable)
    {
        for (i = 0; i < 20; i++) {
            if (matrix[i] > 127.0f || matrix[i] < -128.0f) {
                return VG_LITE_INVALID_ARGUMENT;
            }
            pix_matrix[i] = (short)(matrix[i] * 256);
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0ADE + i, pix_matrix[i]));
        }
    }

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_gaussian_filter(vg_lite_float_t w0, vg_lite_float_t w1, vg_lite_float_t w2)
{
#if gcFEATURE_VG_GAUSSIAN_BLUR
    vg_lite_error_t error = VG_LITE_SUCCESS;

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD2 + 1, (uint32_t)(w0 * 256)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD4 + 1, (uint32_t)(w1 * 256)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD6 + 1, (uint32_t)(w2 * 256)));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_scissor_rects(vg_lite_uint32_t nums, vg_lite_rectangle_t rect[])
{
#if gcFEATURE_VG_MASK
    int i = 0, j = 0, k = 0;
    uint32_t temp[100];
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    vg_lite_buffer_t scissor_dst_layer;
    vg_lite_buffer_t scissor_src_layer;

    vg_lite_identity(&matrix);
    memset(&scissor_dst_layer, 0, sizeof(vg_lite_buffer_t));
    memset(&scissor_src_layer, 0, sizeof(vg_lite_buffer_t));
    scissor_src_layer.width = scissor_dst_layer.width = s_context.target_width;
    scissor_src_layer.height = scissor_dst_layer.height = s_context.target_height;
    scissor_src_layer.format = scissor_dst_layer.format = VG_LITE_A8;
    error = vg_lite_allocate(&scissor_dst_layer);
    error = vg_lite_clear(&scissor_dst_layer, NULL, 0x0);
    error = vg_lite_allocate(&scissor_src_layer);
    error = vg_lite_clear(&scissor_src_layer, NULL, 0xFFFFFFFF);
    /* One bit data of scissor layer corresponds to one pixel, so adjust the size of the rect area. */
    /* Determine how many edge pixels need to be rendered. */
    for (i = 0, k = 0; i < (int)nums; i++, k += 2) {
        temp[k] = rect[i].x % 8;
        temp[k + 1] = (rect[i].x + rect[i].width) % 8;
    }
    /* Determine the pixels that can be rendered as a whole. */
    for (i = 0; i < (int)nums; i++) {
        rect[i].width = (rect[i].x + rect[i].width) / 8;
        rect[i].x /= 8;
        rect[i].width -= rect[i].x;
    }
    /* Use the gpu to render areas that can be drawn as a whole. */
    for (i = 0; i < (int)nums; i++) {
        vg_lite_translate((vg_lite_float_t)rect[i].x, (vg_lite_float_t)rect[i].y, &matrix);
        VG_LITE_RETURN_ERROR(vg_lite_blit_rect(&scissor_dst_layer, &scissor_src_layer, &rect[i], &matrix, VG_LITE_BLEND_NONE, 0, filter));

    }
    vg_lite_finish();
    /* Render edge pixels using cpu. */
    for (i = 0; i < (int)nums; i++) {
        for (j = rect[i].y; j < (int)(rect[i].y + rect[i].height); j++) {
            memset((uint8_t*)((uint8_t*)scissor_dst_layer.memory + rect[i].x + j * scissor_src_layer.width), 0xFF << temp[i * 2], 1);
            memset((uint8_t*)((uint8_t*)scissor_dst_layer.memory + rect[i].x + rect[i].width + j * scissor_src_layer.width), 0xFF >> (8 - temp[i * 2 + 1]), 1);
        }
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A16, scissor_dst_layer.address));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A17, scissor_dst_layer.stride));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00000100));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_mirror(vg_lite_orientation_t orientation)
{
#if gcFEATURE_VG_MIRROR
    vg_lite_error_t error = VG_LITE_SUCCESS;

    s_context.mirror_orient = orientation;
    s_context.mirror_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_gamma(vg_lite_gamma_conversion_t gamma_value)
{
#if gcFEATURE_VG_GAMMA
    vg_lite_error_t error = VG_LITE_SUCCESS;

    s_context.gamma_value = gamma_value << 12;
    s_context.gamma_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_premultiply(vg_lite_uint8_t src_enable, vg_lite_uint8_t dst_enable, vg_lite_uint8_t lerp_enable)
{
#if gcFEATURE_VG_HW_PREMULTIPLY
    vg_lite_error_t error = VG_LITE_SUCCESS;

    /* Enable premultiply Mode. */
    s_context.premultiply_src = src_enable;
    s_context.premultiply_dst = dst_enable;
    s_context.premultiply_lerp = lerp_enable;
    s_context.premultiply_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_enable_color_transform()
{
#if gcFEATURE_VG_COLOR_TRANSFORMATION
    vg_lite_error_t error = VG_LITE_SUCCESS;

    s_context.color_transform = (1 << 16);

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_disable_color_transform()
{
#if gcFEATURE_VG_COLOR_TRANSFORMATION
    vg_lite_error_t error = VG_LITE_SUCCESS;

    s_context.color_transform = 0;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_set_color_transform(vg_lite_color_transfrom_t* values)
{
#if gcFEATURE_VG_COLOR_TRANSFORMATION
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_float_t* color_transformations = (vg_lite_float_t*)values;
    int color_elements = 0;
    short temp_transform[8] = { 0 };

    for (color_elements = 0; color_elements < 8; color_elements++) {
        if (color_transformations[color_elements] > 127.0f || color_transformations[color_elements] < -128.0f) {
            return VG_LITE_INVALID_ARGUMENT;
        }
        temp_transform[color_elements] = (short)(color_transformations[color_elements] * 256);
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0C, temp_transform[0] | temp_transform[1] << 16));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0D, temp_transform[2] | temp_transform[3] << 16));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0E, temp_transform[4] | temp_transform[5] << 16));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0F, temp_transform[6] | temp_transform[7] << 16));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

/****************** FLEXA Support ***************/

vg_lite_error_t vg_lite_flexa_enable()
{
#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_info_t data;
    uint32_t reset_bit;
    vg_lite_kernel_flexa_info_t flexa_data;

    flexa_data.sbi_mode = s_context.sbi_mode = 0x1;
    flexa_data.sync_mode = s_context.sync_mode = BIT(14);
    flexa_data.flexa_mode = s_context.flexa_mode = 0x1;
    flexa_data.segment_address = s_context.segment_address;
    flexa_data.segment_count = s_context.segment_count;
    flexa_data.segment_size = s_context.segment_size;
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.start_flag = s_context.start_flag = BIT(9);
    flexa_data.stop_flag = s_context.stop_flag = BIT(11);
    flexa_data.reset_flag = s_context.reset_flag = BIT(10);
    /* set sync mode */
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_ENABLE, &flexa_data));

    /* check if reset is complete */
    data.addr = 0x03600;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_CHECK, &data));
    reset_bit = data.reg;

    if (reset_bit == 1) {
        error = VG_LITE_TIMEOUT;
        return error;
    }
    s_context.flexa_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_set_stream(vg_lite_uint8_t stream_id)
{
#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;

    s_context.stream_id = stream_id << 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_bg_buffer(vg_lite_uint8_t stream_id, vg_lite_buffer_t* buffer, vg_lite_uint32_t background_segment_count, vg_lite_uint32_t background_segment_size)
{
#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;

    flexa_data.sbi_mode = s_context.sbi_mode;
    flexa_data.segment_address = s_context.segment_address = buffer->address;
    flexa_data.segment_count = s_context.segment_count = background_segment_count;
    flexa_data.segment_size = s_context.segment_size = background_segment_size;
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.start_flag = s_context.start_flag;
    flexa_data.stop_flag = s_context.stop_flag;
    flexa_data.reset_flag = s_context.reset_flag;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_SET_BACKGROUND_ADDRESS, &flexa_data));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_stop_frame()
{
#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;

    flexa_data.stop_flag = s_context.stop_flag = BIT(11);
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.sbi_mode = s_context.sbi_mode;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_STOP_FRAME, &flexa_data));

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

vg_lite_error_t vg_lite_flexa_disable()
{
#if gcFEATURE_VG_FLEXA
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;
    vg_lite_kernel_info_t data;
    uint32_t reset_bit;

    flexa_data.flexa_mode = s_context.flexa_mode = 0x0;
    flexa_data.sync_mode = s_context.sync_mode = BIT(2);
    flexa_data.stream_id = s_context.stream_id = 0;
    flexa_data.sbi_mode = s_context.sbi_mode = 0x0;
    flexa_data.start_flag = s_context.start_flag = 0x0;
    flexa_data.stop_flag = s_context.stop_flag = 0x0;
    flexa_data.reset_flag = s_context.reset_flag = 0x0;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_DISABLE, &flexa_data));

    /* check if reset is complete */
    data.addr = 0x03600;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_CHECK, &data));
    reset_bit = data.reg;
    if (reset_bit == 1) {
        error = VG_LITE_TIMEOUT;
        return error;
    }
    s_context.flexa_dirty = 1;

    return error;
#else
    return VG_LITE_NOT_SUPPORT;
#endif
}

/****************** FAST_CLEAR implementation. ***************/
#if gcFEATURE_VG_IM_FASTCLEAR
static vg_lite_error_t _free_fc_buffer(vg_lite_fc_buffer_t* buffer)
{
    vg_lite_error_t error;
    vg_lite_kernel_free_t free;

    if (buffer == NULL)
        return VG_LITE_INVALID_ARGUMENT;

    /* Make sure we have a valid memory handle. */
    if (buffer->handle) {
        /* Free the buffer. */
        free.memory_handle = buffer->handle;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free));

        /* Mark the buffer as freed. */
        buffer->handle = NULL;
        buffer->memory = NULL;
    }

    return VG_LITE_SUCCESS;
}
static vg_lite_error_t convert_color(vg_lite_buffer_format_t format, uint32_t value, uint32_t* result, int* bpp)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t r, g, b, a;
    int Bpp = 0;

    r = B(value);
    g = G(value);
    b = R(value);
    a = A(value);

    do {
        switch (format) {
        case VG_LITE_RGBA8888:
            *result = ARGB(a, b, g, r);
            Bpp = 32;
            break;

        case VG_LITE_BGRA8888:
            *result = ARGB(a, r, g, b);
            Bpp = 32;
            break;

        case VG_LITE_RGBX8888:
            *result = ARGB(0xff, b, g, r);
            Bpp = 32;
            break;

        case VG_LITE_BGRX8888:
            *result = ARGB(0xff, r, g, b);
            Bpp = 32;
            break;


        case VG_LITE_RGBA4444:
            *result = ARGB4(a, b, g, r);
            Bpp = 16;
            break;

        case VG_LITE_BGRA4444:
            *result = ARGB4(a, r, g, b);
            Bpp = 16;
            break;

        case VG_LITE_RGB565:
            *result = ((b & 0xf8) << 8) |
                ((g & 0xfc) << 3) |
                ((r & 0xf8) >> 3);
            Bpp = 16;
            break;

        case VG_LITE_BGR565:
            *result = ((r & 0xf8) << 8) |
                ((g & 0xfc) << 3) |
                ((b & 0xf8) >> 3);
            Bpp = 16;
            break;

        case VG_LITE_BGRA5551:
            *result = ((b & 0xf8) << 8) |
                ((g & 0xf8) << 3) |
                ((r & 0xf8) >> 2) |
                ((a & 0x80) >> 7);
            Bpp = 16;
            break;

        case VG_LITE_A8:
            *result = ARGB(a, a, a, a);
            Bpp = 8;
            break;

        case VG_LITE_L8:
            *result = ARGB(r, r, r, r);
            Bpp = 8;
            break;

        default:
            error = VG_LITE_NOT_SUPPORT;
            break;
        }
    } while (0);

    if (bpp != NULL) {
        *bpp = Bpp;
    }

    if (Bpp == 16) {
        *result = ((*result) << 16) | (*result);
    }
    return error;
}

/* Fill Target buffer by FC buffer. Only used in cmodel/fpga for verification. */
#if defined(DEBUG) || defined(_DEBUG)
static vg_lite_error_t fill_fc_target(vg_lite_buffer_t* target, vg_lite_buffer_t* fcb)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint8_t* fc = (uint8_t*)fcb->memory;
    uint16_t* target16;
    uint32_t* target32;
    uint8_t* target8;
    uint32_t  clear32;
    int byte_done = 0;
    int i, j, k;
    int bpp;

    do {
        convert_color(target->format, target->fc_buffer[0].color, &clear32, &bpp);

        if (bpp == 32) {
            target32 = (uint32_t*)target->memory;
            for (i = 0; i < fcb->width; i++) {

                for (j = 0; j < 8; j++) {   /* Loop the bits*/

                    if (!(((*fc) >> j) & 1)) {
                        for (k = 0; k < 64 / 4; k++) {
                            target32[k] = clear32;
                            byte_done += 4;
                            if (byte_done >= target->stride * target->height) {
                                return error;
                            }
                        }
                    }

                    target32 += 64 / 4;
                }

                fc++;
            }
        }
        else if (bpp == 16) {
            target16 = (uint16_t*)target->memory;
            for (i = 0; i < fcb->width; i++) {

                for (j = 0; j < 8; j++) {   /* Loop the bits*/

                    if (!(((*fc) >> j) & 1)) {
                        for (k = 0; k < 64 / 2; k++) {
                            target16[k] = (uint16_t)clear32;
                            byte_done += 2;
                            if (byte_done >= target->stride * target->height) {
                                return error;
                            }
                        }
                    }

                    target16 += 64 / 2;
                }

                fc++;
            }
        }
        else if (bpp == 8) {
            target8 = (uint8_t*)target->memory;
            for (i = 0; i < fcb->width; i++) {

                for (j = 0; j < 8; j++) {   /* Loop the bits*/

                    if (!(((*fc) >> j) & 1)) {
                        for (k = 0; k < 64; k++) {
                            target8[k] = (uint8_t)clear32;
                            byte_done++;
                            if (byte_done >= target->stride * target->height) {
                                return error;
                            }
                        }
                    }

                    target8 += 64;
                }

                fc++;
            }
        }
    } while (0);

    return error;
}
#endif

/* Update the fast_clear buffer when render target switched. */
static vg_lite_error_t update_fc_buffer(vg_lite_buffer_t* target)
{
    int rt_bytes;
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_context_t* context = GET_CONTEXT();
    vg_lite_kernel_allocate_t allocate;

    do {
        if (target == NULL) {
            error = VG_LITE_INVALID_ARGUMENT;
            break;
        }

        rt_bytes = target->stride * target->height;
        rt_bytes = VG_LITE_ALIGN(rt_bytes, (FC_BIT_TO_BYTES * 2));
        rt_bytes = rt_bytes / FC_BIT_TO_BYTES / 2;
        /* Only allocate new buffer when the allocated is not big enough. Yes*/
        if (rt_bytes > target->fc_buffer[0].stride) {
            _free_fc_buffer(&target->fc_buffer[0]);

            target->fc_buffer[0].width = rt_bytes;         /* The actually used bytes. */
            rt_bytes = VG_LITE_ALIGN(rt_bytes, FC_BURST_BYTES);     /* The allocated aligned bytes. */
            target->fc_buffer[0].stride = rt_bytes;
            target->fc_buffer[0].height = 1;
            allocate.bytes = rt_bytes;
            allocate.contiguous = 1;

            VG_LITE_BREAK_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &allocate));
            target->fc_buffer[0].handle = allocate.memory_handle;
            target->fc_buffer[0].memory = allocate.memory;
            target->fc_buffer[0].address = allocate.memory_gpu;
        }
        else {
            /* Just update the fc buffer size. */
            target->fc_buffer[0].width = rt_bytes;
        }
        memset(target->fc_buffer[0].memory, 0xff, target->fc_buffer[0].stride);
        VG_LITE_RETURN_ERROR(push_state(context, 0x0A9A, target->fc_buffer[0].address));   /* FC buffer address. */
    } while (0);

    return error;
}

/* Update FC registers and clear FC buffer. */
static vg_lite_error_t clear_fc(vg_lite_fc_buffer_t* buffer, uint32_t value)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_context_t* context = GET_CONTEXT();
    uint32_t bytes_to_clear;

    if (buffer == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    bytes_to_clear = buffer->stride / FC_BURST_BYTES;
    buffer->color = value;

    do {
        VG_LITE_BREAK_ERROR(push_state(context, 0x0A9B, value));                       /* FC clear value. */
        VG_LITE_BREAK_ERROR(push_state(context, 0x0AB0, 0x80000000 | bytes_to_clear));   /* FC clear command. */
    } while (0);

    return error;
}

#if VG_TARGET_FC_DUMP
static int fc_buf_dump(vg_lite_buffer_t* target, vg_lite_buffer_t* fcb)
{
    int error = VG_LITE_SUCCESS;
    uint8_t* fc = (uint8_t*)fcb->memory;
    uint8_t* target8;
    int byte_done = 0;
    int target_bytes;
    int i, j;

    static unsigned s_cnt;
    unsigned cnt = s_cnt;

    FILE* fpFCBuf;
    FILE* fpTargetBuf;
    FILE* fpTargetBufInfo;
    char buf[256];

    s_cnt++;

    sprintf(buf, "vg255v2.fc_buf.f%04d.txt", cnt);
    fpFCBuf = fopen(buf, "wt");
    if (NULL == fpFCBuf) {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        return -1;
    }

    sprintf(buf, "vg255v2.target_buf_info.f%04d.txt", cnt);
    fpTargetBufInfo = fopen(buf, "wt");
    if (NULL == fpTargetBufInfo) {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        fclose(fpFCBuf);
        return -1;
    }
    else {
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "format", target->format);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "tiled", target->tiled);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "width", target->width);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "height", target->height);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "stride", target->stride);

        fclose(fpTargetBufInfo);
    }

    sprintf(buf, "vg255v2.target_buf.f%04d.txt", cnt);
    fpTargetBuf = fopen(buf, "wt");
    if (NULL == fpTargetBuf) {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        fclose(fpFCBuf);
        return -1;
    }

    /* Dump FC buffer & Dump target buffer */
    target8 = (uint8_t*)target->memory;
    target_bytes = target->stride * target->height;

    for (i = 0; i < fcb->width; ++i)
    {
        fprintf(fpFCBuf, "%02x\n", fc[i]);
        /* 1 byte of fc related with 512 bytes of target buffer */
        for (j = 0; j < 128; ++j) {
            fprintf(fpTargetBuf, "%02x", byte_done < target_bytes ? target8[0] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x", byte_done < target_bytes ? target8[1] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x", byte_done < target_bytes ? target8[2] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x\n", byte_done < target_bytes ? target8[3] : 0);
            byte_done++;

            target8 += 4;
        }
    }

    fclose(fpFCBuf);
    fclose(fpTargetBuf);

    return error;
}
#endif /* VG_TARGET_FC_DUMP */
#endif /* gcFEATURE_VG_IM_FASTCLEAR */
