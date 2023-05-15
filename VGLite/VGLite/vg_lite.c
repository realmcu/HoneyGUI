/****************************************************************************
*
*    Copyright 2012 - 2020 Vivante Corporation, Santa Clara, California.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "vg_lite.h"
#include "vg_lite_kernel.h"


/* This is the function to call from the VGLite driver to interface with the GPU. */
vg_lite_error_t vg_lite_kernel(vg_lite_kernel_command_t command, void *data);


/*** Command buffer dump ***/
#ifndef DUMP_COMMAND
#define DUMP_COMMAND 0
#endif

#ifndef DUMP_IMAGE
#define DUMP_IMAGE 0

#else
#ifndef DUMP_COMMAND
#define DUMP_COMMAND 0
#endif

#ifndef DUMP_IMAGE
#define DUMP_IMAGE 0
#endif
#endif

#define VG_TARGET_FC_DUMP 0

/* Fast clear is not used by default,if SOC should use this ,set this macro to 1. */
#ifndef VG_TARGET_FAST_CLEAR
#define VG_TARGET_FAST_CLEAR 0
#endif /* VG_TARGET_FAST_CLEAR */

#if DUMP_COMMAND || DUMP_IMAGE
#ifdef __linux__
#include <unistd.h>
#endif
#endif

/*** Command buffer configurations ***/

/*** Global Context Access ***/
#define GET_CONTEXT() &s_context

/*** Command buffer configurations, double buffer support ***/
#define VG_LITE_COMMAND_BUFFER_SIZE (64 << 10)
#define CMDBUF_BUFFER(context)  (context).command_buffer[(context).command_buffer_current]
#define CMDBUF_INDEX(context)   (context).command_buffer_current
#define CMDBUF_SIZE(context)    (context).command_buffer_size
#define CMDBUF_OFFSET(context)  (context).command_offset[(context).command_buffer_current]
#define CMDBUF_SWAP(context)    (context).command_buffer_current = \
                                                                   ((context).command_buffer_current + 1) % CMDBUF_COUNT

#define VG_LITE_RETURN_ERROR(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
        return error

#define VG_LITE_BREAK_ERROR(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
        break

#define VG_LITE_ERROR_HANDLER(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
        goto ErrorHandler

/*** Command macros ***/

#define VG_LITE_END(interrupt)  (0x00000000 | interrupt)
#define VG_LITE_SEMAPHORE(id)   (0x10000000 | id)
#define VG_LITE_STALL(id)       (0x20000000 | id)
#define VG_LITE_STATE(address)  (0x30010000 | address)
#define VG_LITE_STATES(count, address)  (0x30000000 | ((count) << 16) | address)
#define VG_LITE_DATA(count)     (0x40000000 | count)
#define VG_LITE_CALL(count)     (0x60000000 | count)
#define VG_LITE_RETURN()        (0x70000000)
#define VG_LITE_NOP()           (0x80000000)

/*** Shortcuts. ***/
#define A(color) (color) >> 24
#define R(color) ((color) & 0x00ff0000) >> 16
#define G(color) ((color) & 0x0000ff00) >> 8
#define B(color) ((color) & 0xff)
#define ARGB(a, r, g, b) ((a) << 24) | ((r) << 16) | ((g) << 8 ) | (b)
#define ARGB4(a, r, g, b) (((a) & 0xf0) << 8) | (((r) & 0xf0) << 4) | (((g) & 0xf0)) | ((b) >> 4)

#define FC_BURST_BYTES  64
#define FC_BIT_TO_BYTES 64

#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) > (b) ? (a) : (b)

static uint32_t command_buffer_size = VG_LITE_COMMAND_BUFFER_SIZE;

#define FORMAT_ALIGNMENT(stride,align) \
    { \
        if((stride) % (align) != 0) \
            return VG_LITE_INVALID_ARGUMENT; \
        return VG_LITE_SUCCESS; \
    }

#define DEST_ALIGNMENT_LIMITATION 64  /* To match hardware alignment requirement */

#define MATRIX_ROWS    3
#define GET_MATRIX_VALUES(Pointer)    ((float *) (Pointer))
#define MAT(Matrix, Row, Column)    (GET_MATRIX_VALUES(Matrix)[Column * MATRIX_ROWS + Row])

#define COLOR_FROM_RAMP(ColorRamp)    (((vg_lite_float_t *) ColorRamp) + 1)
#define CLAMP(x, min, max)    (((x) < (min)) ? (min) : \
                               ((x) > (max)) ? (max) : (x))
#define LERP(v1, v2, w)    ((v1) * (w) + (v2) * (1.0f - (w)))

#define STATES_COUNT             208
#define MIN_TS_SIZE              8 << 10

#define PI                           3.141592653589793238462643383279502f
#define SINF(x)                      ((vg_lite_float_t) sin(x))
#define COSF(x)                      ((vg_lite_float_t) cos(x))
#define FABSF(x)                     ((vg_lite_float_t) fabs(x))
#define SQRTF(x)                     ((vg_lite_float_t) sqrt(x))
#define CLAMP(x, min, max)           (((x) < (min)) ? (min) : \
                                      ((x) > (max)) ? (max) : (x))
#define ACOSF(x)                     ((vg_lite_float_t) acos(x))
#define FMODF(x, y)                  ((vg_lite_float_t) fmod((x), (y)))
#define CEILF(x)                     ((vg_lite_float_t) ceil(x))
#define FALSE                        0
#define TURE                         1
#define SIZEOF(a) \
    ( \
      (size_t) (sizeof(a)) \
    )

typedef uintptr_t               UINTPTR_T;

#define PTR2SIZE(p) \
    ( \
      (UINTPTR_T) (p) \
    )

#define GETINCREMENT(pointer, datatype_size) \
    (datatype_size - (PTR2SIZE(pointer) & (datatype_size - 1)))

#define SKIPTODATA(pointer, datatype_size, SIZE) \
    /* Determine the increment value. */ \
    increment = GETINCREMENT(pointer, datatype_size); \
    /* Skip to the data. */ \
    pointer += increment; \
    SIZE -= increment

#define VGSL_GETVALUE(X) \
    X = get_value(data_pointer); \
    data_pointer += data_type_size; \
    size -= data_type_size

#define VGSL_GETCOORDXY(X, Y) \
    VGSL_GETVALUE(X); \
    VGSL_GETVALUE(Y); \
    if (is_relative) { X += ox; Y += oy; }

#define FLOAT_EPSILON               0.001f

#define SWING_NO             0
#define SWING_OUT            1
#define SWING_IN             2

/* Point curve type for generated stroke path. */
#define CURVE_LINE           0
#define CURVE_QUAD_CONTROL   1
#define CURVE_QUAD_ANCHOR    2
#define CURVE_ARC_SCCW       3
#define CURVE_ARC_SCCW_HALF  4

#define FLOAT_PI                    3.141592654f
#define FLOAT_PI_TWO                6.283185307f
#define FLOAT_PI_THREE_QUARTER      2.356194490f
#define FLOAT_PI_HALF               1.570796327f
#define FLOAT_PI_QUARTER            0.7853981634f
#define FLOAT_PI_EIGHTH             0.3926990817f
/* cos(PI/8) */
#define FLOAT_COS_PI_EIGHTH         0.9238795325f

#define FLOAT_DIFF_EPSILON              0.125f
#define FLOAT_SWING_CENTER_RANGE        0.125f
#define FLOAT_ANGLE_EPSILON             0.0045f
#define FLOAT_ANGLE_EPSILON_COS         0.99999f
#define FLOAT_MIN_ARC_ANGLE             0.044f
#define FLOAT_MIN_ARC_ANGLE_COS         0.999f
/* Float constants. */
#define gcvMAX_POS_FLOAT        ((vg_lite_float_t)  3.4028235e+038)
#define gcvMAX_NEG_FLOAT        ((vg_lite_float_t) -3.4028235e+038)
#define FLOAT_MIN                   gcvMAX_NEG_FLOAT
#define FLOAT_MAX                   gcvMAX_POS_FLOAT

#define FLOAT_FAT_LINE_WIDTH            2.5f

/* Point flatten type for flattened line segments. */
#define vgcFLATTEN_NO           0
#define vgcFLATTEN_START        1
#define vgcFLATTEN_MIDDLE       2
#define vgcFLATTEN_END          3

/* Command size calculation shortcuts. */
#define COMMANDSIZE(CoordinateCount, CoordinateType) \
    ((1+CoordinateCount) * SIZEOF(CoordinateType))
typedef vg_lite_float_t FLOATVECTOR4[4];

#define ABS(x)               (((x) < 0)    ? -(x) :  (x))
#define EPS                  2.2204460492503131e-14

typedef struct vg_lite_states
{
    uint32_t state;
    uint8_t  init;
} vg_lite_states_t;

typedef struct vg_lite_hardware
{
    vg_lite_states_t hw_states[STATES_COUNT];
} vg_lite_hardware_t;

typedef struct vg_lite_context
{
    vg_lite_kernel_context_t    context;
    vg_lite_hardware_t          hw;
    vg_lite_capabilities_t      capabilities;
    uint8_t                    *command_buffer[CMDBUF_COUNT];
    uint32_t                    command_buffer_size;
    uint32_t                    command_offset[CMDBUF_COUNT];
    uint32_t                    command_buffer_current;
    vg_lite_tsbuffer_info_t     tsbuffer;
    vg_lite_buffer_t
    *rtbuffer;                   /* DDRLess: this is used as composing buffer. */

    uint32_t                    scissor_enabled;
    uint32_t
    scissor_dirty;              /* Indicates whether scissor states are changed or not. e.g., scissors[4] or scissor_enabled. */
    int32_t                     scissor[4];                 /* Scissor area: x, y, right, bottom. */

    uint32_t                    image_global_alpha_mode;
    uint32_t                    image_global_alpha_value;
    uint32_t                    dest_global_alpha_mode;
    uint32_t                    dest_global_alpha_value;

    uint32_t                    sbi_mode;
    uint32_t                    sync_mode;
    uint32_t                    flexa_mode;
    uint32_t                    stream_id;
    uint32_t                    segment_buffer_address;
    uint32_t                    segment_count;
    uint32_t                    segment_size;
    uint32_t                    stop_flag;
    uint32_t                    start_flag;
    uint32_t                    reset_flag;
    uint8_t                     flexa_dirty;
    uint8_t                     custom_cmd_buf;
    uint8_t                     custom_ts_buf;
    uint8_t                     use_dst;
} vg_lite_context_t;

typedef struct vg_lite_feature_database
{
    uint32_t                    chip_id;
    uint32_t                    chip_version;
    uint32_t                    cid;
    uint32_t                    vg_im_index_format: 1;
    uint32_t                    vg_pe_premultiply: 1;
    uint32_t                    vg_scissor: 1;
    uint32_t                    vg_border_culling: 1;
    uint32_t                    vg_rgba2_format: 1;
    uint32_t                    vg_quality_8x: 1;
    uint32_t                    vg_im_fastclear: 1;
    uint32_t                    vg_radial_gradient: 1;
    uint32_t                    vg_global_alpha: 1;
    uint32_t                    vg_rgba8_etc2_eac: 1;
    uint32_t                    vg_color_key: 1;
    uint32_t                    vg_double_image: 1;
    uint32_t                    vg_yuv_output: 1;
    uint32_t                    vg_flexa: 1;
    uint32_t                    vg_24bit: 1;
    uint32_t                    vg_dither: 1;
    uint32_t                    vg_yuv_input: 1;
    uint32_t                    vg_use_dst: 1;
    uint32_t                    vg_pe_clear: 1;
    uint32_t                    vg_im_input: 1;
    uint32_t                    vg_dec_compress: 1;
    uint32_t                    vg_linear_gradient_ext: 1;
} vg_lite_feature_database_t;

static vg_lite_feature_database_t VGFeatureInfos[] =
{
    /* vg255 */
    {
        GPU_CHIP_ID_GCNANOLITEV, /* ChipID */
        0x1311, /* ChipRevision */
        0x404,  /* CID */
        0x0, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg255 */
    {
        GPU_CHIP_ID_GCNANOLITEV, /* ChipID */
        0x1311, /* ChipRevision */
        0x40a,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg255 */
    {
        GPU_CHIP_ID_GCNANOLITEV, /* ChipID */
        0x1311, /* ChipRevision */
        0x40b,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg355 */
    {
        GPU_CHIP_ID_GC355, /* ChipID */
        0x1217, /* ChipRevision */
        0x408,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x0, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x0, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X1, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x1, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg355 */
    {
        GPU_CHIP_ID_GC355, /* ChipID */
        0x1216, /* ChipRevision */
        0x0,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x0, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x0, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X1, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x1, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1000, /* ChipRevision */
        0x40c,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1001, /* ChipRevision */
        0x40c,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1001, /* ChipRevision */
        0x40e,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1001, /* ChipRevision */
        0x411,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1001, /* ChipRevision */
        0x410,  /* CID */
        0x0, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x0, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x1, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x1, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1002, /* ChipRevision */
        0x410,  /* CID */
        0x0, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x0, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x1, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x1, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1003, /* ChipRevision */
        0x412,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x1, /* gcFEATURE_BIT_VG_24BIT */
        0x1, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x1, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x2000, /* ChipRevision */
        0x418,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x1, /* gcFEATURE_BIT_VG_24BIT */
        0x1, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x1, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x0, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x2000, /* ChipRevision */
        0x413,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x1, /* gcFEATURE_BIT_VG_24BIT */
        0x1, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x1, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x2000, /* ChipRevision */
        0x415,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x1, /* gcFEATURE_BIT_VG_24BIT */
        0x1, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x1, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1003, /* ChipRevision */
        0x40f,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x1, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x1, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1003, /* ChipRevision */
        0x410,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x1, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x1, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1003, /* ChipRevision */
        0x40c,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x0, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x0, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x0, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1003, /* ChipRevision */
        0x417,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x0, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x0, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x0, /* gcFEATURE_BIT_VG_24BIT */
        0x0, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x0, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x2000, /* ChipRevision */
        0x419,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x1, /* gcFEATURE_BIT_VG_24BIT */
        0x1, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x1, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x1, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
    /* vg265 */
    {
        GPU_CHIP_ID_GCNANOULTRAV, /* ChipID */
        0x1000, /* ChipRevision */
        0x415,  /* CID */
        0x1, /* gcFEATURE_BIT_VG_IM_INDEX_FORMAT */
        0x1, /* gcFEATURE_BIT_VG_PE_PREMULTIPLY */
        0x1, /* gcFEATURE_BIT_VG_SCISSOR */
        0x1, /* gcFEATURE_BIT_VG_BORDER_CULLING */
        0x1, /* gcFEATURE_BIT_VG_RGBA2_FORMAT */
        0x0, /* gcFEATURE_BIT_VG_QUALITY_8X */
        0x1, /* gcFEATURE_BIT_IM_FASTCLEAR */
        0X0, /* gcFEATURE_BIT_VG_RADIAL_GRADIENT */
        0x1, /* gcFEATURE_BIT_VG_GLOBAL_ALPHA */
        0x1, /* gcFEATURE_BIT_VG_RGBA8_ETC2_EAC */
        0x1, /* gcFEATURE_BIT_VG_COLOR_KEY */
        0x0, /* gcFEATURE_BIT_VG_DOUBLE_IMAGE */
        0x0, /* gcFEATURE_BIT_VG_YUV_OUTPUT */
        0x0, /* gcFEATURE_BIT_VG_FLEXA */
        0x1, /* gcFEATURE_BIT_VG_24BIT */
        0x1, /* gcFEATURE_BIT_VG_DITHER */
        0x0, /* gcFEATURE_BIT_YUV_INPUT */
        0x1, /* gcFEATURE_BIT_USE_DST */
        0x0, /* gcFEATURE_BIT_VG_PE_CLEAR */
        0x1, /* gcFEATURE_BIT_VG_IM_INPUT */
        0x0, /* gcFEATURE_BIT_VG_DEC_COMPRESS */
        0x0, /* gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT */
    },
};

typedef struct vg_lite_ftable
{
    uint32_t    ftable[gcFEATURE_COUNT];
    uint32_t    ftflag;
} vg_lite_ftable_t;

#if DUMP_COMMAND
FILE *fp;
char filename[30];
#endif

vg_lite_context_t s_context = {0};
vg_lite_ftable_t s_ftable = {{0}, 0};

int submit_flag = 0;

typedef struct vg_lite_control_coord
{
    vg_lite_float_t    startX;
    vg_lite_float_t    startY;
    vg_lite_float_t    lastX;
    vg_lite_float_t    lastY;
    vg_lite_float_t    controlX;
    vg_lite_float_t    controlY;
}
vg_lite_control_coord_t;

static uint32_t _commandSize_float[] =
{
    COMMANDSIZE(0, vg_lite_float_t),              /*   0: END             */
    COMMANDSIZE(0, vg_lite_float_t),              /*   1: CLOSE           */
    COMMANDSIZE(2, vg_lite_float_t),              /*   2: MOVE            */
    COMMANDSIZE(2, vg_lite_float_t),              /*   3: MOVE_REL        */
    COMMANDSIZE(2, vg_lite_float_t),              /*   4: LINE            */
    COMMANDSIZE(2, vg_lite_float_t),              /*   5: LINE_REL        */
    COMMANDSIZE(4, vg_lite_float_t),              /*   6: QUAD            */
    COMMANDSIZE(4, vg_lite_float_t),              /*   7: QUAD_REL        */
    COMMANDSIZE(6, vg_lite_float_t),              /*   8: CUBIC           */
    COMMANDSIZE(6, vg_lite_float_t),              /*   9: CUBIC_REL     */
    COMMANDSIZE(5, vg_lite_float_t),              /*   10: SCCWARC         */
    COMMANDSIZE(5, vg_lite_float_t),              /*   11: SCCWARC_REL     */
    COMMANDSIZE(5, vg_lite_float_t),              /*   12: SCWARC         */
    COMMANDSIZE(5, vg_lite_float_t),              /*   13: SCWARC_REL     */
    COMMANDSIZE(5, vg_lite_float_t),              /*   14: LCCWARC        */
    COMMANDSIZE(5, vg_lite_float_t),              /*   15: LCCWARC_REL    */
    COMMANDSIZE(5, vg_lite_float_t),              /*   16: LCWARC         */
    COMMANDSIZE(5, vg_lite_float_t),              /*   17: LCWARC_REL     */
};

static vg_lite_float_t _GetS8_NS_NB(int8_t *Data)
{
    int8_t x0 = *((int8_t *) Data);
    vg_lite_float_t x = (vg_lite_float_t) x0;

    return x;
}

static vg_lite_float_t _GetS16_NS_NB(int8_t *Data)
{
    int16_t x0 = *((int16_t *) Data);
    vg_lite_float_t x = (vg_lite_float_t) x0;

    return x;
}

static vg_lite_float_t _GetS32_NS_NB(int8_t *Data)
{
    int32_t x0 = *((int32_t *) Data);
    vg_lite_float_t x = (vg_lite_float_t) x0;

    return x;
}

static vg_lite_float_t _GetF_NS_NB(int8_t *Data)
{
    vg_lite_float_t x = *((vg_lite_float_t *) Data);

    return x;
}

typedef vg_lite_float_t (* vg_value_getter)(int8_t *Data);

/* Special sqrt(1.0f + x) for quick calculation when 0 <= x <= 1. */
static vg_lite_float_t _Sqrt(
    vg_lite_float_t X
)
{
    vg_lite_float_t x = X;
    vg_lite_float_t s = 1.0f;

    s += x * 0.5f;
    x *= X;
    s -= x * 0.12445995211601257f;
    x *= X;
    s += x * 0.058032196015119553f;
    x *= X;
    s -= x * 0.025314478203654289f;
    x *= X;
    s += x * 0.0059584137052297592f;

    return s;
}

static vg_lite_error_t _set_point_tangent(
    vg_lite_path_point_ptr Point,
    vg_lite_float_t Dx,
    vg_lite_float_t Dy
)
{
    if (!Point)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    if (Dx == 0.0f)
    {
        if (Dy == 0.0f)
        {
            if (Point->prev)
            {
                Point->length = 0.0f;
                Point->tangentX = Point->prev->tangentX;
                Point->tangentY = Point->prev->tangentY;
            }
            else
            {
                Point->length = 0.0f;
                Point->tangentX = 0.0f;
                Point->tangentY = 0.0f;
            }
        }
        else
        {
            Point->tangentX = 0.0f;
            if (Dy > 0.0f)
            {
                Point->length = Dy;
                Point->tangentY = 1.0f;
            }
            else
            {
                Point->length = -Dy;
                Point->tangentY = -1.0f;
            }
        }
    }
    else if (Dy == 0.0f)
    {
        Point->tangentY = 0.0f;
        if (Dx > 0.0f)
        {
            Point->length = Dx;
            Point->tangentX = 1.0f;
        }
        else
        {
            Point->length = -Dx;
            Point->tangentX = -1.0f;
        }
    }
    else
    {
        vg_lite_float_t l, tx, ty;

        vg_lite_float_t dx, dy;
        vg_lite_float_t t, t2;

        dx = (Dx >= 0.0f ? Dx : -Dx);
        dy = (Dy >= 0.0f ? Dy : -Dy);
        if (dx >= dy)
        {
            t = dy / dx;
            t2 = t * t;
            l = _Sqrt(t2);
            Point->length = l * dx;

            tx = 1.0f / l;
            ty = tx * t;
        }
        else
        {
            t = dx / dy;
            t2 = t * t;
            l = _Sqrt(t2);
            Point->length = l * dy;

            ty = 1.0f / l;
            tx = ty * t;
        }
        if (Dx < 0.0f) { tx = -tx; }
        if (Dy < 0.0f) { ty = -ty; }

        tx = CLAMP(tx, -1.0f, 1.0f);
        ty = CLAMP(ty, -1.0f, 1.0f);
        Point->tangentX = tx;
        Point->tangentY = ty;
    }
    return VG_LITE_SUCCESS;
}

static vg_lite_error_t _add_point_to_point_list_wdelta(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t DX,
    vg_lite_float_t DY,
    uint8_t flatten_flag
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr last_point;
    vg_lite_path_point_ptr point;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    last_point = stroke_conversion->path_last_point;
    point = (vg_lite_path_point_ptr)malloc(sizeof(*point));
    if (!point)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    memset(point, 0, sizeof(*point));

    point->x = X;
    point->y = Y;
    point->flatten_flag = flatten_flag;

    /* Calculate tangent for last_point. */
    VG_LITE_ERROR_HANDLER(_set_point_tangent(last_point, DX, DY));

    last_point->next = point;
    stroke_conversion->path_last_point = point;
    point->prev = last_point;
    stroke_conversion->point_count++;

    return error;
ErrorHandler:

    free(point);
    point = NULL;
    return error;
}

static vg_lite_error_t _add_point_to_point_list(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    uint8_t flatten_flag
)
{
    vg_lite_error_t status = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr last_point;
    vg_lite_path_point_ptr point;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    last_point = stroke_conversion->path_last_point;
    if (last_point == NULL)
    {
        point = (vg_lite_path_point_ptr)malloc(sizeof(*point));
        if (!point)
        {
            return VG_LITE_OUT_OF_RESOURCES;
        }
        memset(point, 0, sizeof(*point));

        point->x = X;
        point->y = Y;
        point->flatten_flag = flatten_flag;
        point->prev = NULL;
        stroke_conversion->path_last_point = stroke_conversion->path_point_list = point;
        stroke_conversion->point_count++;
        status = VG_LITE_SUCCESS;
    }
    else
    {
        vg_lite_float_t dX = X - last_point->x;
        vg_lite_float_t dY = Y - last_point->y;
        vg_lite_float_t deltaX = (dX >= 0.0f ? dX : -dX);
        vg_lite_float_t deltaY = (dY >= 0.0f ? dY : -dY);

        /* Check for degenerated line. */
        if (deltaX == 0.0f && deltaY == 0.0f)
        {
            /* Skip degenerated line. */
            status = VG_LITE_SUCCESS;
            goto ErrorHandler;
        }
        if (deltaX < FLOAT_EPSILON && deltaY < FLOAT_EPSILON)
        {
            vg_lite_float_t ratioX, ratioY;

            if (deltaX == 0.0f)
            {
                ratioX = 0.0f;
            }
            else if (X == 0.0f)
            {
                ratioX = deltaX;
            }
            else
            {
                ratioX = deltaX / X;
                if (ratioX < 0.0f) { ratioX = -ratioX; }
            }
            if (deltaY == 0.0f)
            {
                ratioY = 0.0f;
            }
            else if (Y == 0.0f)
            {
                ratioY = deltaY;
            }
            else
            {
                ratioY = deltaY / Y;
                if (ratioY < 0.0f) { ratioY = -ratioY; }
            }
            if (ratioX < 1.0e-6f && ratioY < 1.0e-6f)
            {
                /* Skip degenerated line. */
                status = VG_LITE_SUCCESS;
                goto ErrorHandler;
            }
        }

        status = _add_point_to_point_list_wdelta(stroke_conversion, X, Y, dX, dY, flatten_flag);
    }

ErrorHandler:
    return status;
}

static vg_lite_error_t
_flatten_quad_bezier(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_float_t X0,
    vg_lite_float_t Y0,
    vg_lite_float_t X1,
    vg_lite_float_t Y1,
    vg_lite_float_t X2,
    vg_lite_float_t Y2
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t n;
    vg_lite_path_point_ptr point0, point1;
    vg_lite_float_t x, y;
    vg_lite_float_t a1x, a1y, a2x, a2y;
    vg_lite_float_t f1, f2, t1, t2, upper_bound;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Formula.
    * f(t) = (1 - t)^2 * p0 + 2 * t * (1 - t) * p1 + t^2 * p2
    *      = a0 + a1 * t + a2 * t^2
    *   a0 = p0
    *   a1 = 2 * (-p0 + p1)
    *   a2 = p0 - 2 * p1 + p2
    */
    x = X1 - X0;
    a1x = x + x;
    y = Y1 - Y0;
    a1y = y + y;
    a2x = X0 - X1 - X1 + X2;
    a2y = Y0 - Y1 - Y1 + Y2;

    /* Step 1: Calculate N. */
    /* Lefan's method. */
    /* dist(t) = ...
    * t2 = ...
    * if 0 <= t2 <= 1
    *    upper_bound = dist(t2)
    * else
    *    upper_bound = max(dist(0), dist(1))
    * N = ceil(sqrt(upper_bound / epsilon / 8))
    */
    /* Prepare dist(t). */
    f1 = a1x * a2y - a2x * a1y;
    if (f1 != 0.0f)
    {
        if (f1 < 0.0f) { f1 = -f1; }

        /* Calculate t2. */
        t1 = a2x * a2x + a2y * a2y;
        t2 = -(x * a2x + y * a2y) / t1;
        /* Calculate upper_bound. */
        if (t2 >= 0.0f && t2 <= 1.0f)
        {
            f2 = x + a2x * t2;
            f2 *= f2;
            t1 = y + a2y * t2;
            t1 *= t1;
            upper_bound = t1 + f2;
        }
        else
        {
            f2 = x + a2x;
            f2 *= f2;
            t1 = y + a2y;
            t1 *= t1;
            t2 = t1 + f2;
            t1 = x * x + y * y;
            upper_bound = t1 < t2 ? t1 : t2;
        }
        /* Calculate n. */
        upper_bound = f1 / SQRTF(upper_bound);
        upper_bound = SQRTF(upper_bound);
        if (stroke_conversion->is_fat)
        {
            upper_bound *= stroke_conversion->stroke_line_width;
        }
        n = (uint32_t) ceil(upper_bound);
    }
    else
    {
        /* n = 0 => n = 256. */
        n = 256;
    }

    if (n == 0 || n > 256)
    {
        n = 256;
    }

    /* Add extra P0 for incoming tangent. */
    point0 = stroke_conversion->path_last_point;
    /* First add P1 to calculate incoming tangent, which is saved in P0. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X1, Y1, vgcFLATTEN_START));

    point1 = stroke_conversion->path_last_point;
    /* Change the point1's coordinates back to P0. */
    point1->x = X0;
    point1->y = Y0;
    point0->length = 0.0f;

    if (n > 1)
    {
        vg_lite_float_t d, dsquare, dx, dy, ddx, ddy;
        vg_lite_float_t ratioX, ratioY;
        uint32_t i;

        /* Step 2: Calculate deltas. */
        /*   Df(t) = f(t + d) - f(t)
        *         = a1 * d + a2 * d^2 + 2 * a2 * d * t
        *  DDf(t) = Df(t + d) - Df(t)
        *         = 2 * a2 * d^2
        *    f(0) = a0
        *   Df(0) = a1 * d + a2 * d^2
        *  DDf(0) = 2 * a2 * d^2
        */
        d = 1.0f / (vg_lite_float_t) n;
        dsquare = d * d;
        ddx = a2x * dsquare;
        ddy = a2y * dsquare;
        dx  = a1x * d + ddx;
        dy  = a1y * d + ddy;
        ddx += ddx;
        ddy += ddy;

        /* Step 3: Add points. */
        ratioX = dx / X0;
        if (ratioX < 0.0f) { ratioX = -ratioX; }
        ratioY = dy / Y0;
        if (ratioY < 0.0f) { ratioY = -ratioY; }
        if (ratioX > 1.0e-6f && ratioY > 1.0e-6f)
        {
            x = X0;
            y = Y0;
            for (i = 1; i < n; i++)
            {
                x += dx;
                y += dy;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list_wdelta(stroke_conversion, x, y, dx, dy,
                                                                      vgcFLATTEN_MIDDLE));

                dx += ddx;
                dy += ddy;
            }

        }
        else
        {
            for (i = 1; i < n; i++)
            {
                vg_lite_float_t t = (vg_lite_float_t) i / (vg_lite_float_t) n;
                vg_lite_float_t u = 1.0f - t;
                vg_lite_float_t a0 = u * u;
                vg_lite_float_t a1 = 2.0f * t * u;
                vg_lite_float_t a2 = t * t;

                x  = a0 * X0 + a1 * X1 + a2 * X2;
                y  = a0 * Y0 + a1 * Y1 + a2 * Y2;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x, y, vgcFLATTEN_MIDDLE));
            }
        }
    }


    /* Add point 2 separately to avoid cumulative errors. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X2, Y2, vgcFLATTEN_END));

    /* Add extra P2 for outgoing tangent. */
    /* First change P2(point0)'s coordinates to P1. */
    point0 = stroke_conversion->path_last_point;
    point0->x = X1;
    point0->y = Y1;

    /* Add P2 to calculate outgoing tangent. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X2, Y2, vgcFLATTEN_NO));

    point1 = stroke_conversion->path_last_point;

    /* Change point0's coordinates back to P2. */
    point0->x = X2;
    point0->y = Y2;
    point0->length = 0.0f;

ErrorHandler:
    return error;
}

static vg_lite_error_t
_flatten_cubic_bezier(
    vg_lite_stroke_conversion_t   *stroke_conversion,
    vg_lite_float_t X0,
    vg_lite_float_t Y0,
    vg_lite_float_t X1,
    vg_lite_float_t Y1,
    vg_lite_float_t X2,
    vg_lite_float_t Y2,
    vg_lite_float_t X3,
    vg_lite_float_t Y3
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t n;
    vg_lite_path_point_ptr point0, point1;
    vg_lite_float_t x, y;
    vg_lite_float_t a1x, a1y, a2x, a2y, a3x, a3y;
    vg_lite_float_t ddf0, ddf1, t1, t2, upper_bound;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Formula.
    * f(t) = (1 - t)^3 * p0 + 3 * t * (1 - t)^2 * p1 + 3 * t^2 * (1 - t) * p2 + t^3 * p3
    *      = a0 + a1 * t + a2 * t^2 + a3 * t^3
    */
    x = X1 - X0;
    a1x = x + x + x;
    y = Y1 - Y0;
    a1y = y + y + y;
    x = X0 - X1 - X1 + X2;
    a2x = x + x + x;
    y = Y0 - Y1 - Y1 + Y2;
    a2y = y + y + y;
    x = X1 - X2;
    a3x = x + x + x + X3 - X0;
    y = Y1 - Y2;
    a3y = y + y + y + Y3 - Y0;

    /* Step 1: Calculate N. */
    /* Lefan's method. */
    /*  df(t)/dt  = a1 + 2 * a2 * t + 3 * a3 * t^2
    * d2f(t)/dt2 = 2 * a2 + 6 * a3 * t
    * N = ceil(sqrt(max(ddfx(0)^2 + ddfy(0)^2, ddfx(1)^2 + ddyf(1)^2) / epsilon / 8))
    */

    ddf0 = a2x * a2x + a2y * a2y;
    t1 = a2x + a3x + a3x + a3x;
    t2 = a2y + a3y + a3y + a3y;
    ddf1 = t1 * t1 + t2 * t2;
    upper_bound = ddf0 > ddf1 ? ddf0 : ddf1;
    upper_bound = SQRTF(upper_bound);
    upper_bound += upper_bound;
    upper_bound = SQRTF(upper_bound);
    if (stroke_conversion->is_fat)
    {
        upper_bound *= stroke_conversion->stroke_line_width;
    }
    n = (uint32_t) ceil(upper_bound);

    if (n == 0 || n > 256)
    {
        n = 256;
    }

    /* Add extra P0 for incoming tangent. */
    point0 = stroke_conversion->path_last_point;
    /* First add P1/P2/P3 to calculate incoming tangent, which is saved in P0. */
    if (X0 != X1 || Y0 != Y1)
    {
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X1, Y1, vgcFLATTEN_START));
    }
    else if (X0 != X2 || Y0 != Y2)
    {
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X2, Y2, vgcFLATTEN_START));
    }
    else
    {
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X3, Y3, vgcFLATTEN_START));
    }
    point1 = stroke_conversion->path_last_point;
    /* Change the point1's coordinates back to P0. */
    point1->x = X0;
    point1->y = Y0;
    point0->length = 0.0f;

    if (n > 1)
    {
        vg_lite_float_t d, dsquare, dcube, dx, dy, ddx, ddy, dddx, dddy;
        vg_lite_float_t ratioX, ratioY;
        uint32_t i;

        /* Step 2: Calculate deltas */
        /*   Df(t) = f(t + d) - f(t)
        *  DDf(t) = Df(t + d) - Df(t)
        * DDDf(t) = DDf(t + d) - DDf(t)
        *    f(0) = a0
        *   Df(0) = a1 * d + a2 * d^2 + a3 * d^3
        *  DDf(0) = 2 * a2 * d^2 + 6 * a3 * d^3
        * DDDf(0) = 6 * a3 * d^3
        */
        d = 1.0f / (vg_lite_float_t) n;
        dsquare   = d * d;
        dcube     = dsquare * d;
        ddx  = a2x * dsquare;
        ddy  = a2y * dsquare;
        dddx = a3x * dcube;
        dddy = a3y * dcube;
        dx   = a1x * d + ddx + dddx;
        dy   = a1y * d + ddy + dddy;
        ddx  += ddx;
        ddy  += ddy;
        dddx *= 6.0f;
        dddy *= 6.0f;
        ddx  += dddx;
        ddy  += dddy;

        /* Step 3: Add points. */
        ratioX = dx / X0;
        if (ratioX < 0.0f) { ratioX = -ratioX; }
        ratioY = dy / Y0;
        if (ratioY < 0.0f) { ratioY = -ratioY; }
        if (ratioX > 1.0e-6f && ratioY > 1.0e-6f)
        {
            x = X0;
            y = Y0;
            for (i = 1; i < n; i++)
            {
                x += dx;
                y += dy;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list_wdelta(stroke_conversion, x, y, dx, dy,
                                                                      vgcFLATTEN_MIDDLE));
                dx += ddx; ddx += dddx;
                dy += ddy; ddy += dddy;
            }
        }
        else
        {
            for (i = 1; i < n; i++)
            {
                vg_lite_float_t t = (vg_lite_float_t) i / (vg_lite_float_t) n;
                vg_lite_float_t tSquare = t * t;
                vg_lite_float_t tCube = tSquare * t;
                vg_lite_float_t a0 =  1.0f -  3.0f * t + 3.0f * tSquare -        tCube;
                vg_lite_float_t a1 =          3.0f * t - 6.0f * tSquare + 3.0f * tCube;
                vg_lite_float_t a2 =                     3.0f * tSquare - 3.0f * tCube;
                vg_lite_float_t a3 =                                             tCube;

                x  = a0 * X0 + a1 * X1 + a2 * X2 + a3 * X3;
                y  = a0 * Y0 + a1 * Y1 + a2 * Y2 + a3 * Y3;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x, y, vgcFLATTEN_MIDDLE));
            }
        }
    }

    /* Add point 3 separately to avoid cumulative errors. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X3, Y3, vgcFLATTEN_END));

    /* Add extra P3 for outgoing tangent. */
    /* First change P3(point0)'s coordinates to P0/P1/P2. */
    point0 = stroke_conversion->path_last_point;
    if (X3 != X2 || Y3 != Y2)
    {
        point0->x = X2;
        point0->y = Y2;
    }
    else if (X3 != X1 || Y3 != Y1)
    {
        point0->x = X1;
        point0->y = Y1;
    }
    else
    {
        point0->x = X0;
        point0->y = Y0;
    }

    /* Add P3 to calculate outgoing tangent. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X3, Y3, vgcFLATTEN_NO));

    point1 = stroke_conversion->path_last_point;

    /* Change point0's coordinates back to P3. */
    point0->x = X3;
    point0->y = Y3;
    point0->length = 0.0f;

ErrorHandler:
    return error;
}

static vg_lite_error_t _flatten_path(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_path_t *path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t increment;
    uint8_t is_relative;
    uint32_t size;
    uint32_t path_command;
    uint32_t prev_command;
    uint8_t data_type_size;
    int8_t *data_pointer = NULL;
    vg_lite_float_t sx, sy;
    vg_lite_float_t ox, oy;
    vg_lite_float_t px, py;
    vg_lite_float_t x0, y0, x1, y1, x2, y2;
    vg_value_getter get_value = NULL;

    if (!stroke_conversion || !path)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    sx = sy = ox = oy = px = py = 0.0f;

    prev_command = VLC_OP_MOVE;

    /* Determine the data size. */
    size = path->path_length;

    /* Determine the beginning of the path data. */
    data_pointer = (int8_t *)path->path;

    /* Select the data picker. */
    switch (path->format)
    {
    case VG_LITE_S8:
        data_type_size = 1;
        get_value = _GetS8_NS_NB;
        break;

    case VG_LITE_S16:
        data_type_size = 2;
        get_value = _GetS16_NS_NB;
        break;

    case VG_LITE_S32:
        data_type_size = 4;
        get_value = _GetS32_NS_NB;
        break;

    case VG_LITE_FP32:
        data_type_size = 4;
        get_value = _GetF_NS_NB;
        break;

    default:
        error = VG_LITE_INVALID_ARGUMENT;
        goto ErrorHandler;
    }
    /* Add an extra gcvVGCMD_MOVE 0.0 0.0 to handle the case the first command is not gcvVGCMD_MOVE. */
    if (*data_pointer != VLC_OP_MOVE)
    {
        /* Add first point to subpath. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, sx, sy, vgcFLATTEN_NO));
    }

    while (size > 0)
    {
        /* Get the command. */
        path_command = *data_pointer & 0x1F;

        /* Assume absolute. */
        is_relative = FALSE;

        switch (path_command)
        {
        case VLC_OP_END:
            /* Skip the command. */
            size -= 1;

            if (prev_command == VLC_OP_END)
            {
                /* Continuous gcvVGCMD_CLOSE - do nothing. */
                break;
            }

            /* Check if subPath is already closed. */
            if (ox != sx || oy != sy)
            {
                /* Add a line from current point to the first point of current subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, sx, sy, vgcFLATTEN_NO));
            }
            if (stroke_conversion->path_point_list != stroke_conversion->path_last_point)
            {
                /* Copy tangent data from first point to last_point. */
                vg_lite_path_point_ptr first_point = stroke_conversion->path_point_list;
                vg_lite_path_point_ptr last_point = stroke_conversion->path_last_point;
                last_point->length = first_point->length;
                last_point->tangentX = first_point->tangentX;
                last_point->tangentY = first_point->tangentY;
            }
            else
            {
                /* Single point path. */
                vg_lite_path_point_ptr point = stroke_conversion->path_point_list;
                point->tangentX = 0.0f;
                point->tangentY = 0.0f;
                point->length = 0.0f;
            }
            stroke_conversion->closed = 1;
            stroke_conversion->path_last_point->next = NULL;
            break;

        case VLC_OP_MOVE_REL:
            is_relative = 1;

        case VLC_OP_MOVE:        /* Indicate the beginning of a new sub-path. */
            /* Skip to the data. */
            SKIPTODATA(data_pointer, data_type_size, size);
            VGSL_GETCOORDXY(x0, y0);

            /* First command is gcvVGCMD_MOVE. */
            /* Add first point to subpath. */
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x0, y0, vgcFLATTEN_NO));

            sx = px = ox = x0;
            sy = py = oy = y0;
            break;

        case VLC_OP_LINE_REL:
            is_relative = 1;

        case VLC_OP_LINE:
            /* Skip to the data. */
            SKIPTODATA(data_pointer, data_type_size, size);
            VGSL_GETCOORDXY(x0, y0);

            /* Add a point to subpath. */
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x0, y0, vgcFLATTEN_NO));

            px = ox = x0;
            py = oy = y0;
            break;

        case VLC_OP_QUAD_REL:
            is_relative = 1;

        case VLC_OP_QUAD:
            /* Skip to the data. */
            SKIPTODATA(data_pointer, data_type_size, size);
            VGSL_GETCOORDXY(x0, y0);
            VGSL_GETCOORDXY(x1, y1);

            if ((ox == x0 && oy == y0) && (ox == x1 && oy == y1))
            {
                /* Degenerated Bezier curve.  Becomes a point. */
                /* Discard zero-length segments. */
            }
            else if ((ox == x0 && oy == y0) || (x0 == x1 && y0 == y1))
            {
                /* Degenerated Bezier curve.  Becomes a line. */
                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x1, y1, vgcFLATTEN_NO));
            }
            else
            {
                VG_LITE_ERROR_HANDLER(_flatten_quad_bezier(stroke_conversion, ox, oy, x0, y0, x1, y1));
            }

            px = x0;
            py = y0;
            ox = x1;
            oy = y1;
            break;

        case VLC_OP_CUBIC_REL:
            is_relative = 1;

        case VLC_OP_CUBIC:
            /* Skip to the data. */
            SKIPTODATA(data_pointer, data_type_size, size);
            VGSL_GETCOORDXY(x0, y0);
            VGSL_GETCOORDXY(x1, y1);
            VGSL_GETCOORDXY(x2, y2);

            if ((ox == x0 && oy == y0) && (ox == x1 && oy == y1) && (ox == x2 && oy == y2))
            {
                /* Degenerated Bezier curve.  Becomes a point. */
                /* Discard zero-length segments. */
            }
            else
            {
                VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier(stroke_conversion, ox, oy, x0, y0, x1, y1, x2, y2));
            }

            px = x1;
            py = y1;
            ox = x2;
            oy = y2;
            break;

        default:
            error = VG_LITE_INVALID_ARGUMENT;
            goto ErrorHandler;
        }
        prev_command = path_command;
    }

    if ((prev_command != VLC_OP_END))
    {
        stroke_conversion->path_last_point->next = NULL;
        if (stroke_conversion->point_count == 1)
        {
            /* Single point path. */
            vg_lite_path_point_ptr point = stroke_conversion->path_point_list;
            point->tangentX = 0.0f;
            point->tangentY = 0.0f;
            point->length = 0.0f;
        }
    }

ErrorHandler:
    return error;
}

static vg_lite_error_t
_add_point_to_right_stroke_point_list_tail(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr point;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    point = (vg_lite_path_point_ptr)malloc(sizeof(*point));
    if (!point)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    memset(point, 0, sizeof(*point));

    point->x = X;
    point->y = Y;
    point->curve_type = CURVE_LINE;
    point->prev = stroke_conversion->last_right_stroke_point;
    point->next = NULL;
    stroke_conversion->last_right_stroke_point->next = point;
    stroke_conversion->last_right_stroke_point = point;
    stroke_conversion->stroke_point_count++;

    stroke_conversion->last_stroke_sub_path->point_count++;

    return error;
}

static vg_lite_error_t
_add_point_to_left_stroke_point_list_head(
    vg_lite_stroke_conversion_t   *stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr point;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    point = (vg_lite_path_point_ptr)malloc(sizeof(*point));
    if (!point)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    memset(point, 0, sizeof(*point));

    point->x = X;
    point->y = Y;
    point->curve_type = CURVE_LINE;
    point->next = stroke_conversion->left_stroke_point;
    point->prev = NULL;
    stroke_conversion->left_stroke_point->prev = point;
    stroke_conversion->left_stroke_point = point;
    stroke_conversion->stroke_point_count++;

    stroke_conversion->last_stroke_sub_path->point_count++;

    return error;
}

static vg_lite_error_t _add_stroke_sub_path(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_sub_path_ptr *sub_path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (!stroke_conversion || !sub_path)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    *sub_path = (vg_lite_sub_path_ptr)malloc(sizeof(**sub_path));
    if (!*sub_path)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    memset(*sub_path, 0, sizeof(**sub_path));

    if (stroke_conversion->last_stroke_sub_path != NULL)
    {
        stroke_conversion->last_stroke_sub_path->next = *sub_path;
        stroke_conversion->last_stroke_sub_path = *sub_path;
    }
    else
    {
        stroke_conversion->last_stroke_sub_path = stroke_conversion->stroke_sub_path_list = *sub_path;
    }

    return error;
}

static vg_lite_error_t
_add_zero_length_stroke_sub_path(
    vg_lite_stroke_conversion_t   *stroke_conversion,
    vg_lite_sub_path_ptr *stroke_subpath
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr new_point, Point;
    vg_lite_sub_path_ptr stroke_sub_path;
    vg_lite_float_t half_width;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    half_width = stroke_conversion->half_line_width;
    Point = stroke_conversion->path_point_list;
    if (stroke_conversion->stroke_cap_style == VG_LITE_CAP_BUTT)
    {
        /* No need to draw zero-length subPath for gcvCAP_BUTT. */
        error = VG_LITE_SUCCESS;
        goto ErrorHandler;
    }

    VG_LITE_ERROR_HANDLER(_add_stroke_sub_path(stroke_conversion, &stroke_sub_path));

    if (stroke_conversion->stroke_cap_style == VG_LITE_CAP_SQUARE)
    {
        /* Draw a square along the point's direction. */
        vg_lite_float_t dx, dy;

        if (Point->tangentX == 0.0f || Point->tangentY == 0.0f)
        {
            dx = half_width;
            dy = 0.0f;
        }
        else
        {
            dx =  Point->tangentY * half_width;
            dy = -Point->tangentX * half_width;
        }

        new_point = (vg_lite_path_point_ptr)malloc(sizeof(*new_point));
        if (!new_point)
        {
            return VG_LITE_OUT_OF_RESOURCES;
        }
        memset(new_point, 0, sizeof(*new_point));

        new_point->x = Point->x + dx + dy;
        new_point->y = Point->y - dx + dy;
        new_point->curve_type = CURVE_LINE;
        stroke_sub_path->point_list = stroke_conversion->last_right_stroke_point = new_point;
        stroke_sub_path->point_count = 1;

        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x + dx - dy, Point->y + dx + dy));

        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x - dx - dy, Point->y + dx - dy));

        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x - dx + dy, Point->y - dx - dy));
    }
    else
    {
        /* Draw a circle. */
        new_point = (vg_lite_path_point_ptr)malloc(sizeof(*new_point));
        if (!new_point)
        {
            return VG_LITE_OUT_OF_RESOURCES;
        }
        memset(new_point, 0, sizeof(*new_point));

        new_point->x = Point->x + half_width;
        new_point->y = Point->y;
        new_point->curve_type = CURVE_LINE;
        stroke_sub_path->point_list = stroke_conversion->last_right_stroke_point = new_point;
        stroke_sub_path->point_count = 1;

        /* Add upper half circle. */
        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x - half_width, Point->y));

        stroke_conversion->last_right_stroke_point->curve_type = CURVE_ARC_SCCW_HALF;
        stroke_conversion->last_right_stroke_point->centerX = Point->x;
        stroke_conversion->last_right_stroke_point->centerY = Point->y;

        /* Add lower half circle. */
        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x + half_width, Point->y));

        stroke_conversion->last_right_stroke_point->curve_type = CURVE_ARC_SCCW_HALF;
        stroke_conversion->last_right_stroke_point->centerX = Point->x;
        stroke_conversion->last_right_stroke_point->centerY = Point->y;
    }

    stroke_sub_path->last_point = stroke_conversion->last_right_stroke_point;
    stroke_sub_path->last_point->next = NULL;

ErrorHandler:
    return error;
}

/* Special asin(x) for quick calculation when -sqrt(0.5) <= x <= sqrt(0.5). */
static vg_lite_float_t _Asin(
    vg_lite_float_t X
)
{
    vg_lite_float_t x = X;
    vg_lite_float_t x2 = X * X;
    vg_lite_float_t s = X;

    x *= x2;
    s += x * 0.1660510562575219f;
    x *= x2;
    s += x * 0.084044676143618186f;
    x *= x2;
    s += x * 0.0023776176698039313f;
    x *= x2;
    s += x * 0.10211922020091345f;

    return s;
}
/* Special cos(x) for quick calculation when -PI <= x <= PI. */
static vg_lite_float_t _Cos(
    vg_lite_float_t X
)
{
    vg_lite_float_t x2 = X * X;
    vg_lite_float_t x = x2;
    vg_lite_float_t s = 1.0f;

    s -= x * 0.49985163079668843f;
    x *= x2;
    s += x * 0.041518066216932693f;
    x *= x2;
    s -= x * 0.0013422997970712939f;
    x *= x2;
    s += x * 0.000018930111278021357f;

    return s;
}
/* Special sin(x) for quick calculation when -PI <= x <= PI. */
static vg_lite_float_t _Sine(
    vg_lite_float_t X
)
{
    vg_lite_float_t x = X;
    vg_lite_float_t x2 = X * X;
    vg_lite_float_t s = X;

    x *= x2;
    s -= x * 0.16664527099620879f;
    x *= x2;
    s += x * 0.0083154803736487041f;
    x *= x2;
    s -= x * 0.00019344151251408578f;
    x *= x2;
    s += x * 0.0000021810214160988925f;

    return s;
}

static vg_lite_float_t
_Angle(
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Length
)
{
    vg_lite_float_t angle;
    vg_lite_float_t ux = (X >= 0.0f ? X : -X);
    vg_lite_float_t uy = (Y >= 0.0f ? Y : -Y);

    if (ux > uy)
    {
        angle = ((uy > 0.0f && ux < Length) ? _Asin(uy / Length) : 0.0f);
    }
    else
    {
        angle = ((ux > 0.0f && uy < Length) ? (FLOAT_PI_HALF - _Asin(ux / Length)) : FLOAT_PI_HALF);
    }

    if (X < 0.0f) { angle = FLOAT_PI - angle; }
    if (Y < 0.0f) { angle = -angle; }

    return angle;
}

/* The arc is always counter clockwise and less than half circle (small). */
static vg_lite_error_t
_convert_circle_arc(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_float_t Radius,
    vg_lite_float_t CenterX,
    vg_lite_float_t CenterY,
    vg_lite_float_t StartX,
    vg_lite_float_t StartY,
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    uint8_t Half_circle,
    vg_lite_path_point_ptr *point_list
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    /*gceVGCMD segmentCommand;*/
    vg_lite_float_t theta1, theta_span;
    uint32_t segs;
    vg_lite_float_t theta, theta_half, theta2;
    vg_lite_float_t cos_theta_half;
    vg_lite_float_t control_ratio;
    vg_lite_float_t controlX, controlY, anchorX, anchorY;
    /*gctFLOAT lastX, lastY;*/
    vg_lite_path_point_ptr point, start_point, last_point;

    if (!stroke_conversion || !point_list)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Converting. */
    theta1 = _Angle(StartX - CenterX, StartY - CenterY, Radius);
    if (Half_circle)
    {
        theta_span = FLOAT_PI;
        segs = 4;
        theta = FLOAT_PI_QUARTER;
        theta_half = FLOAT_PI_EIGHTH;
        cos_theta_half = FLOAT_COS_PI_EIGHTH;
    }
    else
    {
        theta_span = _Angle(EndX - CenterX, EndY - CenterY, Radius) - theta1;
        if (theta_span == 0.0f)
        {
            /* Handle specail case for huge scaling. */
            *point_list = NULL;
            error = VG_LITE_SUCCESS;
            return error;
        }

        if ((theta_span < 0))
        {
            theta_span += FLOAT_PI_TWO;
        }

        /* Calculate the number of quadratic Bezier curves. */
        /* Assumption: most of angles are small angles. */
        if (theta_span <= FLOAT_PI_QUARTER) { segs = 1; }
        else if (theta_span <= FLOAT_PI_HALF) { segs = 2; }
        else if (theta_span <= FLOAT_PI_THREE_QUARTER) { segs = 3; }
        else { segs = 4; }

        theta = theta_span / segs;
        theta_half = theta / 2.0f;
        cos_theta_half = _Cos(theta_half);
    }

    /* Determine the segment command. */
    /*egmentCommand = gcvVGCMD_ARC_QUAD;*/

    /* Generate quadratic Bezier curves. */
    start_point = last_point = NULL;
    control_ratio = Radius / cos_theta_half;
    while (segs-- > 0)
    {
        theta1 += theta;

        theta2 = theta1 - theta_half;
        if (theta2 > FLOAT_PI) { theta2 -= FLOAT_PI_TWO; }
        controlX = CenterX + _Cos(theta2)  * control_ratio;
        controlY = CenterY + _Sine(theta2) * control_ratio;

        theta2 = theta1;
        if (theta2 > FLOAT_PI) { theta2 -= FLOAT_PI_TWO; }
        anchorX = CenterX + _Cos(theta2)  * Radius;
        anchorY = CenterY + _Sine(theta2) * Radius;

        if (segs == 0)
        {
            /* Use end point directly to avoid accumulated errors. */
            anchorX = EndX;
            anchorY = EndY;
        }

        /* Add control point. */
        point = (vg_lite_path_point_ptr)malloc(sizeof(*point));
        if (!point)
        {
            return VG_LITE_OUT_OF_RESOURCES;
        }

        memset(point, 0, sizeof(*point));

        point->x = controlX;
        point->y = controlY;
        point->curve_type = CURVE_QUAD_CONTROL;
        if (last_point)
        {
            last_point->next = point;
            last_point = point;
        }
        else
        {
            start_point = last_point = point;
        }

        /* Add anchor point. */
        point = (vg_lite_path_point_ptr)malloc(sizeof(*point));
        if (!point)
        {
            error = VG_LITE_OUT_OF_RESOURCES;
            goto ErrorHandler;
        }

        memset(point, 0, sizeof(*point));

        point->x = anchorX;
        point->y = anchorY;
        point->curve_type = CURVE_QUAD_ANCHOR;
        last_point->next = point;
        last_point = point;
    }

    last_point->next = NULL;
    *point_list = start_point;

    return error;
ErrorHandler:
    /* Return status. */
    if (start_point)
    {
        free(start_point);
        start_point = last_point = NULL;
    }
    return error;
}

static vg_lite_error_t
_start_new_stroke_sub_path(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Dx,
    vg_lite_float_t Dy,
    uint8_t add_end_cap,
    vg_lite_sub_path_ptr *stroke_subpath
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    vg_lite_sub_path_ptr stroke_sub_path;
    vg_lite_path_point_ptr new_point;

    if (!stroke_conversion || !stroke_subpath)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    VG_LITE_ERROR_HANDLER(_add_stroke_sub_path(stroke_conversion, &stroke_sub_path));

    new_point = (vg_lite_path_point_ptr)malloc(sizeof(*new_point));
    if (!new_point)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    memset(new_point, 0, sizeof(*new_point));
    new_point->x = X + Dx;
    new_point->y = Y + Dy;
    new_point->prev = NULL;
    new_point->curve_type = CURVE_LINE;
    stroke_conversion->stroke_point_list = stroke_conversion->last_right_stroke_point = new_point;

    stroke_sub_path->point_list = stroke_conversion->last_right_stroke_point = new_point;

    new_point = (vg_lite_path_point_ptr)malloc(sizeof(*new_point));
    if (!new_point)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    memset(new_point, 0, sizeof(*new_point));
    new_point->x = X - Dx;
    new_point->y = Y - Dy;
    new_point->curve_type = CURVE_LINE;
    new_point->next = NULL;
    stroke_conversion->stroke_last_point = stroke_conversion->left_stroke_point = new_point;

    stroke_conversion->stroke_point_count = 2;

    stroke_sub_path->last_point = stroke_conversion->left_stroke_point = new_point;
    stroke_sub_path->point_count = 2;

    if (add_end_cap)
    {
        /* Add end cap if the subPath is not closed. */
        switch (stroke_conversion->stroke_cap_style)
        {
        case VG_LITE_CAP_BUTT:
            /* No adjustment needed. */
            break;
        case VG_LITE_CAP_ROUND:
            /* Add curve. */
            /* Add the starting point again as arc. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             stroke_sub_path->point_list->x, stroke_sub_path->point_list->y));
            stroke_conversion->last_right_stroke_point->curve_type = CURVE_ARC_SCCW_HALF;
            stroke_conversion->last_right_stroke_point->centerX = X;
            stroke_conversion->last_right_stroke_point->centerY = Y;
            /* Change the starting point to end point. */
            stroke_sub_path->point_list->x = stroke_sub_path->last_point->x;
            stroke_sub_path->point_list->y = stroke_sub_path->last_point->y;
            break;
        case VG_LITE_CAP_SQUARE:
            stroke_conversion->last_right_stroke_point->x += Dy;
            stroke_conversion->last_right_stroke_point->y -= Dx;
            stroke_conversion->left_stroke_point->x += Dy;
            stroke_conversion->left_stroke_point->y -= Dx;
            break;
        }
    }

    *stroke_subpath = stroke_sub_path;

ErrorHandler:
    return error;
}

static void
_adjust_joint_point(
    vg_lite_path_point_ptr Point,
    vg_lite_path_point_ptr join_point,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Ratio
)
{
    vg_lite_float_t mx = (join_point->x + X) / 2.0f;
    vg_lite_float_t my = (join_point->y + Y) / 2.0f;
    vg_lite_float_t dx = mx - Point->x;
    vg_lite_float_t dy = my - Point->y;

    dx = dx * Ratio;
    dy = dy * Ratio;
    join_point->x = Point->x + dx;
    join_point->y = Point->y + dy;
}

static uint8_t
_is_angle_span_acute(
    vg_lite_float_t Ux,
    vg_lite_float_t Uy,
    vg_lite_float_t Vx,
    vg_lite_float_t Vy
)
{
    return ((Ux * Vx + Uy * Vy) > 0.0f ? 1 : 0);
}

static vg_lite_error_t
_draw_swing_pie_area(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_path_point_ptr center_point,
    uint8_t end_at_prev_point
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    if (stroke_conversion->swing_counter_clockwise)
    {
        vg_lite_path_point_ptr start_point = stroke_conversion->swing_start_stroke_point;
        vg_lite_path_point_ptr end_point = NULL, real_end_point = NULL;
        vg_lite_path_point_ptr point, prev_point;
        uint32_t count = 0;

        {
            if (end_at_prev_point)
            {
                /* Detach the end point from leftStrokePoint. */
                /* The end point will be added back later. */
                real_end_point = stroke_conversion->left_stroke_point;
                stroke_conversion->left_stroke_point = real_end_point->next;
                stroke_conversion->left_stroke_point->prev = NULL;
            }

            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion,
                                                                            center_point->x, center_point->y));
            end_point = stroke_conversion->left_stroke_point;

            /* Reverse the point list from startPoint to endPoint. */
            for (point = start_point; point; point = prev_point)
            {
                prev_point = point->prev;
                point->prev = point->next;
                point->next = prev_point;
                count++;
            }
            end_point->next = start_point->prev;
            start_point->prev->prev = end_point;
            start_point->prev = NULL;
            stroke_conversion->left_stroke_point = start_point;

            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion,
                                                                            center_point->x, center_point->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion,
                                                                            stroke_conversion->swing_start_point->x,
                                                                            stroke_conversion->swing_start_point->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion,
                                                                            end_point->prev->x, end_point->prev->y));

            if (end_at_prev_point)
            {
                real_end_point->next = stroke_conversion->left_stroke_point;
                stroke_conversion->left_stroke_point->prev = real_end_point;
                stroke_conversion->left_stroke_point = real_end_point;
            }
        }
    }
    else
    {
        vg_lite_path_point_ptr start_point = stroke_conversion->swing_start_stroke_point;
        vg_lite_path_point_ptr end_point = NULL, real_end_point = NULL;
        vg_lite_path_point_ptr point, next_point;
        uint32_t count = 0;

        {
            if (end_at_prev_point)
            {
                /* Detach the end point from leftStrokePoint. */
                /* The end point will be added back later. */
                real_end_point = stroke_conversion->last_right_stroke_point;
                stroke_conversion->last_right_stroke_point = real_end_point->prev;
                stroke_conversion->last_right_stroke_point->next = NULL;
            }

            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             center_point->x, center_point->y));
            end_point = stroke_conversion->last_right_stroke_point;

            /* Reverse the point list from startPoint to endPoint. */
            for (point = start_point; point; point = next_point)
            {
                next_point = point->next;
                point->next = point->prev;
                point->prev = next_point;
                count++;
            }
            end_point->prev = start_point->next;
            start_point->next->next = end_point;
            start_point->next = NULL;
            stroke_conversion->last_right_stroke_point = start_point;

            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             center_point->x, center_point->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             stroke_conversion->swing_start_point->x,
                                                                             stroke_conversion->swing_start_point->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             end_point->next->x, end_point->next->y));

            if (end_at_prev_point)
            {
                real_end_point->prev = stroke_conversion->last_right_stroke_point;
                stroke_conversion->last_right_stroke_point->next = real_end_point;
                stroke_conversion->last_right_stroke_point = real_end_point;
            }
        }
    }

    stroke_conversion->swing_handling = SWING_NO;

ErrorHandler:

    return error;
}

static vg_lite_error_t
_process_line_joint(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_path_point_ptr Point,
    vg_lite_float_t Length,
    vg_lite_float_t prev_length,
    uint32_t Swing_handling,
    vg_lite_float_t X1,
    vg_lite_float_t Y1,
    vg_lite_float_t X2,
    vg_lite_float_t Y2
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_join_style_t stroke_join_style = stroke_conversion->stroke_join_style;
    vg_lite_float_t half_width = stroke_conversion->half_line_width;
    vg_lite_float_t ratio;
    vg_lite_float_t min_length_square;
    vg_lite_float_t cos_theta;
    uint8_t counter_clockwise;
    uint8_t fat_line = stroke_conversion->is_fat;
    uint32_t swing_handling = SWING_NO;
    uint8_t handle_short_line = 0;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    if (stroke_conversion->swing_accu_length < half_width)
    {
        if (stroke_conversion->swing_need_to_handle)
        {
            swing_handling = SWING_OUT;
        }
        else
        {
            handle_short_line = 1;
        }
    }
    else if (stroke_conversion->stroke_path_length - stroke_conversion->swing_accu_length < half_width)
    {
        if (stroke_conversion->swing_need_to_handle)
        {
            swing_handling = SWING_IN;
        }
        else
        {
            handle_short_line = 1;
        }
    }

    if (swing_handling != Swing_handling)
    {
        error = VG_LITE_INVALID_ARGUMENT;
        goto ErrorHandler;
    }

    /* For flattened curves/arcs, the join style is always round. */
    if ((Point->flatten_flag != vgcFLATTEN_NO) && fat_line)
    {
        stroke_join_style = VG_LITE_JOIN_ROUND;
    }

    /* First, determine the turn is clockwise or counter-clockwise. */
    cos_theta = Point->prev->tangentX * Point->tangentX + Point->prev->tangentY * Point->tangentY;

    if (cos_theta > FLOAT_ANGLE_EPSILON_COS)
    {
        /* Straight line or semi-straight line--no need to handle join. */
        if (stroke_conversion->swing_handling != SWING_NO)
        {
            /* Begin to swing to the opposite direction. */
            /* Draw the swing area (pie area). */
            VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
        }

        /* Add the new stroke points. */
        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
        VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, X2, Y2));
        if (stroke_conversion->swing_handling != SWING_NO)
        {
            stroke_conversion->swing_count++;
        }

        goto endCheck;
    }
    else if (cos_theta < -FLOAT_ANGLE_EPSILON_COS)
    {
        /* Almost 180 degree turn. */
        counter_clockwise = 1;
        ratio = FLOAT_MAX;
        min_length_square = FLOAT_MAX;
    }
    else
    {
        vg_lite_float_t angleSign = Point->prev->tangentX * Point->tangentY - Point->prev->tangentY *
                                    Point->tangentX;
        counter_clockwise = (angleSign >= 0.0f ? 1 : 0);
        ratio = 2.0f / (1.0f + cos_theta);
        min_length_square = half_width * half_width * (1.0f - cos_theta) / (1.0f + cos_theta) + 0.02f;
    }

    if (stroke_conversion->swing_handling != SWING_NO)
    {
        if (counter_clockwise != stroke_conversion->swing_counter_clockwise)
        {
            /* Swing to the opposite direction. */
            /* Draw the swing area (pie area). */
            VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
        }
    }

    if (counter_clockwise)
    {
        if (stroke_conversion->swing_handling != SWING_NO)
        {
            vg_lite_path_point_ptr prev_point =
                stroke_conversion->left_stroke_point->next;   /* Skip the line segment movement. */
            vg_lite_float_t deltaX = X2 - prev_point->x;
            vg_lite_float_t deltaY = Y2 - prev_point->y;
            if (_is_angle_span_acute(stroke_conversion->swing_stroke_deltax,
                                     stroke_conversion->swing_stroke_deltay,
                                     deltaX, deltaY))
            {
                /* Continue swinging. */
                stroke_conversion->swing_stroke_deltax = deltaX;
                stroke_conversion->swing_stroke_deltay = deltaY;
            }
            else
            {
                /* Swing to the max. */
                /* Draw the swing area (pie area). */
                VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
            }
        }

        /* Check if the miter length is too long for inner intersection. */
        if (stroke_conversion->swing_handling == SWING_NO
            && ! handle_short_line
            && min_length_square <= Length * Length
            && min_length_square <= prev_length * prev_length)
        {
            /* Adjust leftStrokePoint to the intersection point. */
            _adjust_joint_point(Point, stroke_conversion->left_stroke_point, X2, Y2, ratio);
        }
        else if (stroke_conversion->swing_handling == SWING_NO && Point->flatten_flag == vgcFLATTEN_NO)
        {
            /* Add the point to avoid incorrect sharp angle. */
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, Point->x,
                                                                            Point->y));
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, X2, Y2));
        }
        else if (stroke_conversion->swing_handling == SWING_NO && (! fat_line ||
                                                                   Swing_handling == SWING_NO))
        {
            /* Flattened line segments should not have sharp angle. */
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, X2, Y2));
        }
        else
        {
            if (stroke_conversion->swing_handling == SWING_NO)
            {
                vg_lite_path_point_ptr prev_point = stroke_conversion->left_stroke_point;

                /* Start swing handling. */
                stroke_conversion->swing_handling = Swing_handling;
                stroke_conversion->swing_counter_clockwise = 1;
                stroke_conversion->swing_start_point = Point;
                stroke_conversion->swing_center_length = 0.0f;
                stroke_conversion->swing_count = 0;

                /* Save stroking path delta. */
                stroke_conversion->swing_stroke_deltax = X2 - prev_point->x;
                stroke_conversion->swing_stroke_deltay = Y2 - prev_point->y;

                /* Add extra center point for swing out pie area. */
                /* VIV: [todo] Should adjust prev_point, instead of adding new point? */
                VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, Point->x,
                                                                                Point->y));

                /* Add extra start stroke point for swing out pie area. */
                VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, prev_point->x,
                                                                                prev_point->y));

                stroke_conversion->swing_start_stroke_point = stroke_conversion->left_stroke_point;
            }

#if USE_MIN_ARC_FILTER
            if (cosTheta > FLOAT_MIN_ARC_ANGLE_COS)
            {
                /* Add a point. */
                gcmERR_GOTO(_add_point_to_left_stroke_point_list_head(Context, stroke_conversion, X2, Y2));

                VGSL_STAT_COUNTER_INCREASE(vgStrokeFilteredByMinArcAngleCount);
            }
            else
#endif
            {
                /* Add curve. */
                /* Note that the curve will be reversed, so the direction is CW. */
                /* Then, left side is in reversed order, so the direction is CCW. */
                VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, X2, Y2));
                stroke_conversion->left_stroke_point->curve_type = CURVE_ARC_SCCW;
                stroke_conversion->left_stroke_point->centerX = Point->x;
                stroke_conversion->left_stroke_point->centerY = Point->y;
            }
            stroke_conversion->swing_count++;
        }

        switch (stroke_join_style)
        {
        case VG_LITE_JOIN_ROUND:
            if (cos_theta > FLOAT_MIN_ARC_ANGLE_COS)
            {
                /* Add a point. */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
            }
            else
            {
                /* Add curve. */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
                stroke_conversion->last_right_stroke_point->curve_type = CURVE_ARC_SCCW;
                stroke_conversion->last_right_stroke_point->centerX = Point->x;
                stroke_conversion->last_right_stroke_point->centerY = Point->y;
            }
            break;
        case VG_LITE_JOIN_MITER:
            if (ratio <= stroke_conversion->stroke_miter_limit_square)
            {
                /* Adjust lastRightStrokePoint to the outer intersection point. */
                _adjust_joint_point(Point, stroke_conversion->last_right_stroke_point, X1, Y1, ratio);
                break;
            }
        /* Else use Bevel join style. */
        case VG_LITE_JOIN_BEVEL:
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
            break;
        }
    }
    else
    {
        if (stroke_conversion->swing_handling != SWING_NO)
        {
            vg_lite_path_point_ptr prev_point =
                stroke_conversion->last_right_stroke_point->prev;  /* Skip the line segment movement. */
            vg_lite_float_t deltaX = X1 - prev_point->x;
            vg_lite_float_t deltaY = Y1 - prev_point->y;
            if (_is_angle_span_acute(stroke_conversion->swing_stroke_deltax,
                                     stroke_conversion->swing_stroke_deltay,
                                     deltaX, deltaY))
            {
                /* Continue swinging. */
                stroke_conversion->swing_stroke_deltax = deltaX;
                stroke_conversion->swing_stroke_deltay = deltaY;
            }
            else
            {
                /* Swing to the max. */
                /* Draw the swing area (pie area). */
                VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
            }
        }

        /* Check if the miter length is too long for inner intersection. */
        if (stroke_conversion->swing_handling == SWING_NO
            && ! handle_short_line
            && min_length_square <= Length * Length
            && min_length_square <= prev_length * prev_length)
        {
            /* Adjust lastRightStrokePoint to the intersection point. */
            _adjust_joint_point(Point, stroke_conversion->last_right_stroke_point, X1, Y1, ratio);
        }
        else if (stroke_conversion->swing_handling == SWING_NO && Point->flatten_flag == vgcFLATTEN_NO)
        {
            /* Add the point to avoid incorrect sharp angle. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, Point->x,
                                                                             Point->y));
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
        }
        else if (stroke_conversion->swing_handling == SWING_NO && (! fat_line ||
                                                                   Swing_handling == SWING_NO))
        {
            /* Flattened line segments should not have sharp angle. */
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
        }
        else
        {
            if (stroke_conversion->swing_handling == SWING_NO)
            {
                vg_lite_path_point_ptr prev_point = stroke_conversion->last_right_stroke_point;

                /* Start swing handling. */
                stroke_conversion->swing_handling = Swing_handling;
                stroke_conversion->swing_counter_clockwise = 0;
                stroke_conversion->swing_start_point = Point;
                stroke_conversion->swing_center_length = 0.0f;
                stroke_conversion->swing_count = 0;

                /* Save stroking path delta. */
                stroke_conversion->swing_stroke_deltax = X1 - prev_point->x;
                stroke_conversion->swing_stroke_deltay = Y1 - prev_point->y;

                /* Add extra center point for swing out pie area. */
                /* VIV: [todo] Should adjust prev_point, instead of adding new point? */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, Point->x,
                                                                                 Point->y));

                /* Add extra start stroke point for swing out pie area. */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, prev_point->x,
                                                                                 prev_point->y));

                stroke_conversion->swing_start_stroke_point = stroke_conversion->last_right_stroke_point;
            }

            if (cos_theta > FLOAT_MIN_ARC_ANGLE_COS)
            {
                /* Add a point. */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
            }
            else
            {
                /* Add curve. */
                /* Note that the curve will be reversed, so the direction is CCW. */
                stroke_conversion->last_right_stroke_point->curve_type = CURVE_ARC_SCCW;
                stroke_conversion->last_right_stroke_point->centerX = Point->x;
                stroke_conversion->last_right_stroke_point->centerY = Point->y;
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
            }
            stroke_conversion->swing_count++;
        }

        switch (stroke_join_style)
        {
        case VG_LITE_JOIN_ROUND:
            if (cos_theta > FLOAT_MIN_ARC_ANGLE_COS)
            {
                /* Add a point. */
                VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, X2, Y2));
            }
            else
            {
                /* Add curve. */
                stroke_conversion->left_stroke_point->curve_type = CURVE_ARC_SCCW;
                stroke_conversion->left_stroke_point->centerX = Point->x;
                stroke_conversion->left_stroke_point->centerY = Point->y;
                VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, X2, Y2));
            }
            break;
        case VG_LITE_JOIN_MITER:
            if (ratio <= stroke_conversion->stroke_miter_limit_square)
            {
                /* Adjust leftStrokePoint to the outer intersection point. */
                _adjust_joint_point(Point, stroke_conversion->left_stroke_point, X2, Y2, ratio);
                break;
            }
        /* Else use Bevel join style. */
        case VG_LITE_JOIN_BEVEL:
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion, X2, Y2));
            break;
        }
    }

endCheck:
    if (stroke_conversion->swing_need_to_handle)
    {
        stroke_conversion->swing_accu_length += Point->length;
    }
    if (stroke_conversion->swing_handling != SWING_NO)
    {
        if (Point->flatten_flag == vgcFLATTEN_END ||
            (stroke_conversion->swing_handling == SWING_OUT &&
             stroke_conversion->swing_accu_length > half_width))
        {
            /* Draw the swing area (pie area). */
            VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point, 0));
        }
        else
        {
            /* Check if center line will move too far. */
            stroke_conversion->swing_center_length += Point->length;
            if (stroke_conversion->swing_center_length > FLOAT_SWING_CENTER_RANGE)
            {
#if USE_NEW_SWING_HANDLE_FOR_END
                if (stroke_conversion->currentSubPath->length < half_width ||
                    Point->next->flatten_flag == vgcFLATTEN_END)
#endif
                {
                    /* Draw the swing area (pie area). */
                    VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point, 0));
                }
            }
        }
    }

ErrorHandler:

    return error;
}

static vg_lite_error_t
_close_stroke_sub_path(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_path_point_ptr Point,
    vg_lite_float_t Length,
    vg_lite_float_t prev_length,
    uint8_t Swing_handling,
    vg_lite_path_point_ptr first_stroke_point,
    vg_lite_path_point_ptr last_stroke_point
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Handle line joint style for the first/last point in closed path. */
    VG_LITE_ERROR_HANDLER(_process_line_joint(
                              stroke_conversion, Point,
                              Length, prev_length, Swing_handling,
                              first_stroke_point->x, first_stroke_point->y,
                              last_stroke_point->x, last_stroke_point->y
                          ));

    /* Adjust the two end ponts of the first point. */
    first_stroke_point->x = stroke_conversion->last_right_stroke_point->x;
    first_stroke_point->y = stroke_conversion->last_right_stroke_point->y;
    last_stroke_point->x = stroke_conversion->left_stroke_point->x;
    last_stroke_point->y = stroke_conversion->left_stroke_point->y;

    /* Concatnate right and left point lists. */
    stroke_conversion->last_right_stroke_point->next = stroke_conversion->left_stroke_point;
    stroke_conversion->left_stroke_point->prev = stroke_conversion->last_right_stroke_point;

    /*gcmERROR_RETURN(_CheckStrokeSubPath(stroke_conversion->lastStrokeSubPath));*/

ErrorHandler:
    return error;
}

static vg_lite_error_t _end_stroke_sub_path(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Dx,
    vg_lite_float_t Dy
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Add points for end of line. */
    VG_LITE_RETURN_ERROR(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X + Dx, Y + Dy));
    VG_LITE_RETURN_ERROR(_add_point_to_left_stroke_point_list_head(stroke_conversion, X - Dx, Y - Dy));

    /* Add end cap if the subPath is not closed. */
    switch (stroke_conversion->stroke_cap_style)
    {
    case VG_LITE_CAP_BUTT:
        /* No adjustment needed. */
        break;
    case VG_LITE_CAP_ROUND:
        /* Add curve. */
        stroke_conversion->left_stroke_point->curve_type = CURVE_ARC_SCCW_HALF;
        stroke_conversion->left_stroke_point->centerX = X;
        stroke_conversion->left_stroke_point->centerY = Y;
        break;
    case VG_LITE_CAP_SQUARE:
        stroke_conversion->last_right_stroke_point->x -= Dy;
        stroke_conversion->last_right_stroke_point->y += Dx;
        stroke_conversion->left_stroke_point->x -= Dy;
        stroke_conversion->left_stroke_point->y += Dx;
        break;
    }

    /* Concatnate right and left point lists. */
    stroke_conversion->last_right_stroke_point->next = stroke_conversion->left_stroke_point;
    stroke_conversion->left_stroke_point->prev = stroke_conversion->last_right_stroke_point;

    /*gcmERROR_RETURN(_CheckStrokeSubPath(stroke_conversion->lastStrokeSubPath));*/
    return error;
}

static vg_lite_error_t _get_next_dash_length(
    vg_lite_stroke_conversion_t *stroke_conversion,
    uint32_t *dash_index,
    vg_lite_float_t *dash_length
)
{
    if (!stroke_conversion || !dash_index || !dash_length)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    (*dash_index)++;
    if (*dash_index == stroke_conversion->stroke_dash_pattern_count)
    {
        *dash_index = 0;
    }
    *dash_length = stroke_conversion->stroke_dash_pattern[*dash_index];

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t
_create_stroke_path(
    vg_lite_stroke_conversion_t *stroke_conversion
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_sub_path_ptr stroke_sub_path = NULL, first_stroke_sub_path = NULL;
    vg_lite_path_point_ptr point, next_point;
    vg_lite_float_t half_width;
    vg_lite_float_t x, y;
    vg_lite_float_t dx, dy, ux, uy;
    vg_lite_float_t length, prev_length, first_length;
    vg_lite_float_t dash_length;
    uint32_t dash_index;
    uint8_t dashing;
    uint8_t add_end_cap;
    uint8_t need_to_handle_swing = 1 /* (stroke_conversion->strokeCapStyle == gcvCAP_BUTT) */;

    vg_lite_path_point_ptr first_right_point = NULL;
    vg_lite_path_point_ptr last_left_point = NULL;
    vg_lite_float_t first_dx = 0.0f, first_dy = 0.0f;
    uint8_t drawing = 0;
    vg_lite_float_t total_length = 0.0f;
    vg_lite_float_t accu_length = 0.0f;
    uint32_t swing_handling = SWING_NO;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    half_width = stroke_conversion->half_line_width;
    dashing = stroke_conversion->stroke_dash_pattern_count > 0 ? 1 : 0;
    dash_index = stroke_conversion->stroke_dash_initial_index;
    dash_length = stroke_conversion->stroke_dash_initial_length;

    /* VIV: [todo] Need to check/debug closed stroke path. */
    need_to_handle_swing = (stroke_conversion->stroke_cap_style == VG_LITE_CAP_BUTT ||
                            stroke_conversion->closed);
    if (need_to_handle_swing)
    {
        uint8_t reallyneed_to_handle_swing = 0;

        /* Calculate the total length. */
        for (point = stroke_conversion->path_point_list; point; point = point->next)
        {
            total_length += point->length;

            if (point->flatten_flag != vgcFLATTEN_NO)
            {
                reallyneed_to_handle_swing = 1;
            }
        }
        stroke_conversion->stroke_path_length = total_length;
        if (reallyneed_to_handle_swing)
        {
            swing_handling = SWING_OUT;
        }
        else
        {
            need_to_handle_swing = 0;
            swing_handling = SWING_NO;
        }
    }
    stroke_conversion->swing_need_to_handle = need_to_handle_swing;

    point = stroke_conversion->path_point_list;
    next_point = point->next;
    if (next_point == NULL)
    {
        if (!dashing || ((dash_index & 0x1) == 0))
        {
            /* Single point (zero-length) subpath. */
            /* Note that one-MOVE_TO subpaths are removed during parsing. */
            VG_LITE_ERROR_HANDLER(_add_zero_length_stroke_sub_path(stroke_conversion, &stroke_sub_path));
        }
        goto ErrorHandler;
    }

    /* Adjust closed status for dashing. */
    if (dashing && stroke_conversion->closed && ((dash_index & 0x1) == 1))
    {
        stroke_conversion->closed = FALSE;
    }

    /* Set add_end_cap. */
    add_end_cap = dashing ? 1 : (stroke_conversion->closed ? 0 : 1);

    /* Process first line. */
    first_length = point->length;
    ux = point->tangentX;
    uy = point->tangentY;
    dx =  uy * half_width;
    dy = -ux * half_width;
    if (need_to_handle_swing)
    {
        stroke_conversion->swing_accu_length = first_length;
    }

    if (dashing)
    {
        vg_lite_float_t delta_length;

        /* Draw dashes. */
        x = point->x;
        y = point->y;
        do
        {
            if ((dash_index & 0x1) == 0)
            {
                VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                          stroke_conversion,
                                          x, y,
                                          dx, dy, add_end_cap,
                                          &stroke_sub_path
                                      ));

                drawing = 1;
                add_end_cap = 1;
                if (stroke_conversion->closed && (first_stroke_sub_path == NULL))
                {
                    first_stroke_sub_path = stroke_conversion->last_stroke_sub_path;
                    first_right_point = stroke_conversion->last_right_stroke_point;
                    last_left_point = stroke_conversion->left_stroke_point;
                    first_dx = dx;
                    first_dy = dy;
                }
            }

            delta_length = first_length - dash_length;
            if (delta_length >= FLOAT_EPSILON)
            {
                /* Move (x, y) forward along the line by dash_length. */
                x += ux * dash_length;
                y += uy * dash_length;

                if ((dash_index & 0x1) == 0)
                {
                    VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                              stroke_conversion,
                                              x, y,
                                              dx, dy
                                          ));

                    drawing = 0;
                }

                VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                first_length = delta_length;
            }
            else if (delta_length <= -FLOAT_EPSILON)
            {
                dash_length = -delta_length;
                break;
            }
            else
            {
                if ((dash_index & 0x1) == 0)
                {
                    VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                              stroke_conversion,
                                              next_point->x, next_point->y,
                                              dx, dy
                                          ));

                    drawing = 0;
                }

                VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                first_length = 0;
                break;
            }
        }
        while (1);
    }
    else
    {
        VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                  stroke_conversion,
                                  point->x, point->y,
                                  dx, dy, add_end_cap,
                                  &stroke_sub_path
                              ));

        drawing = 1;
        add_end_cap = 1;
    }

    /* Process the rest of lines. */
    prev_length = first_length;
    for (point = next_point, next_point = point->next; next_point;
         point = next_point, next_point = point->next)
    {
        if (!dashing || ((dash_index & 0x1) == 0 && drawing))
        {
            /* Add points for end of line for line join process with next line. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             point->x + dx, point->y + dy));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion,
                                                                            point->x - dx, point->y - dy));
        }

        length = point->length;
        ux = point->tangentX;
        uy = point->tangentY;
        dx =  uy * half_width;
        dy = -ux * half_width;
        if (need_to_handle_swing)
        {
            accu_length += point->prev->length;
            stroke_conversion->swing_accu_length = accu_length;
            if (accu_length < half_width)
            {
                swing_handling = SWING_OUT;
            }
            else if (total_length - accu_length < half_width)
            {
                swing_handling = SWING_IN;
            }
            else
            {
                swing_handling = SWING_NO;
            }
        }

        if (!dashing)
        {
            /* Handle line joint style. */
            VG_LITE_ERROR_HANDLER(_process_line_joint(
                                      stroke_conversion, point,
                                      length, prev_length, swing_handling,
                                      point->x + dx, point->y + dy,
                                      point->x - dx, point->y - dy
                                  ));
        }
        else
        {
            vg_lite_float_t delta_length;

            /* Draw dashes. */
            x = point->x;
            y = point->y;
            if ((dash_index & 0x1) == 0)
            {
                if (drawing)
                {
                    /* Handle line joint style. */
                    VG_LITE_ERROR_HANDLER(_process_line_joint(
                                              stroke_conversion, point,
                                              dash_length, prev_length, swing_handling,
                                              x + dx, y + dy,
                                              x - dx, y - dy
                                          ));
                }
                else
                {
                    /* Start a new sub path. */
                    VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                              stroke_conversion,
                                              x, y,
                                              dx, dy, add_end_cap,
                                              &stroke_sub_path
                                          ));

                    drawing = 1;
                    add_end_cap = 1;
                }
            }
            do
            {
                delta_length = length - dash_length;
                if (delta_length >= FLOAT_EPSILON)
                {
                    /* Move (x, y) forward along the line by dash_length. */
                    x += ux * dash_length;
                    y += uy * dash_length;

                    if ((dash_index & 0x1) == 0)
                    {
                        VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                                  stroke_conversion,
                                                  x, y, dx, dy
                                              ));

                        drawing = 0;
                    }

                    VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                    length = delta_length;
                }
                else if (delta_length <= -FLOAT_EPSILON)
                {
                    dash_length = -delta_length;
                    break;
                }
                else
                {
                    if ((dash_index & 0x1) == 0)
                    {
                        VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                                  stroke_conversion,
                                                  next_point->x, next_point->y,
                                                  dx, dy
                                              ));

                        drawing = 0;
                    }

                    VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                    length = 0;
                    break;
                }

                if ((dash_index & 0x1) == 0)
                {
                    VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                              stroke_conversion,
                                              x, y,
                                              dx, dy, add_end_cap,
                                              &stroke_sub_path
                                          ));

                    drawing = 1;
                    add_end_cap = 1;
                }
            }
            while (1);
        }

        prev_length = length;
    }

    if (need_to_handle_swing)
    {
        accu_length += point->prev->length;
        stroke_conversion->swing_accu_length = accu_length;
        if (accu_length < half_width)
        {
            swing_handling = SWING_OUT;
        }
        else if (total_length - accu_length < half_width)
        {
            swing_handling = SWING_IN;
        }
        else
        {
            swing_handling = SWING_NO;
        }
    }

    if (stroke_conversion->swing_handling != SWING_NO)
    {
        /* Draw the swing area (pie area). */
        VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, stroke_conversion->path_last_point,
                                                   FALSE));
    }

    if (stroke_conversion->closed)
    {
        if (! dashing || drawing)
        {
            /* Add points for end of line. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             point->x + dx, point->y + dy));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_stroke_point_list_head(stroke_conversion,
                                                                            point->x - dx, point->y - dy));

            if (! dashing)
            {
                /* Handle line joint style for the first/last point in closed path. */
                VG_LITE_ERROR_HANDLER(_close_stroke_sub_path(
                                          stroke_conversion, point,
                                          first_length, prev_length, swing_handling,
                                          stroke_sub_path->point_list, stroke_sub_path->last_point
                                      ));
            }
            else
            {
                /* Handle line joint style for the first/last point in closed path. */
                VG_LITE_ERROR_HANDLER(_close_stroke_sub_path(
                                          stroke_conversion, point,
                                          first_length, prev_length, swing_handling,
                                          first_right_point, last_left_point
                                      ));
            }
        }
        else if (stroke_conversion->stroke_cap_style != VG_LITE_CAP_BUTT)
        {
            /* No closing join need.  Add end cap for the starting point. */

            if (stroke_conversion->stroke_cap_style == VG_LITE_CAP_SQUARE)
            {
                first_right_point->x += first_dy;
                first_right_point->y -= first_dx;
                last_left_point->x += first_dy;
                last_left_point->y -= first_dx;
            }
            else
            {
                vg_lite_sub_path_ptr last_stroke_sub_path = stroke_conversion->last_stroke_sub_path;
                vg_lite_path_point_ptr start_point = last_stroke_sub_path->point_list;
                vg_lite_path_point_ptr point;

                /* Add curve. */
                /* Add extra point to the beginning with end point's coordinates. */
                point = (vg_lite_path_point_ptr)malloc(sizeof(*point));
                if (!point)
                {
                    return VG_LITE_INVALID_ARGUMENT;
                }
                memset(point, 0, sizeof(*point));

                point->x = last_stroke_sub_path->last_point->x;
                point->y = last_stroke_sub_path->last_point->y;
                point->next = start_point;
                start_point->prev = point;
                start_point->curve_type = CURVE_ARC_SCCW;
                start_point->centerX = stroke_conversion->path_point_list->x;
                start_point->centerY = stroke_conversion->path_point_list->y;
                last_stroke_sub_path->point_list = point;
            }
        }
    }
    else if (! dashing ||
             (((dash_index & 0x1) == 0) && (dash_length < stroke_conversion->stroke_dash_pattern[dash_index])))
    {
        /* Add end cap if the subPath is not closed. */
        VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                  stroke_conversion,
                                  point->x, point->y,
                                  dx, dy
                              ));

        drawing = 0;
    }


ErrorHandler:
    return error;
}

static vg_lite_error_t _copy_stroke_path(
    vg_lite_stroke_conversion_t *stroke_conversion,
    vg_lite_path_t *path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr point, prev_point, tmp_point;
    vg_lite_float_t totalsize = 0, real_size = 0;
    float *pfloat;
    char *cpath;
    void *temp_stroke_path_data = NULL;
    uint32_t temp_stroke_path_size;
    vg_lite_sub_path_ptr sub_path;
    vg_lite_float_t half_width;

    if (!stroke_conversion || !path)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    half_width = stroke_conversion->half_line_width;
    sub_path = stroke_conversion->stroke_sub_path_list;

    if (!stroke_conversion || !path || !sub_path)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    while (sub_path)
    {
        tmp_point = prev_point = point = sub_path->point_list;
        totalsize += _commandSize_float[VLC_OP_LINE] * sub_path->point_count +
                     _commandSize_float[VLC_OP_CLOSE];
        for (; tmp_point; tmp_point = tmp_point->next)
        {
            if (tmp_point->curve_type == CURVE_ARC_SCCW || tmp_point->curve_type == CURVE_ARC_SCCW_HALF)
            {
                totalsize += 4 * _commandSize_float[VLC_OP_QUAD];
            }
        }

        temp_stroke_path_data = path->stroke_path_data;
        temp_stroke_path_size = path->stroke_path_size;

        path->stroke_path_size += totalsize;
        path->stroke_path_data = (void *)malloc(path->stroke_path_size);
        if (!path->stroke_path_data)
        {
            error = VG_LITE_INVALID_ARGUMENT;
            goto ErrorHandler;
        }

        memset(path->stroke_path_data, 0, path->stroke_path_size);

        if (temp_stroke_path_data)
        {
            memcpy(path->stroke_path_data, temp_stroke_path_data, temp_stroke_path_size);
            free(temp_stroke_path_data);
            temp_stroke_path_data = NULL;
        }

        pfloat = (vg_lite_float_t *)((char *)path->stroke_path_data + temp_stroke_path_size);
        cpath = (char *)pfloat;
        *cpath = VLC_OP_MOVE;
        pfloat++;
        *pfloat++ = point->x;
        *pfloat++ = point->y;
        real_size += _commandSize_float[VLC_OP_MOVE];

        for (point = point->next; point; prev_point = point, point = point->next)
        {
            if (point->curve_type == CURVE_LINE)
            {
                if (point->x == prev_point->x && point->y == prev_point->y)
                {
                    path->stroke_path_size -= _commandSize_float[VLC_OP_LINE];
                    /* Skip zero-length lines. */
                    continue;
                }

                /* Add new command. */
                cpath = (char *)pfloat;
                *cpath = VLC_OP_LINE;
                pfloat++;

                /* Set the coordinates. */
                *pfloat++ = point->x;
                *pfloat++ = point->y;
                real_size += _commandSize_float[VLC_OP_LINE];
            }
            else if (point->curve_type == CURVE_QUAD_CONTROL)
            {
                /* Add new command. */
                cpath = (char *)pfloat;
                *cpath = VLC_OP_QUAD;
                pfloat++;

                /* Set the coordinates. */
                prev_point = point, point = point->next;
                *pfloat++ = prev_point->x;
                *pfloat++ = prev_point->y;
                *pfloat++ = point->x;
                *pfloat++ = point->y;

                real_size += _commandSize_float[VLC_OP_QUAD];
            }
            else
            {
                vg_lite_path_point_ptr point_list, p, nextP;
                vg_lite_path_point_ptr p2;

                if (point->curve_type == CURVE_ARC_SCCW)
                {
                    /* Convert an arc to Bezier curves. */
                    VG_LITE_ERROR_HANDLER(_convert_circle_arc(stroke_conversion, half_width,
                                                              point->centerX, point->centerY,
                                                              prev_point->x, prev_point->y,
                                                              point->x, point->y,
                                                              0, &point_list));
                }
                else
                {
                    /* Convert a half circle to Bezier curves. */
                    VG_LITE_ERROR_HANDLER(_convert_circle_arc(stroke_conversion, half_width,
                                                              point->centerX, point->centerY,
                                                              prev_point->x, prev_point->y,
                                                              point->x, point->y,
                                                              1, &point_list));

                }

                if (point_list)
                {
                    for (p = point_list; p; p = nextP)
                    {
                        /* Add new command. */
                        cpath = (char *)pfloat;
                        *cpath = VLC_OP_QUAD;
                        pfloat++;

                        /* Set the coordinates. */
                        p2 = p->next;
                        nextP = p2->next;

                        *pfloat++ = p->x;
                        *pfloat++ = p->y;
                        *pfloat++ = p2->x;
                        *pfloat++ = p2->y;
                        real_size += _commandSize_float[VLC_OP_QUAD];
                    }
                    vg_lite_path_point_ptr temp_point;
                    while (point_list)
                    {
                        temp_point = point_list->next;
                        free(point_list);
                        point_list = temp_point;
                    }
                    temp_point = NULL;
                }
                else
                {
                    /* Handle special case of huge scaling. */
                    /* Add new command. */
                    cpath = (char *)pfloat;
                    *cpath = VLC_OP_LINE;
                    pfloat++;

                    /* Set the coordinates. */
                    *pfloat++ = point->x;
                    *pfloat++ = point->y;
                    real_size += _commandSize_float[VLC_OP_LINE];
                }
            }
        }

        /* Create a CLOSE_PATH command at the end. */
        cpath = (char *)pfloat;
        if (sub_path->next)
        {
            *cpath = VLC_OP_CLOSE;
        }
        else
        {
            *cpath = VLC_OP_END;
        }
        real_size += _commandSize_float[VLC_OP_CLOSE];
        path->stroke_path_size = temp_stroke_path_size + real_size;
        totalsize = 0;
        real_size = 0;
        sub_path = sub_path->next;
    }

ErrorHandler:

    if (temp_stroke_path_data)
    {
        free(temp_stroke_path_data);
        temp_stroke_path_data = NULL;
    }

    return error;
}

static vg_lite_error_t _initialize_stroke_dash_parameters(
    vg_lite_stroke_conversion_t *stroke_conversion
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t count;
    uint32_t i;
    vg_lite_float_t *pattern_src;
    vg_lite_float_t *pattern, *temp_pattern;
    vg_lite_float_t length;

    if (!stroke_conversion)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    count = stroke_conversion->stroke_dash_pattern_count;
    if (count == 0 || !stroke_conversion->stroke_dash_pattern)
    {
        return error;
    }

    length = stroke_conversion->stroke_dash_phase;

    /* The last pattern is ignored if the number is odd. */
    if (count & 0x1) { count--; }

    pattern = (vg_lite_float_t *)malloc(count * sizeof(vg_lite_float_t));
    if (!pattern)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    temp_pattern = pattern;
    stroke_conversion->stroke_dash_pattern_length = 0.0f;
    pattern_src = stroke_conversion->stroke_dash_pattern;

    for (i = 0; i < count; i++, pattern++, pattern_src++)
    {
        if (*pattern_src < 0.0f)
        {
            *pattern = 0.0f;
        }
        else
        {
            *pattern = *pattern_src;
        }
        stroke_conversion->stroke_dash_pattern_length += *pattern;
    }

    if (stroke_conversion->stroke_dash_pattern_length < FLOAT_EPSILON)
    {
        stroke_conversion->stroke_dash_pattern_count = 0;
        free(temp_pattern);
        temp_pattern = NULL;
        return error;
    }

    while (length < 0.0f)
    {
        length += stroke_conversion->stroke_dash_pattern_length;
    }

    while (length >= stroke_conversion->stroke_dash_pattern_length)
    {
        length -= stroke_conversion->stroke_dash_pattern_length;
    }

    pattern = stroke_conversion->stroke_dash_pattern;
    for (i = 0; i < stroke_conversion->stroke_dash_pattern_count; i++, pattern++)
    {
        if (length <= *pattern) { break; }

        length -= *pattern;
    }

    stroke_conversion->stroke_dash_initial_index = i;
    stroke_conversion->stroke_dash_initial_length = *pattern - length;

    free(temp_pattern);
    temp_pattern = NULL;

    return error;
}

vg_lite_error_t vg_lite_update_stroke(
    vg_lite_path_t *path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_stroke_conversion_t *stroke_conversion;

    if (!path)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    stroke_conversion = &path->stroke_conversion;

    /* Free the stroke. */
    if (path->stroke_path_data)
    {
        free(path->stroke_path_data);
        /* Reset the stroke. */
        path->stroke_path_data = NULL;
    }

    if (stroke_conversion->stroke_line_width >= FLOAT_FAT_LINE_WIDTH
        &&  stroke_conversion->stroke_line_width >= 1.0f)
    {
        stroke_conversion->is_fat = 1;
    }
    VG_LITE_RETURN_ERROR(_initialize_stroke_dash_parameters(stroke_conversion));
    VG_LITE_RETURN_ERROR(_flatten_path(stroke_conversion, path));
    VG_LITE_RETURN_ERROR(_create_stroke_path(stroke_conversion));
    VG_LITE_RETURN_ERROR(_copy_stroke_path(stroke_conversion, path));

    return error;
}

vg_lite_error_t vg_lite_set_stroke(
    vg_lite_path_t *path,
    vg_lite_cap_style_t stroke_cap_style,
    vg_lite_join_style_t stroke_join_style,
    vg_lite_float_t stroke_line_width,
    vg_lite_float_t stroke_miter_limit,
    vg_lite_float_t *stroke_dash_pattern,
    uint32_t stroke_dash_pattern_count,
    vg_lite_float_t stroke_dash_phase,
    vg_lite_color_t stroke_color
)
{
    if (!path || stroke_line_width <= 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    if (stroke_miter_limit < 1.0f)
    {
        stroke_miter_limit = 1.0f;
    }

    path->stroke_conversion.stroke_cap_style = stroke_cap_style;
    path->stroke_conversion.stroke_join_style = stroke_join_style;
    path->stroke_conversion.stroke_line_width = stroke_line_width;
    path->stroke_conversion.stroke_miter_limit = stroke_miter_limit;
    path->stroke_conversion.half_line_width = stroke_line_width / 2.0f;
    path->stroke_conversion.stroke_miter_limit_square = path->stroke_conversion.stroke_miter_limit *
                                                        path->stroke_conversion.stroke_miter_limit;
    path->stroke_conversion.stroke_dash_pattern = stroke_dash_pattern;
    path->stroke_conversion.stroke_dash_pattern_count = stroke_dash_pattern_count;
    path->stroke_conversion.stroke_dash_phase = stroke_dash_phase;

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t _check_compress_valid(
    vg_lite_buffer_format_t format,
    vg_lite_compress_mode_t compress_mode,
    vg_lite_buffer_layout_t tiled,
    uint32_t width,
    uint32_t height
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (compress_mode)
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_DEC_COMPRESS))
        {
            return VG_LITE_NOT_SUPPORT;
        }

        if (compress_mode > VG_LITE_DEC_HV_SAMPLE || compress_mode < VG_LITE_DEC_DISABLE)
        {
            return VG_LITE_INVALID_ARGUMENT;
        }

        if (tiled)
        {
            if (width % 16 || height % 4)
            {
                return VG_LITE_INVALID_ARGUMENT;
            }
        }
        else
        {
            if (width % 16 || compress_mode == VG_LITE_DEC_HV_SAMPLE)
            {
                return VG_LITE_INVALID_ARGUMENT;
            }
        }

        if (format != VG_LITE_XBGR8888 && format != VG_LITE_XRGB8888 && format != VG_LITE_BGRX8888 &&
            format != VG_LITE_RGBX8888 &&
            format != VG_LITE_ABGR8888 && format != VG_LITE_ARGB8888 && format != VG_LITE_BGRA8888 &&
            format != VG_LITE_RGBA8888)
        {
            return VG_LITE_INVALID_ARGUMENT;
        }
    }

    return error;
}

static vg_lite_float_t _calc_decnano_compress_ratio(
    vg_lite_buffer_format_t format,
    vg_lite_compress_mode_t compress_mode
)
{
    vg_lite_float_t ratio = 1.0f;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_DEC_COMPRESS))
    {
        return ratio;
    }

    switch (compress_mode)
    {
    case VG_LITE_DEC_NON_SAMPLE:
        switch (format)
        {
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBA8888:
            ratio = 0.625;
            break;
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGBX8888:
            ratio = 0.5;
            break;
        default:
            return ratio;
        }
        break;

    case VG_LITE_DEC_HSAMPLE:
        switch (format)
        {
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBA8888:
            ratio = 0.5;
            break;
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGBX8888:
            ratio = 0.375;
            break;
        default:
            return ratio;
        }
        break;

    case VG_LITE_DEC_HV_SAMPLE:
        switch (format)
        {
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBA8888:
            ratio = 0.375;
            break;
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGBX8888:
            ratio = 0.25;
            break;
        default:
            return ratio;
        }
        break;
    default:
        return ratio;
    }
    return ratio;
}

static vg_lite_float_t _angle(
    vg_lite_float_t Ux,
    vg_lite_float_t Uy,
    vg_lite_float_t Vx,
    vg_lite_float_t Vy
)
{

    vg_lite_float_t dot, length, angle, cosVal;
    int32_t sign;

    dot    = Ux * Vx + Uy * Vy;
    length = SQRTF(Ux * Ux + Uy * Uy) * SQRTF(Vx * Vx + Vy * Vy);
    sign   = (Ux * Vy - Uy * Vx < 0) ? -1 : 1;
    cosVal = dot / length;
    cosVal = CLAMP(cosVal, -1.0f, 1.0f);
    angle  = sign * ACOSF(cosVal);
    return angle;
}

/*!
    @discussion
    Convert arc to multi-segment bezier curve.
    @param HorRadius
    Major axis radius.
    @param VerRadius
    minor axis radius.
    @param RotAngle
    Rotation angle.
    @param EndX
    End coordinate x.
    @param EndX
    End coordinate y.
    @param CounterClockwise
    If this is 0,anticlockwise rotation,if this is 1,clockwise rotation.
    @param Large
    1 means big arc,0 means little arc.
    @param Relative
    1 means absolute coordinates,0 means relative coordinates.
    @param coords
    Including the start point coordinates of the path,the control point of the last segment of the path,
    and the end point of the last segment of the path.
    @param path_data
    Path data usr for internal conversion.
    @param offset
    The offset of path_data.
    @param last_size
    The remain unconverted size of the original path data.
    @result
    Error code. VG_LITE_INVALID_ARGUMENTS to indicate the parameters are wrong.
*/
vg_lite_error_t _convert_arc(
    vg_lite_float_t HorRadius,
    vg_lite_float_t VerRadius,
    vg_lite_float_t RotAngle,
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    uint8_t CounterClockwise,
    uint8_t Large,
    uint8_t Relative,
    vg_lite_control_coord_t *coords,
    void **path_data,
    uint32_t *offset,
    uint32_t last_size
)
{
    vg_lite_float_t endX, endY;
    uint8_t segmentCommand;
    vg_lite_float_t phi, cosPhi, sinPhi;
    vg_lite_float_t dxHalf, dyHalf;
    vg_lite_float_t x1Prime, y1Prime;
    vg_lite_float_t rx, ry;
    vg_lite_float_t x1PrimeSquare, y1PrimeSquare;
    vg_lite_float_t lambda;
    vg_lite_float_t rxSquare, rySquare;
    int32_t sign;
    vg_lite_float_t sq, signedSq;
    vg_lite_float_t cxPrime, cyPrime;
    vg_lite_float_t theta1, thetaSpan;
    int32_t segs;
    vg_lite_float_t theta, ax, ay, x, y;
    vg_lite_float_t controlX, controlY, anchorX, anchorY;
    vg_lite_float_t lastX, lastY;
    uint32_t bufferSize;
    char    *pchar, *arcPath;
    vg_lite_float_t   *pfloat;
    /*******************************************************************
    ** Converting.
    */
    if (path_data == NULL || *path_data == NULL || offset == NULL || coords == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    if (Relative)
    {
        endX = EndX + coords->lastX;
        endY = EndY + coords->lastY;
    }
    else
    {
        endX = EndX;
        endY = EndY;
    }

    phi = RotAngle / 180.0f * PI;
    cosPhi = COSF(phi);
    sinPhi = SINF(phi);

    if (Relative)
    {
        dxHalf = - EndX / 2.0f;
        dyHalf = - EndY / 2.0f;
    }
    else
    {
        dxHalf = (coords->lastX - endX) / 2.0f;
        dyHalf = (coords->lastY - endY) / 2.0f;
    }

    x1Prime =  cosPhi * dxHalf + sinPhi * dyHalf;
    y1Prime = -sinPhi * dxHalf + cosPhi * dyHalf;

    rx = FABSF(HorRadius);
    ry = FABSF(VerRadius);

    x1PrimeSquare = x1Prime * x1Prime;
    y1PrimeSquare = y1Prime * y1Prime;

    lambda = x1PrimeSquare / (rx * rx) + y1PrimeSquare / (ry * ry);
    if (lambda > 1.0f)
    {
        rx *= SQRTF(lambda);
        ry *= SQRTF(lambda);
    }

    rxSquare = rx * rx;
    rySquare = ry * ry;

    sign     = (Large == CounterClockwise) ? -1 : 1;
    sq       = (rxSquare * rySquare
                - rxSquare * y1PrimeSquare
                - rySquare * x1PrimeSquare
               )
               /
               (rxSquare * y1PrimeSquare
                + rySquare * x1PrimeSquare
               );
    signedSq = sign * ((sq < 0) ? 0 : SQRTF(sq));
    cxPrime  = signedSq * (rx * y1Prime / ry);
    cyPrime  = signedSq * -(ry * x1Prime / rx);

    theta1 = _angle(1, 0, (x1Prime - cxPrime) / rx, (y1Prime - cyPrime) / ry);
    theta1 = FMODF(theta1, 2 * PI);

    thetaSpan = _angle((x1Prime - cxPrime) / rx, (y1Prime - cyPrime) / ry,
                       (-x1Prime - cxPrime) / rx, (-y1Prime - cyPrime) / ry);

    if (!CounterClockwise && (thetaSpan > 0))
    {
        thetaSpan -= 2 * PI;
    }
    else if (CounterClockwise && (thetaSpan < 0))
    {
        thetaSpan += 2 * PI;
    }

    thetaSpan = FMODF(thetaSpan, 2 * PI);


    /*******************************************************************
    ** Drawing.
    */

    segs  = (int32_t)(CEILF(FABSF(thetaSpan) / (45.0f / 180.0f * PI)));

    theta = thetaSpan / segs;

    ax = coords->lastX - COSF(theta1) * rx;
    ay = coords->lastY - SINF(theta1) * ry;

    /* Determine the segment command. */
    segmentCommand = Relative
                     ? VLC_OP_QUAD_REL
                     : VLC_OP_QUAD;

    /* Determine the size of the buffer required. */
    bufferSize = (1 + 2 * 2) * SIZEOF(vg_lite_float_t) * segs;

    arcPath = (char *)malloc(*offset + bufferSize + last_size);
    if (arcPath == NULL)
    {
        return VG_LITE_OUT_OF_MEMORY;
    }
    memcpy(arcPath, (char *)*path_data, *offset);
    free(*path_data);

    *path_data = arcPath;

    pchar = arcPath + *offset;
    pfloat = (vg_lite_float_t *)pchar;

    /* Set initial last point. */
    lastX = coords->lastX;
    lastY = coords->lastY;

    while (segs-- > 0)
    {
        theta1 += theta;

        controlX = ax + COSF(theta1 - (theta / 2.0f)) * rx / COSF(theta / 2.0f);
        controlY = ay + SINF(theta1 - (theta / 2.0f)) * ry / COSF(theta / 2.0f);

        anchorX = ax + COSF(theta1) * rx;
        anchorY = ay + SINF(theta1) * ry;

        if (RotAngle != 0)
        {
            x = coords->lastX + cosPhi * (controlX - coords->lastX) - sinPhi * (controlY - coords->lastY);
            y = coords->lastY + sinPhi * (controlX - coords->lastX) + cosPhi * (controlY - coords->lastY);
            controlX = x;
            controlY = y;

            x = coords->lastX + cosPhi * (anchorX - coords->lastX) - sinPhi * (anchorY - coords->lastY);
            y = coords->lastY + sinPhi * (anchorX - coords->lastX) + cosPhi * (anchorY - coords->lastY);
            anchorX = x;
            anchorY = y;
        }

        if (segs == 0)
        {
            /* Use end point directly to avoid accumulated errors. */
            anchorX = endX;
            anchorY = endY;
        }

        /* Adjust relative coordinates. */
        if (Relative)
        {
            vg_lite_float_t nextLastX = anchorX;
            vg_lite_float_t nextLastY = anchorY;

            controlX -= lastX;
            controlY -= lastY;

            anchorX -= lastX;
            anchorY -= lastY;

            lastX = nextLastX;
            lastY = nextLastY;
        }
        pchar = (char *)pfloat;
        *pchar = segmentCommand ;
        pfloat++;
        *pfloat++ = controlX;
        *pfloat++ = controlY;
        *pfloat++ = anchorX;
        *pfloat++ = anchorY;
        *offset += (1 + 2 * 2) * SIZEOF(vg_lite_float_t);
    }

    /* Update the control coordinates. */
    coords->lastX    = endX;
    coords->lastY    = endY;
    coords->controlX = endX;
    coords->controlY = endY;

    return VG_LITE_SUCCESS;
}

static void ClampColor(FLOATVECTOR4 Source, FLOATVECTOR4 Target, uint8_t Premultiplied)
{
    vg_lite_float_t colorMax;
    /* Clamp the alpha channel. */
    Target[3] = CLAMP(Source[3], 0.0f, 1.0f);

    /* Determine the maximum value for the color channels. */
    colorMax = Premultiplied ? Target[3] : 1.0f;

    /* Clamp the color channels. */
    Target[0] = CLAMP(Source[0], 0.0f, colorMax);
    Target[1] = CLAMP(Source[1], 0.0f, colorMax);
    Target[2] = CLAMP(Source[2], 0.0f, colorMax);
}

static uint8_t PackColorComponent(vg_lite_float_t value)
{
    /* Compute the rounded normalized value. */
    vg_lite_float_t rounded = value * 255.0f + 0.5f;

    /* Get the integer part. */
    int32_t roundedInt = (int32_t) rounded;

    /* Clamp to 0..1 range. */
    uint8_t clamped = (uint8_t) CLAMP(roundedInt, 0, 255);

    /* Return result. */
    return clamped;
}

static void _memset(void *p, unsigned char value, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        ((unsigned char *) p)[i] = value;
    }
}

static int has_valid_command_buffer(vg_lite_context_t *context)
{
    if (context == NULL)
    {
        return 0;
    }
    if (context->command_buffer_current >= CMDBUF_COUNT)
    {
        return 0;
    }
    if (context->command_buffer[context->command_buffer_current] == NULL)
    {
        return 0;
    }

    return 1;
}

#if DUMP_IMAGE
static void dump_img(void *memory, int width, int height, vg_lite_buffer_format_t format)
{
    FILE *fp;
    char imgname[255] = {'\0'};
    int i;
    static int num = 1;
    unsigned int *pt = (unsigned int *) memory;

    sprintf(imgname, "img_pid%d_%d.txt", getpid(), num++);

    fp = fopen(imgname, "w");

    if (fp == NULL)
    {
        printf("error!\n");
    }


    switch (format)
    {
    case VG_LITE_INDEX_1:
        for (i = 0; i < width * height / 32; ++i)
        {
            fprintf(fp, "0x%08x\n", pt[i]);
        }
        break;

    case VG_LITE_INDEX_2:
        for (i = 0; i < width * height / 16; ++i)
        {
            fprintf(fp, "0x%08x\n", pt[i]);
        }
        break;

    case VG_LITE_INDEX_4:
        for (i = 0; i < width * height / 8; ++i)
        {
            fprintf(fp, "0x%08x\n", pt[i]);
        }
        break;

    case VG_LITE_INDEX_8:
        for (i = 0; i < width * height / 4; ++i)
        {
            fprintf(fp, "0x%08x\n", pt[i]);
        }
        break;

    case VG_LITE_RGBA2222:
        for (i = 0; i < width * height / 4; ++i)
        {
            fprintf(fp, "0x%08x\n", pt[i]);
        }
        break;

    case VG_LITE_RGBA4444:
    case VG_LITE_BGRA4444:
    case VG_LITE_RGB565:
    case VG_LITE_BGR565:
        for (i = 0; i < width * height / 2; ++i)
        {
            fprintf(fp, "0x%08x\n", pt[i]);
        }
        break;

    case VG_LITE_RGBA8888:
    case VG_LITE_BGRA8888:
    case VG_LITE_RGBX8888:
    case VG_LITE_BGRX8888:
        for (i = 0; i < width * height; ++i)
        {
            fprintf(fp, "0x%08x\n", pt[i]);
        }
        break;

    default:
        break;
    }
    fclose(fp);
    fp = NULL;
}
#endif

/* Convert VGLite data format to HW value. */
static uint32_t convert_path_format(vg_lite_format_t format)
{
    switch (format)
    {
    case VG_LITE_S8:
        return 0;

    case VG_LITE_S16:
        return 0x100000;

    case VG_LITE_S32:
        return 0x200000;

    case VG_LITE_FP32:
        return 0x300000;

    default:
        return 0;
    }
}

/* Convert VGLite quality enums to HW values. */
static uint32_t convert_path_quality(vg_lite_quality_t quality)
{
    switch (quality)
    {
    case VG_LITE_HIGH:
        return 0x3;

    case VG_LITE_UPPER:
        return 0x2;

    case VG_LITE_MEDIUM:
        return 0x1;

    default:
        return 0x0;
    }
}

static uint32_t rgb_to_l(uint32_t color)
{
    uint32_t l = (uint32_t)((0.2126f * (vg_lite_float_t)(color & 0xFF)) +
                            (0.7152f * (vg_lite_float_t)((color >> 8) & 0xFF)) +
                            (0.0722f * (vg_lite_float_t)((color >> 16) & 0xFF)));
    return l | (l << 24);
}

/* Get the bpp information of a color format. */
static void get_format_bytes(vg_lite_buffer_format_t format,
                             uint32_t *mul,
                             uint32_t *div,
                             uint32_t *bytes_align)
{
    *mul = *div = 1;
    *bytes_align = 4;
    switch (format)
    {
    case VG_LITE_L8:
    case VG_LITE_A8:
    case VG_LITE_RGBA8888_ETC2_EAC:
        break;

    case VG_LITE_A4:
        *div = 2;
        break;

    case VG_LITE_ABGR1555:
    case VG_LITE_ARGB1555:
    case VG_LITE_BGRA5551:
    case VG_LITE_RGBA5551:
    case VG_LITE_RGBA4444:
    case VG_LITE_BGRA4444:
    case VG_LITE_ABGR4444:
    case VG_LITE_ARGB4444:
    case VG_LITE_RGB565:
    case VG_LITE_BGR565:
    case VG_LITE_YUYV:
    case VG_LITE_YUY2:
    case VG_LITE_YUY2_TILED:
    /* AYUY2 buffer memory = YUY2 + alpha. */
    case VG_LITE_AYUY2:
    case VG_LITE_AYUY2_TILED:
        *mul = 2;
        break;

    case VG_LITE_RGBA8888:
    case VG_LITE_BGRA8888:
    case VG_LITE_ABGR8888:
    case VG_LITE_ARGB8888:
    case VG_LITE_RGBX8888:
    case VG_LITE_BGRX8888:
    case VG_LITE_XBGR8888:
    case VG_LITE_XRGB8888:
        *mul = 4;
        break;

    case VG_LITE_NV12:
    case VG_LITE_NV12_TILED:
        *mul = 3;
        break;

    case VG_LITE_ANV12:
    case VG_LITE_ANV12_TILED:
        *mul = 4;
        break;

    case VG_LITE_INDEX_1:
        *div = 8;
        *bytes_align = 8;
        break;

    case VG_LITE_INDEX_2:
        *div = 4;
        *bytes_align = 8;
        break;

    case VG_LITE_INDEX_4:
        *div = 2;
        *bytes_align = 8;
        break;

    case VG_LITE_INDEX_8:
        *bytes_align = 1;
        break;

    case VG_LITE_RGBA2222:
    case VG_LITE_BGRA2222:
    case VG_LITE_ABGR2222:
    case VG_LITE_ARGB2222:
        *mul = 1;
        break;

    case VG_LITE_RGB888:
    case VG_LITE_BGR888:
    case VG_LITE_ABGR8565:
    case VG_LITE_BGRA5658:
    case VG_LITE_ARGB8565:
    case VG_LITE_RGBA5658:
        *mul = 3;
        break;

    default:
        break;
    }
}

/* Convert VGLite target color format to HW value. */
static uint32_t convert_target_format(vg_lite_buffer_format_t format, vg_lite_capabilities_t caps)
{
    switch (format)
    {
    case VG_LITE_A8:
        return 0x0;

    case VG_LITE_L8:
        return 0x6;

    case VG_LITE_ABGR4444:
        return 0x14;

    case VG_LITE_ARGB4444:
        return 0x34;

    case VG_LITE_RGBA4444:
        return 0x24;

    case VG_LITE_BGRA4444:
        return 0x4;

    case VG_LITE_RGB565:
        return 0x21;

    case VG_LITE_BGR565:
        return 0x1;

    case VG_LITE_ABGR8888:
        return 0x13;

    case VG_LITE_ARGB8888:
        return 0x33;

    case VG_LITE_RGBA8888:
        return 0x23;

    case VG_LITE_BGRA8888:
        return 0x3;

    case VG_LITE_RGBX8888:
        return 0x22;

    case VG_LITE_BGRX8888:
        return 0x2;

    case VG_LITE_XBGR8888:
        return 0x12;

    case VG_LITE_XRGB8888:
        return 0x32;

    case VG_LITE_ABGR1555:
        return 0x15;

    case VG_LITE_RGBA5551:
        return 0x25;

    case VG_LITE_ARGB1555:
        return 0x35;

    case VG_LITE_BGRA5551:
        return 0x5;

    case VG_LITE_YUYV:
    case VG_LITE_YUY2:
    case VG_LITE_YUY2_TILED:
        return 0x8;

    case VG_LITE_NV12:
    case VG_LITE_NV12_TILED:
        return 0xB;

    case VG_LITE_ANV12:
    case VG_LITE_ANV12_TILED:
        return 0xE;

    case VG_LITE_BGRA2222:
        return 0x7;

    case VG_LITE_RGBA2222:
        return 0x27;

    case VG_LITE_ABGR2222:
        return 0x17;

    case VG_LITE_ARGB2222:
        return 0x37;

    case VG_LITE_ARGB8565:
        return 0x3A;

    case VG_LITE_RGBA5658:
        return 0x2A;

    case VG_LITE_ABGR8565:
        return 0x1A;

    case VG_LITE_BGRA5658:
        return 0x0A;

    case VG_LITE_RGB888:
        return 0x29;

    case VG_LITE_BGR888:
        return 0x09;

    case VG_LITE_AYUY2:
    case VG_LITE_AYUY2_TILED:
    default:
        return 0xF;
    }
}

/* determine source IM is aligned by specified bytes */
static vg_lite_error_t _check_source_aligned(vg_lite_buffer_format_t format, uint32_t stride)
{
    switch (format)
    {
    case VG_LITE_A4:
    case VG_LITE_INDEX_1:
    case VG_LITE_INDEX_2:
    case VG_LITE_INDEX_4:
        FORMAT_ALIGNMENT(stride, 8);
        break;

    case VG_LITE_L8:
    case VG_LITE_A8:
    case VG_LITE_INDEX_8:
    case VG_LITE_RGBA2222:
    case VG_LITE_BGRA2222:
    case VG_LITE_ABGR2222:
    case VG_LITE_ARGB2222:
    case VG_LITE_RGBA8888_ETC2_EAC:
        FORMAT_ALIGNMENT(stride, 16);
        break;

    case VG_LITE_RGBA4444:
    case VG_LITE_BGRA4444:
    case VG_LITE_ABGR4444:
    case VG_LITE_ARGB4444:
    case VG_LITE_RGB565:
    case VG_LITE_BGR565:
    case VG_LITE_BGRA5551:
    case VG_LITE_RGBA5551:
    case VG_LITE_ABGR1555:
    case VG_LITE_ARGB1555:
    case VG_LITE_YUYV:
    case VG_LITE_YUY2:
    case VG_LITE_NV12:
    case VG_LITE_YV12:
    case VG_LITE_YV24:
    case VG_LITE_YV16:
    case VG_LITE_NV16:
        FORMAT_ALIGNMENT(stride, 32);
        break;

    case VG_LITE_RGB888:
    case VG_LITE_BGR888:
    case VG_LITE_ABGR8565:
    case VG_LITE_BGRA5658:
    case VG_LITE_ARGB8565:
    case VG_LITE_RGBA5658:
        FORMAT_ALIGNMENT(stride, 48);
        break;

    case VG_LITE_RGBA8888:
    case VG_LITE_BGRA8888:
    case VG_LITE_ABGR8888:
    case VG_LITE_ARGB8888:
    case VG_LITE_RGBX8888:
    case VG_LITE_BGRX8888:
    case VG_LITE_XBGR8888:
    case VG_LITE_XRGB8888:
        FORMAT_ALIGNMENT(stride, 64);
        break;

    default:
        return VG_LITE_SUCCESS;
    }
}

/* Convert VGLite source color format to HW values. */
static uint32_t convert_source_format(vg_lite_buffer_format_t format)
{
    switch (format)
    {
    case VG_LITE_L8:
        return 0x0;

    case VG_LITE_A4:
        return 0x1;

    case VG_LITE_A8:
        return 0x2;

    case VG_LITE_RGBA4444:
        return 0x23;

    case VG_LITE_BGRA4444:
        return 0x3;

    case VG_LITE_ABGR4444:
        return 0x13;

    case VG_LITE_ARGB4444:
        return 0x33;

    case VG_LITE_RGB565:
        return 0x25;

    case VG_LITE_BGR565:
        return 0x5;

    case VG_LITE_RGBA8888:
        return 0x27;

    case VG_LITE_BGRA8888:
        return 0x7;

    case VG_LITE_ABGR8888:
        return 0x17;

    case VG_LITE_ARGB8888:
        return 0x37;

    case VG_LITE_RGBX8888:
        return 0x26;

    case VG_LITE_BGRX8888:
        return 0x6;

    case VG_LITE_XBGR8888:
        return 0x16;

    case VG_LITE_XRGB8888:
        return 0x36;

    case VG_LITE_BGRA5551:
        return 0x4;

    case VG_LITE_RGBA5551:
        return 0x24;

    case VG_LITE_ABGR1555:
        return 0x14;

    case VG_LITE_ARGB1555:
        return 0x34;

    case VG_LITE_YUYV:
        return 0x8;

    case VG_LITE_YUY2:
    case VG_LITE_YUY2_TILED:
        return 0x8;

    case VG_LITE_NV12:
    case VG_LITE_NV12_TILED:
        return 0xB;

    case VG_LITE_ANV12:
    case VG_LITE_ANV12_TILED:
        return 0xE;

    case VG_LITE_YV12:
        return 0x9;

    case VG_LITE_YV24:
        return 0xD;

    case VG_LITE_YV16:
        return 0xC;

    case VG_LITE_NV16:
        return 0xA;

    case VG_LITE_AYUY2:
    case VG_LITE_AYUY2_TILED:
    default:
        return 0xF;

    case VG_LITE_INDEX_1:
        return 0x200;

    case VG_LITE_INDEX_2:
        return 0x400;

    case VG_LITE_INDEX_4:
        return 0x600;

    case VG_LITE_INDEX_8:
        return 0x800;

    case VG_LITE_RGBA2222:
        return 0xA20;

    case VG_LITE_BGRA2222:
        return 0xA00;

    case VG_LITE_ABGR2222:
        return 0xA10;

    case VG_LITE_ARGB2222:
        return 0xA30;

    case VG_LITE_RGBA8888_ETC2_EAC:
        return 0xE00;

    case VG_LITE_ARGB8565:
        return 0x40000030;

    case VG_LITE_RGBA5658:
        return 0x40000020;

    case VG_LITE_ABGR8565:
        return 0x40000010;

    case VG_LITE_BGRA5658:
        return 0x40000000;

    case VG_LITE_RGB888:
        return 0x20000020;

    case VG_LITE_BGR888:
        return 0x20000000;
    }
}

/* Convert VGLite blend modes to HW values. */
static uint32_t convert_blend(vg_lite_blend_t blend)
{
    switch (blend)
    {
    case VG_LITE_BLEND_SRC_OVER:
        return 0x00000100;

    case VG_LITE_BLEND_DST_OVER:
        return 0x00000200;

    case VG_LITE_BLEND_SRC_IN:
        return 0x00000300;

    case VG_LITE_BLEND_DST_IN:
        return 0x00000400;

    case VG_LITE_BLEND_SCREEN:
        return 0x00000600;

    case VG_LITE_BLEND_MULTIPLY:
        return 0x00000500;

    case VG_LITE_BLEND_ADDITIVE:
        return 0x00000900;

    case VG_LITE_BLEND_SUBTRACT:
        return 0x00000A00;

    default:
        return 0;
    }
}

/* Convert VGLite uv swizzle enums to HW values. */
static uint32_t convert_uv_swizzle(vg_lite_swizzle_t swizzle)
{
    switch (swizzle)
    {
    case VG_LITE_SWIZZLE_UV:
        return 0x00000040;
        break;

    case VG_LITE_SWIZZLE_VU:
        return 0x00000050;

    default:
        return 0;
        break;
    }
}

/* Convert VGLite yuv standard enums to HW values. */
static uint32_t convert_yuv2rgb(vg_lite_yuv2rgb_t yuv)
{
    switch (yuv)
    {
    case VG_LITE_YUV601:
        return 0;
        break;

    case VG_LITE_YUV709:
        return 0x00008000;

    default:
        return 0;
        break;
    }
}

/* Initialize the feature table of a chip. */
static vg_lite_error_t fill_feature_table(uint32_t *feature)
{
    uint16_t size = sizeof(VGFeatureInfos) / sizeof(VGFeatureInfos[0]);
    uint16_t i;
    uint32_t chip_id = 0, chip_rev = 0, cid = 0;

    /* Clear all bits. */
    _memset(feature, 0, sizeof(uint32_t) * gcFEATURE_COUNT);
    vg_lite_get_product_info(NULL, &chip_id, &chip_rev);
    vg_lite_get_register(0x30, &cid);

    for (i = 0; i < size; i++)
    {
        if ((VGFeatureInfos[i].chip_id == chip_id)
            && (VGFeatureInfos[i].chip_version == chip_rev)
            && (VGFeatureInfos[i].cid == cid)
           )
        {
            feature[gcFEATURE_BIT_VG_IM_INDEX_FORMAT] = VGFeatureInfos[i].vg_im_index_format;
            feature[gcFEATURE_BIT_VG_PE_PREMULTIPLY] = VGFeatureInfos[i].vg_pe_premultiply;
            feature[gcFEATURE_BIT_VG_SCISSOR] = VGFeatureInfos[i].vg_scissor;
            feature[gcFEATURE_BIT_VG_BORDER_CULLING] = VGFeatureInfos[i].vg_border_culling;
            feature[gcFEATURE_BIT_VG_RGBA2_FORMAT] = VGFeatureInfos[i].vg_rgba2_format;
            feature[gcFEATURE_BIT_VG_QUALITY_8X] = VGFeatureInfos[i].vg_quality_8x;
            feature[gcFEATURE_BIT_VG_IM_FASTCLAER] = VGFeatureInfos[i].vg_im_fastclear;
            feature[gcFEATURE_BIT_VG_RADIAL_GRADIENT] = VGFeatureInfos[i].vg_radial_gradient;
            feature[gcFEATURE_BIT_VG_GLOBAL_ALPHA] = VGFeatureInfos[i].vg_global_alpha;
            feature[gcFEATURE_BIT_VG_RGBA8_ETC2_EAC] = VGFeatureInfos[i].vg_rgba8_etc2_eac;
            feature[gcFEATURE_BIT_VG_COLOR_KEY] = VGFeatureInfos[i].vg_color_key;
            feature[gcFEATURE_BIT_VG_DOUBLE_IMAGE] = VGFeatureInfos[i].vg_double_image;
            feature[gcFEATURE_BIT_VG_YUV_OUTPUT] = VGFeatureInfos[i].vg_yuv_output;
            feature[gcFEATURE_BIT_VG_FLEXA] = VGFeatureInfos[i].vg_flexa;
            feature[gcFEATURE_BIT_VG_24BIT] = VGFeatureInfos[i].vg_24bit;
            feature[gcFEATURE_BIT_VG_DITHER] = VGFeatureInfos[i].vg_dither;
            feature[gcFEATURE_BIT_VG_YUV_INPUT] = VGFeatureInfos[i].vg_yuv_input;
            feature[gcFEATURE_BIT_VG_USE_DST] = VGFeatureInfos[i].vg_use_dst;
            feature[gcFEATURE_BIT_VG_PE_CLEAR] = VGFeatureInfos[i].vg_pe_clear;
            feature[gcFEATURE_BIT_VG_IM_INPUT] = VGFeatureInfos[i].vg_im_input;
            feature[gcFEATURE_BIT_VG_DEC_COMPRESS] = VGFeatureInfos[i].vg_dec_compress;
            feature[gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT] = VGFeatureInfos[i].vg_linear_gradient_ext;
            break;
        }
    }

    if (i == size)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }
    s_ftable.ftflag = 1;

    return VG_LITE_SUCCESS;
}

/* return value :if TURE : use , if FALSE: don't use */
static uint8_t _check_if_use_dst(vg_lite_buffer_format_t format, uint32_t right_x)
{
    if (vg_lite_query_feature(gcFEATURE_BIT_VG_USE_DST))
    {
        uint32_t mul, div, align, bpp;
        get_format_bytes(format, &mul, &div, &align);
        bpp = mul / div * 8;

        /* check if draw rectanle's tail has data that not aligned with 32 bits.*/
        if (bpp * right_x % 32)
        {
            return TURE;
        }
    }

    return FALSE;
}

static vg_lite_error_t submit(vg_lite_context_t *context);
static vg_lite_error_t stall(vg_lite_context_t *context, uint32_t time_ms, uint32_t mask);

/* Push a state array into current command buffer. */
static vg_lite_error_t push_states(vg_lite_context_t *context, uint32_t address, uint32_t count,
                                   uint32_t *data)
{
    uint32_t i;
    vg_lite_error_t error;
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    if (CMDBUF_OFFSET(*context) + 8 + VG_LITE_ALIGN(count + 1, 2) * 4 >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_STATES(count,
                                                                           address);

    for (i = 0; i < count; i++)
    {
        ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1 + i] = data[i];
    }
    if (i % 2 == 0)
    {
        ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1 + i] = VG_LITE_NOP();
    }

#if DUMP_COMMAND
    {
        uint32_t loops;
        if (strncmp(filename, "Commandbuffer", 13))
        {
            sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
        }

        fp = fopen(filename, "a");

        if (fp == NULL)
        {
            printf("error!\n");
        }

        fprintf(fp, "Command buffer: 0x%08x, ",
                ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0]);

        for (loops = 0; loops < count / 2; loops++)
        {
            fprintf(fp, "0x%08x,\nCommand buffer: 0x%08x, ",
                    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[(loops + 1) * 2 - 1],
                    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[(loops + 1) * 2]);
        }

        fprintf(fp, "0x%08x,\n",
                ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[(loops + 1) * 2 - 1]);

        fclose(fp);
        fp = NULL;
    }
#endif

    CMDBUF_OFFSET(*context) += VG_LITE_ALIGN(count + 1, 2) * 4;

    return VG_LITE_SUCCESS;
}

/* Push a single state command into the current command buffer. */
static vg_lite_error_t push_state(vg_lite_context_t *context, uint32_t address, uint32_t data)
{
    vg_lite_error_t error;
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    /* TODO wait for hw to complete development. */
    /*if(address == 0x0A1B || context->hw.hw_states[address & 0xff].state != data || !context->hw.hw_states[address & 0xff].init){ */
    if (CMDBUF_OFFSET(*context) + 16 >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    /*context->hw.hw_states[address & 0xff].state = data;
    context->hw.hw_states[address & 0xff].init = 1;*/

    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_STATE(address);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = data;

#if DUMP_COMMAND
    if (strncmp(filename, "Commandbuffer", 13))
    {
        sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
    }

    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        printf("error!\n");
    }

    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0],
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1]);

    fclose(fp);
    fp = NULL;
#endif

    CMDBUF_OFFSET(*context) += 8;
    /* } */
    return VG_LITE_SUCCESS;
}

/* Push a single state command with given address. */
static vg_lite_error_t push_state_ptr(vg_lite_context_t *context, uint32_t address, void *data_ptr)
{
    vg_lite_error_t error;
    uint32_t data = *(uint32_t *) data_ptr;
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    /* TODO wait for hw to complete development. */
    /* if(address == 0x0A1B || context->hw.hw_states[address & 0xff].state != data || !context->hw.hw_states[address & 0xff].init){ */
    if (CMDBUF_OFFSET(*context) + 16 >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    /*context->hw.hw_states[address & 0xff].state = data;
    context->hw.hw_states[address & 0xff].init = 1;*/

    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_STATE(address);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = data;

#if DUMP_COMMAND
    if (strncmp(filename, "Commandbuffer", 13))
    {
        sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
    }

    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        printf("error!\n");
    }
    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0],
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1]);

    fclose(fp);
    fp = NULL;
#endif

    CMDBUF_OFFSET(*context) += 8;
    /* } */
    return VG_LITE_SUCCESS;
}

/* Push a "call" command into the current command buffer. */
static vg_lite_error_t push_call(vg_lite_context_t *context, uint32_t address, uint32_t bytes)
{
    vg_lite_error_t error;
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    if (CMDBUF_OFFSET(*context) + 16 >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_CALL((
                                                                               bytes + 7) / 8);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = address;

#if DUMP_COMMAND
    if (strncmp(filename, "Commandbuffer", 13))
    {
        sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
    }

    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        printf("error!\n");
    }
    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0],
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1]);

    fclose(fp);
    fp = NULL;
#endif

    CMDBUF_OFFSET(*context) += 8;

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t push_pe_clear(vg_lite_context_t *context, uint32_t size)
{
    vg_lite_error_t error;
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    if (CMDBUF_OFFSET(*context) + 16 >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_DATA(1);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = 0;
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[2] = size;
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[3] = 0;

    CMDBUF_OFFSET(*context) += 16;

    return VG_LITE_SUCCESS;
}

/* Push a rectangle command into the current command buffer. */
static vg_lite_error_t push_rectangle(vg_lite_context_t *context, int x, int y, int width,
                                      int height)
{
    vg_lite_error_t error;
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    if (CMDBUF_OFFSET(*context) + 16 >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_DATA(1);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = 0;
    ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[4] = (uint16_t)x;
    ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[5] = (uint16_t)y;
    ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[6] = (uint16_t)width;
    ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[7] = (uint16_t)height;

#if DUMP_COMMAND
    if (strncmp(filename, "Commandbuffer", 13))
    {
        sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
    }

    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        printf("error!\n");
    }

    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0], 0);

    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[5] << 16 |
            ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[4],
            ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[7] << 16 |
            ((uint16_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[6]);

    fclose(fp);
    fp = NULL;
#endif

    CMDBUF_OFFSET(*context) += 16;

    return VG_LITE_SUCCESS;
}

/* Push a data array into the current command buffer. */
static vg_lite_error_t push_data(vg_lite_context_t *context, int size, void *data)
{
    vg_lite_error_t error;
    int bytes = VG_LITE_ALIGN(size, 8);

    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    if (CMDBUF_OFFSET(*context) + 16 + bytes >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    ((uint64_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[(bytes / 8)] = 0;
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_DATA(bytes / 8);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = 0;
    memcpy(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context) + 8, data, size);

#if DUMP_COMMAND
    {
        int loops;

        if (strncmp(filename, "Commandbuffer", 13))
        {
            sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
        }

        fp = fopen(filename, "a");

        if (fp == NULL)
        {
            printf("error!\n");
        }

        fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
                ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0], 0);
        for (loops = 0; loops < bytes / 8; loops++)
        {
            fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
                    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[(loops + 1) * 2],
                    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[(loops + 1) * 2 + 1]);
        }

        fclose(fp);
        fp = NULL;
    }
#endif

    CMDBUF_OFFSET(*context) += 8 + bytes;

    return VG_LITE_SUCCESS;
}

/* Push a "stall" command into the current command buffer. */
static vg_lite_error_t push_stall(vg_lite_context_t *context, uint32_t module)
{
    vg_lite_error_t error;
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    if (CMDBUF_OFFSET(*context) + 16 >= CMDBUF_SIZE(*context))
    {
        VG_LITE_RETURN_ERROR(submit(context));
        VG_LITE_RETURN_ERROR(stall(context, 0, (uint32_t)~0));
    }

    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_SEMAPHORE(module);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = 0;
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[2] = VG_LITE_STALL(module);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[3] = 0;

#if DUMP_COMMAND
    if (strncmp(filename, "Commandbuffer", 13))
    {
        sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
    }

    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        printf("error!\n");
    }

    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0], 0);
    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[2], 0);

    fclose(fp);
    fp = NULL;
#endif

    CMDBUF_OFFSET(*context) += 16;

    return VG_LITE_SUCCESS;
}

/* Submit the current command buffer to HW and reset the current command buffer offset. */
static vg_lite_error_t submit(vg_lite_context_t *context)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_submit_t submit;

    /* Check if there is a valid context and an allocated command buffer. */
    if (!has_valid_command_buffer(context))
    {
        return VG_LITE_NO_CONTEXT;
    }

    /* Check if there is anything to submit. */
    if (CMDBUF_OFFSET(*context) == 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Check if there is enough space in the command buffer for the END. */
    if (CMDBUF_OFFSET(*context) + 8 > CMDBUF_SIZE(*context))
    {
        /* Reset command buffer offset. */
        CMDBUF_OFFSET(*context) = 0;
        return VG_LITE_OUT_OF_RESOURCES;
    }

    /* Append END command into the command buffer. */
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0] = VG_LITE_END(EVENT_END);
    ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[1] = 0;

#if DUMP_COMMAND
    if (strncmp(filename, "Commandbuffer", 13))
    {
        sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
    }

    fp = fopen(filename, "a");

    if (fp == NULL)
    {
        printf("error!\n");
    }

    fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
            ((uint32_t *)(CMDBUF_BUFFER(*context) + CMDBUF_OFFSET(*context)))[0], 0);

    fprintf(fp, "Command buffer addr is : %p,\n", CMDBUF_BUFFER(*context));
    fprintf(fp, "Command buffer offset is : %d,\n", CMDBUF_OFFSET(*context) + 8);

    fclose(fp);
    fp = NULL;
#endif

    CMDBUF_OFFSET(*context) += 8;

    /* Submit the command buffer. */
    submit.context = &context->context;
    submit.commands = CMDBUF_BUFFER(*context);
    submit.command_size = CMDBUF_OFFSET(*context);
    submit.command_id = CMDBUF_INDEX(*context);

    /* Wait if GPU has not completed previous CMD buffer */
    if (submit_flag)
    {
        VG_LITE_RETURN_ERROR(stall(&s_context, 0, (uint32_t)~0));
    }
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_SUBMIT, &submit));

    submit_flag = 1;

    vglitemDUMP_BUFFER("command", (size_t)CMDBUF_BUFFER(*context),
                       submit.context->command_buffer_logical[CMDBUF_INDEX(*context)], 0, submit.command_size);
    vglitemDUMP("@[commit]");

    /* Reset command buffer. */
    CMDBUF_OFFSET(*context) = 0;

    return error;
}

/* Wait for the HW to finish the current execution. */
static vg_lite_error_t stall(vg_lite_context_t *context, uint32_t time_ms, uint32_t mask)
{
    vg_lite_error_t error;
    vg_lite_kernel_wait_t wait;

    vglitemDUMP("@[stall]");
    /* Wait until GPU is ready. */
    wait.context = &context->context;
    wait.timeout_ms = time_ms > 0 ? time_ms : VG_LITE_INFINITE;
    wait.event_mask = mask;
#if defined(_WINDLL)
    vg_lite_kernel(VG_LITE_WAIT, &wait);
#else
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_WAIT, &wait));
#endif

    submit_flag = 0;
    return VG_LITE_SUCCESS;
}

/* Get the inversion of a matrix. */
static int inverse(vg_lite_matrix_t *result, vg_lite_matrix_t *matrix)
{
    vg_lite_float_t det00, det01, det02;
    vg_lite_float_t d;
    int isAffine;

    /* Test for identity matrix. */
    if (matrix == NULL)
    {
        result->m[0][0] = 1.0f;
        result->m[0][1] = 0.0f;
        result->m[0][2] = 0.0f;
        result->m[1][0] = 0.0f;
        result->m[1][1] = 1.0f;
        result->m[1][2] = 0.0f;
        result->m[2][0] = 0.0f;
        result->m[2][1] = 0.0f;
        result->m[2][2] = 1.0f;

        /* Success. */
        return 1;
    }

    det00 = (matrix->m[1][1] * matrix->m[2][2]) - (matrix->m[2][1] * matrix->m[1][2]);
    det01 = (matrix->m[2][0] * matrix->m[1][2]) - (matrix->m[1][0] * matrix->m[2][2]);
    det02 = (matrix->m[1][0] * matrix->m[2][1]) - (matrix->m[2][0] * matrix->m[1][1]);

    /* Compute determinant. */
    d = (matrix->m[0][0] * det00) + (matrix->m[0][1] * det01) + (matrix->m[0][2] * det02);

    /* Return 0 if there is no inverse matrix. */
    if (d == 0.0f)
    {
        return 0;
    }

    /* Compute reciprocal. */
    d = 1.0f / d;

    /* Determine if the matrix is affine. */
    isAffine = (matrix->m[2][0] == 0.0f) && (matrix->m[2][1] == 0.0f) && (matrix->m[2][2] == 1.0f);

    result->m[0][0] = d * det00;
    result->m[0][1] = d * ((matrix->m[2][1] * matrix->m[0][2]) - (matrix->m[0][1] * matrix->m[2][2]));
    result->m[0][2] = d * ((matrix->m[0][1] * matrix->m[1][2]) - (matrix->m[1][1] * matrix->m[0][2]));
    result->m[1][0] = d * det01;
    result->m[1][1] = d * ((matrix->m[0][0] * matrix->m[2][2]) - (matrix->m[2][0] * matrix->m[0][2]));
    result->m[1][2] = d * ((matrix->m[1][0] * matrix->m[0][2]) - (matrix->m[0][0] * matrix->m[1][2]));
    result->m[2][0] = isAffine ? 0.0f : d * det02;
    result->m[2][1] = isAffine ? 0.0f : d * ((matrix->m[2][0] * matrix->m[0][1]) -
                                             (matrix->m[0][0] * matrix->m[2][1]));
    result->m[2][2] = isAffine ? 1.0f : d * ((matrix->m[0][0] * matrix->m[1][1]) -
                                             (matrix->m[1][0] * matrix->m[0][1]));

    /* Success. */
    return 1;
}

/* Transform a 2D point by a given matrix. */
static int transform(vg_lite_point_t *result, vg_lite_float_t x, vg_lite_float_t y,
                     vg_lite_matrix_t *matrix)
{
    vg_lite_float_t pt_x;
    vg_lite_float_t pt_y;
    vg_lite_float_t pt_w;

    /* Test for identity matrix. */
    if (matrix == NULL)
    {
        result->x = (int)x;
        result->y = (int)y;

        /* Success. */
        return 1;
    }

    /* Transform x, y, and w. */
    pt_x = (x * matrix->m[0][0]) + (y * matrix->m[0][1]) + matrix->m[0][2];
    pt_y = (x * matrix->m[1][0]) + (y * matrix->m[1][1]) + matrix->m[1][2];
    pt_w = (x * matrix->m[2][0]) + (y * matrix->m[2][1]) + matrix->m[2][2];

    if (pt_w <= 0.0f)
    {
        return 0;
    }

    /* Compute projected x and y. */
    result->x = (int)((pt_x / pt_w) + 0.5f);
    result->y = (int)((pt_y / pt_w) + 0.5f);

    /* Success. */
    return 1;
}

/*!
 Flush specific VG module.
 */
static vg_lite_error_t flush_target()
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_context_t *context = GET_CONTEXT();

    do
    {
        VG_LITE_BREAK_ERROR(push_state(context, 0x0A1B, 0x00000001));
        VG_LITE_BREAK_ERROR(push_stall(context, 7));
    }
    while (0);

    return error;
}

/****************** FAST_CLEAR feature implementation. ***************/
#if VG_TARGET_FAST_CLEAR
static vg_lite_error_t _free_fc_buffer(vg_lite_fc_buffer_t *buffer)
{
    vg_lite_error_t error;
    vg_lite_kernel_free_t free;

    if (buffer == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Make sure we have a valid memory handle. */
    if (buffer->handle)
    {
        /* Free the buffer. */
        free.memory_handle = buffer->handle;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free));

        /* Mark the buffer as freed. */
        buffer->handle = NULL;
        buffer->memory = NULL;
    }

    return VG_LITE_SUCCESS;
}
static vg_lite_error_t convert_color(vg_lite_buffer_format_t format, uint32_t value,
                                     uint32_t *result, int *bpp)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t r, g, b, a;
    int Bpp = 0;

    r = B(value);
    g = G(value);
    b = R(value);
    a = A(value);

    do
    {
        switch (format)
        {
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
    }
    while (0);

    if (bpp != NULL)
    {
        *bpp = Bpp;
    }

    if (Bpp == 16)
    {
        *result = ((*result) << 16) | (*result);
    }
    return error;
}

/* Fill Target buffer by FC buffer. Only used in cmodel/fpga for verification. */
#if defined(DEBUG) || defined(_DEBUG)
static vg_lite_error_t fill_fc_target(vg_lite_buffer_t *target, vg_lite_buffer_t *fcb)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint8_t *fc = (uint8_t *)fcb->memory;
    uint16_t *target16;
    uint32_t *target32;
    uint8_t *target8;
    uint32_t  clear32;
    int byte_done = 0;
    int i, j, k;
    int bpp;

    do
    {
        convert_color(target->format, target->fc_buffer[0].color, &clear32, &bpp);

        if (bpp == 32)
        {
            target32 = (uint32_t *)target->memory;
            for (i = 0; i < fcb->width; i++)
            {

                for (j = 0; j < 8; j++)     /* Loop the bits*/
                {

                    if (!(((*fc) >> j) & 1))
                    {
                        for (k = 0; k < 64 / 4; k++)
                        {
                            target32[k] = clear32;
                            byte_done += 4;
                            if (byte_done >= target->stride * target->height)
                            {
                                return error;
                            }
                        }
                    }

                    target32 += 64 / 4;
                }

                fc++;
            }
        }
        else if (bpp == 16)
        {
            target16 = (uint16_t *)target->memory;
            for (i = 0; i < fcb->width; i++)
            {

                for (j = 0; j < 8; j++)     /* Loop the bits*/
                {

                    if (!(((*fc) >> j) & 1))
                    {
                        for (k = 0; k < 64 / 2; k++)
                        {
                            target16[k] = (uint16_t)clear32;
                            byte_done += 2;
                            if (byte_done >= target->stride * target->height)
                            {
                                return error;
                            }
                        }
                    }

                    target16 += 64 / 2;
                }

                fc++;
            }
        }
        else if (bpp == 8)
        {
            target8 = (uint8_t *)target->memory;
            for (i = 0; i < fcb->width; i++)
            {

                for (j = 0; j < 8; j++)     /* Loop the bits*/
                {

                    if (!(((*fc) >> j) & 1))
                    {
                        for (k = 0; k < 64; k++)
                        {
                            target8[k] = (uint8_t)clear32;
                            byte_done++;
                            if (byte_done >= target->stride * target->height)
                            {
                                return error;
                            }
                        }
                    }

                    target8 += 64;
                }

                fc++;
            }
        }
    }
    while (0);

    return error;
}
#endif

/* Update the fast_clear buffer when render target switched. */
static vg_lite_error_t update_fc_buffer(vg_lite_buffer_t *target)
{
    int rt_bytes;
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_context_t *context = GET_CONTEXT();
    vg_lite_kernel_allocate_t allocate;

    do
    {
        if (target == NULL)
        {
            error = VG_LITE_INVALID_ARGUMENT;
            break;
        }

        rt_bytes = target->stride * target->height;
        rt_bytes = VG_LITE_ALIGN(rt_bytes, (FC_BIT_TO_BYTES * 2));
        rt_bytes = rt_bytes / FC_BIT_TO_BYTES / 2;
        /* Only allocate new buffer when the allocated is not big enough. Yes*/
        if (rt_bytes > target->fc_buffer[0].stride)
        {
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
        else
        {
            /* Just update the fc buffer size. */
            target->fc_buffer[0].width = rt_bytes;
        }
        memset(target->fc_buffer[0].memory, 0xff, target->fc_buffer[0].stride);
        VG_LITE_RETURN_ERROR(push_state(context, 0x0A9A,
                                        target->fc_buffer[0].address));   /* FC buffer address. */
    }
    while (0);

    return error;
}

/* Update FC registers and clear FC buffer. */
static vg_lite_error_t clear_fc(vg_lite_fc_buffer_t *buffer, uint32_t value)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_context_t *context = GET_CONTEXT();
    uint32_t bytes_to_clear;

    if (buffer == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }
    bytes_to_clear = buffer->stride / FC_BURST_BYTES;
    buffer->color = value;

    do
    {
        VG_LITE_BREAK_ERROR(push_state(context, 0x0A9B, value));                       /* FC clear value. */
        VG_LITE_BREAK_ERROR(push_state(context, 0x0AB0,
                                       0x80000000 | bytes_to_clear));   /* FC clear command. */
    }
    while (0);

    return error;
}

#if VG_TARGET_FC_DUMP
static int fc_buf_dump(vg_lite_buffer_t *target, vg_lite_buffer_t *fcb)
{
    int error = VG_LITE_SUCCESS;
    uint8_t *fc = (uint8_t *)fcb->memory;
    uint8_t *target8;
    int byte_done = 0;
    int target_bytes;
    int i, j;

    static unsigned s_cnt;
    unsigned cnt = s_cnt;

    FILE *fpFCBuf;
    FILE *fpTargetBuf;
    FILE *fpTargetBufInfo;
    char buf[256];

    s_cnt++;

    sprintf(buf, "vg255v2.fc_buf.f%04d.txt", cnt);
    fpFCBuf = fopen(buf, "wt");
    if (NULL == fpFCBuf)
    {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        return -1;
    }

    sprintf(buf, "vg255v2.target_buf_info.f%04d.txt", cnt);
    fpTargetBufInfo = fopen(buf, "wt");
    if (NULL == fpTargetBufInfo)
    {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        fclose(fpFCBuf);
        return -1;
    }
    else
    {
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "format", target->format);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "tiled",  target->tiled);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "width",  target->width);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "height", target->height);
        fprintf(fpTargetBufInfo, "%-12s: %d\n", "stride", target->stride);

        fclose(fpTargetBufInfo);
    }

    sprintf(buf, "vg255v2.target_buf.f%04d.txt", cnt);
    fpTargetBuf = fopen(buf, "wt");
    if (NULL == fpTargetBuf)
    {
        fprintf(stderr, "[Warning] Open file \'%s\' fail.\n", buf);
        fclose(fpFCBuf);
        return -1;
    }

    /* Dump FC buffer & Dump target buffer */
    target8 = (uint8_t *)target->memory;
    target_bytes = target->stride * target->height;

    for (i = 0; i < fcb->width; ++i)
    {
        fprintf(fpFCBuf, "%02x\n", fc[i]);
        /* 1 byte of fc related with 512 bytes of target buffer */
        for (j = 0; j < 128; ++j)
        {
            fprintf(fpTargetBuf, "%02x",   byte_done < target_bytes ? target8[0] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x",   byte_done < target_bytes ? target8[1] : 0);
            byte_done++;

            fprintf(fpTargetBuf, "%02x",   byte_done < target_bytes ? target8[2] : 0);
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

#endif

/* Set the current render target. */
static vg_lite_error_t set_render_target(vg_lite_buffer_t *target)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    int32_t tiled;
    int32_t stride;
    uint8_t flexa_mode = 0;
    uint32_t compress_mode = 0;

    if (target == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }
    /* Skip if render target and scissor are not changed. */
    if ((s_context.rtbuffer != NULL) &&
        !(memcmp(s_context.rtbuffer, target, sizeof(vg_lite_buffer_t))) &&
        (s_context.scissor_dirty == 0))
    {
        return VG_LITE_SUCCESS;
    }
    if ((target != NULL) &&
        (target->format == VG_LITE_YUY2 ||
         target->format == VG_LITE_AYUY2 ||
         target->format == VG_LITE_YUY2_TILED ||
         target->format == VG_LITE_AYUY2_TILED))
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_YUV_OUTPUT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }

#if VG_TARGET_FAST_CLEAR
    /* Flush target if necessary when switching. */
    if (s_context.rtbuffer->memory != NULL)      /* If it's not the first time to set target. */
    {
        vg_lite_finish();
    }
    update_fc_buffer(target);
#else
    if (s_context.rtbuffer->memory != NULL)
    {
        /* Flush the old target. */
        vg_lite_finish();
    }
#endif

    tiled = (target->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0;

    if (((target->format >= VG_LITE_YUY2) &&
         (target->format <= VG_LITE_AYUY2)) ||
        ((target->format >= VG_LITE_YUY2_TILED) &&
         (target->format <= VG_LITE_AYUY2_TILED)))
    {
        yuv2rgb = convert_yuv2rgb(target->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(target->yuv.swizzle);
    }

    /* Program render target. */
    if (s_context.rtbuffer != target || memcmp(s_context.rtbuffer, target, sizeof(vg_lite_buffer_t)) ||
        (s_context.flexa_dirty != 0))
    {
        VG_LITE_RETURN_ERROR(_check_compress_valid(target->format, target->compress_mode, target->tiled,
                                                   target->width, target->height));
        if (target->tiled == VG_LITE_TILED)
        {
            if ((target->stride % DEST_ALIGNMENT_LIMITATION) != 0)
            {
                return VG_LITE_INVALID_ARGUMENT;
            }
        }

        if (s_context.flexa_mode)
        {
            flexa_mode = 1 << 7;
        }

        compress_mode = (uint32_t)target->compress_mode << 25;

        if (!s_context.use_dst)
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A10,
                                            convert_target_format(target->format,
                                                                  s_context.capabilities) | 0x00010000 | uv_swiz | yuv2rgb | 0x100 | flexa_mode | compress_mode));
        }
        else
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A10,
                                            convert_target_format(target->format,
                                                                  s_context.capabilities) | 0x00110000 | uv_swiz | yuv2rgb | 0x100 | flexa_mode | compress_mode));
            s_context.use_dst = 0;
        }

        if (s_context.flexa_dirty  && !s_context.flexa_mode && s_context.tsbuffer.tessellation_buffer_size)
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AC8,
                                            s_context.tsbuffer.tessellation_buffer_size - 64));
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AC8, s_context.tsbuffer.tessellation_buffer_size));
            s_context.flexa_dirty = 0;
        }

        if (target->yuv.uv_planar)
        {
            /* Program uv plane address if necessary. */
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5C, target->yuv.uv_planar));
        }
        if (target->yuv.alpha_planar)
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5D, target->yuv.alpha_planar));
        }
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A11, target->address));
        /* 24bit format stride configured to 4bpp. */
        if (target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658)
        {
            stride = target->stride / 3 * 4;
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A12, stride | tiled));
        }
        else
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A12, target->stride | tiled));
        }
    }

    if (s_context.scissor_dirty != 0)
    {
        if (s_context.scissor_enabled)
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A13,
                                            s_context.scissor[2] | (s_context.scissor[3] << 16)));
        }
        else
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A13, target->width | (target->height << 16)));
        }
        s_context.scissor_dirty = 0;
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A13, target->width | (target->height << 16)));
    }

    memcpy(s_context.rtbuffer, target, sizeof(vg_lite_buffer_t));

    return error;
}

static vg_lite_error_t swap(float *a, float *b)
{
    float temp;
    if (a == NULL || b == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }
    temp = *a;
    *a = *b;
    *b = temp;
    return VG_LITE_SUCCESS;
}

/*************** API Functions ***********************************************/
vg_lite_error_t vg_lite_get_transform_matrix(vg_lite_point4_t src, vg_lite_point4_t dst,
                                             vg_lite_matrix_t *mat)
{
    float a[8][8], b[9], A[64];
    int i, j, k, m = 8, n = 1;
    int astep = 8, bstep = 1;
    float d;

    if (src == NULL || dst == NULL || mat == NULL)
    {
        return  VG_LITE_INVALID_ARGUMENT;
    }

    for (i = 0; i < 4; ++i)
    {
        a[i][0] = a[i + 4][3] = src[i].x;
        a[i][1] = a[i + 4][4] = src[i].y;
        a[i][2] = a[i + 4][5] = 1;
        a[i][3] = a[i][4] = a[i][5] =
                                a[i + 4][0] = a[i + 4][1] = a[i + 4][2] = 0;
        a[i][6] = -src[i].x * dst[i].x;
        a[i][7] = -src[i].y * dst[i].x;
        a[i + 4][6] = -src[i].x * dst[i].y;
        a[i + 4][7] = -src[i].y * dst[i].y;
        b[i] = dst[i].x;
        b[i + 4] = dst[i].y;
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
            if (ABS(A[j * astep + i]) > ABS(A[k * astep + i]))
            {
                k = j;
            }
        if (ABS(A[k * astep + i]) < EPS)
        {
            return VG_LITE_INVALID_ARGUMENT;
        }
        if (k != i)
        {
            for (j = i; j < m; j++)
            {
                swap(&A[i * astep + j], &A[k * astep + j]);
            }
            for (j = 0; j < n; j++)
            {
                swap(&b[i * bstep + j], &b[k * bstep + j]);
            }
        }
        d = -1 / A[i * astep + i];
        for (j = i + 1; j < m; j++)
        {
            float alpha = A[j * astep + i] * d;
            for (k = i + 1; k < m; k++)
            {
                A[j * astep + k] += alpha * A[i * astep + k];
            }
            for (k = 0; k < n; k++)
            {
                b[j * bstep + k] += alpha * b[i * bstep + k];
            }
        }
    }

    for (i = m - 1; i >= 0; i--)
        for (j = 0; j < n; j++)
        {
            float s = b[i * bstep + j];
            for (k = i + 1; k < m; k++)
            {
                s -= A[i * astep + k] * b[k * bstep + j];
            }
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

vg_lite_error_t vg_lite_clear(vg_lite_buffer_t *target,
                              vg_lite_rectangle_t *rectangle,
                              vg_lite_color_t color)
{
    vg_lite_error_t error;
    int32_t x, y, width, height;
    uint32_t color32;
    uint32_t tiled;

    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }

    /* Get rectangle. */
    x = (rectangle != NULL) ? rectangle->x : 0;
    y = (rectangle != NULL) ? rectangle->y : 0;
    width  = (rectangle != NULL) ? rectangle->width : s_context.rtbuffer->width;
    height = (rectangle != NULL) ? rectangle->height : s_context.rtbuffer->height;

    /* Compute the valid rectangle. */
    if (x < 0)
    {
        width += x;
        x = 0;
    }
    if (y < 0)
    {
        height += y;
        y = 0;
    }

    if (s_context.scissor_enabled)
    {
        int right, bottom;
        right = x + width;
        bottom = y + height;

        /* Bounds check. */
        if ((s_context.scissor[0] >= x + width) ||
            (s_context.scissor[2] <= x) ||
            (s_context.scissor[1] >= y + height) ||
            (s_context.scissor[3] <= y))
        {
            /* Do nothing. */
            return VG_LITE_SUCCESS;
        }
        /* Intersects the scissor and the rectangle. */
        x = (x > s_context.scissor[0] ? x : s_context.scissor[0]);
        y = (y > s_context.scissor[1] ? y : s_context.scissor[1]);
        right = (right < s_context.scissor[2]  ? right : s_context.scissor[2]);
        bottom = (bottom < s_context.scissor[3] ? bottom : s_context.scissor[3]);
        width = right - x;
        height = bottom - y;
    }

    /* Get converted color when target is in L8 format. */
    color32 = (target->format == VG_LITE_L8) ? rgb_to_l(color) : color;

    tiled = (target->tiled != VG_LITE_LINEAR) ? 0x40 : 0;
#if VG_TARGET_FAST_CLEAR
    if ((rectangle == NULL) ||
        ((x == 0) && (y == 0)  &&
         (width == s_context.rtbuffer->width) &&
         (height  == s_context.rtbuffer->height)))
    {
        convert_color(s_context.rtbuffer->format, color32, &color32, NULL);
        clear_fc(&target->fc_buffer[0], (uint32_t)color32);
    }
    else
#endif
    {
        /* Setup the command buffer. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color32));

        if (vg_lite_query_feature(gcFEATURE_BIT_VG_PE_CLEAR) && (!rectangle || (x == 0 && y == 0 &&
                                                                                width == target->width)))
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00, 0x10000004 | tiled));
            VG_LITE_RETURN_ERROR(push_pe_clear(&s_context, target->stride * height));
        }
        else
        {
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00, 0x10000001 | tiled));
            VG_LITE_RETURN_ERROR(push_rectangle(&s_context, x, y, width, height));
        }

        /* flush VGPE after clear */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00000001));
    }

    /* Success. */
    return VG_LITE_SUCCESS;
}
vg_lite_error_t vg_lite_blit2(vg_lite_buffer_t *target,
                              vg_lite_buffer_t *source0,
                              vg_lite_buffer_t *source1,
                              vg_lite_matrix_t *matrix0,
                              vg_lite_matrix_t *matrix1,
                              vg_lite_blend_t blend,
                              vg_lite_filter_t filter)
{
    vg_lite_error_t error;
    vg_lite_point_t point_min, point_max, temp;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[2][3];
    vg_lite_float_t y_step[2][3];
    vg_lite_float_t c_step[2][3];
    uint32_t imageMode;
    uint32_t blend_mode;
    int32_t stride0;
    int32_t stride1;
    uint32_t rotation = 0;
    uint32_t conversion = 0;
    uint32_t tiled0, tiled1;
    int left, right, bottom, top;
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_DOUBLE_IMAGE) ||
        !vg_lite_query_feature(gcFEATURE_BIT_VG_IM_INPUT))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if ((target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) ||
        (source0->format >= VG_LITE_RGB888 && source0->format <= VG_LITE_RGBA5658) ||
        (source1->format >= VG_LITE_RGB888 && source1->format <= VG_LITE_RGBA5658))
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }
    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }

    /* Check if the specified matrix has rotation or perspective. */
    if ((matrix0 != NULL)
        && ((matrix0->m[0][1] != 0.0f)
            || (matrix0->m[1][0] != 0.0f)
            || (matrix0->m[2][0] != 0.0f)
            || (matrix0->m[2][1] != 0.0f)
            || (matrix0->m[2][2] != 1.0f)
           )
       )
    {
        /* Mark that we have rotation. */
        rotation = 0x8000;
    }

    /* Check whether L8 is supported or not. */
    if ((target->format == VG_LITE_L8) && ((source0->format != VG_LITE_L8) &&
                                           (source0->format != VG_LITE_A8)))
    {
        conversion = 0x80000000;
    }

    /* Calculate transformation for Image0 (Paint) & Image1 (Image). */
    /* Image1. */
    /* Transform image (0,0) to screen. */
    if (!transform(&temp, 0.0f, 0.0f, matrix0))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Set initial point. */
    point_min = temp;
    point_max = temp;

    /* Transform image (0,height) to screen. */
    if (!transform(&temp, 0.0f, (vg_lite_float_t)source0->height, matrix0))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,height) to screen. */
    if (!transform(&temp, (vg_lite_float_t)source0->width, (vg_lite_float_t)source0->height, matrix0))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,0) to screen. */
    if (!transform(&temp, (vg_lite_float_t)source0->width, 0.0f, matrix0))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Clip to target. */
    if (s_context.scissor_enabled == 1)
    {
        left = s_context.scissor[0];
        top = s_context.scissor[1];
        right = s_context.scissor[2];
        bottom = s_context.scissor[3];
    }
    else
    {
        left = top = 0;
        right = target->width;
        bottom = target->height;
    }

    point_min.x = MAX(point_min.x, left);
    point_min.y = MAX(point_min.y, top);
    point_max.x = MIN(point_max.x, right);
    point_max.y = MIN(point_max.y, bottom);

    /* Compute inverse matrix. */
    if (!inverse(&inverse_matrix, matrix0))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Compute interpolation steps for image1 (Image). */
    x_step[1][0] = inverse_matrix.m[0][0] / source0->width;
    x_step[1][1] = inverse_matrix.m[1][0] / source0->height;
    x_step[1][2] = inverse_matrix.m[2][0];
    y_step[1][0] = inverse_matrix.m[0][1] / source0->width;
    y_step[1][1] = inverse_matrix.m[1][1] / source0->height;
    y_step[1][2] = inverse_matrix.m[2][1];
    c_step[1][0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) /
                   source0->width;
    c_step[1][1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) /
                   source0->height;
    c_step[1][2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];

    /* Image0 (Paint, as background ). */
    /* Transform image (0,0) to screen. */
    if (!transform(&temp, 0.0f, 0.0f, matrix1))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Set initial point. */
    point_min = temp;
    point_max = temp;

    /* Transform image (0,height) to screen. */
    if (!transform(&temp, 0.0f, (vg_lite_float_t)source1->height, matrix1))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,height) to screen. */
    if (!transform(&temp, (vg_lite_float_t)source1->width, (vg_lite_float_t)source1->height, matrix1))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,0) to screen. */
    if (!transform(&temp, (vg_lite_float_t)source1->width, 0.0f, matrix1))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Clip to target. */
    if (s_context.scissor_enabled == 1)
    {
        left = s_context.scissor[0];
        top = s_context.scissor[1];
        right = s_context.scissor[2];
        bottom = s_context.scissor[3];
    }
    else
    {
        left = top = 0;
        right = target->width;
        bottom = target->height;
    }

    point_min.x = MAX(point_min.x, left);
    point_min.y = MAX(point_min.y, top);
    point_max.x = MIN(point_max.x, right);
    point_max.y = MIN(point_max.y, bottom);

    /* Compute inverse matrix. */
    if (!inverse(&inverse_matrix, matrix1))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Compute interpolation steps for image1 (Image). */
    x_step[0][0] = inverse_matrix.m[0][0] / source1->width;
    x_step[0][1] = inverse_matrix.m[1][0] / source1->height;
    x_step[0][2] = inverse_matrix.m[2][0];
    y_step[0][0] = inverse_matrix.m[0][1] / source1->width;
    y_step[0][1] = inverse_matrix.m[1][1] / source1->height;
    y_step[0][2] = inverse_matrix.m[2][1];
    c_step[0][0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) /
                   source1->width;
    c_step[0][1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) /
                   source1->height;
    c_step[0][2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];

    /* DOUBLE_IMAGE mode. */
    imageMode = 0x5000;
    blend_mode = convert_blend(blend);
    tiled0 = (source0->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0;
    tiled1 = (source1->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0;

    /* Setup the command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x00000001 | imageMode | blend_mode | rotation));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));
    /* Program image1. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[1][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[1][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[1][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[1][2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[1][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[1][2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A25,
                                    convert_source_format(source0->format) | filter | conversion));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A27, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source0->address));

    if (source0->yuv.uv_planar != 0)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source0->yuv.uv_planar));
    }
    if (source0->yuv.v_planar != 0)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source0->yuv.v_planar));
    }

    if (source0->yuv.alpha_planar != 0)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source0->yuv.alpha_planar));
    }
    /* 24bit format stride configured to 4bpp. */
    if (source0->format >= VG_LITE_RGB888 && source0->format <= VG_LITE_RGBA5658)
    {
        stride0 = source0->stride / 3 * 4;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, stride0 | tiled0));
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, source0->stride | tiled0));
    }
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2D, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2F, source0->width | (source0->height << 16)));

    /* Program image0 (Paint, as background). */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A84, (void *) &c_step[0][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A85, (void *) &c_step[0][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A86, (void *) &c_step[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A7C, (void *) &x_step[0][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A7D, (void *) &x_step[0][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A7E, (void *) &x_step[0][2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A80, (void *) &y_step[0][0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A81, (void *) &y_step[0][1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A82, (void *) &y_step[0][2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24,
                                    convert_source_format(source1->format) | filter | conversion));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source1->address));
    if (source1->yuv.uv_planar != 0)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A50, source1->yuv.uv_planar));
    }
    if (source1->yuv.v_planar != 0)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A52, source1->yuv.v_planar));
    }
    if (source1->yuv.alpha_planar != 0)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A52, source1->yuv.alpha_planar));
    }
    /* 24bit format stride configured to 4bpp. */
    if (source1->format >= VG_LITE_RGB888 && source1->format <= VG_LITE_RGBA5658)
    {
        stride1 = source1->stride / 3 * 4;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, stride1 | tiled1));
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, source1->stride | tiled1));
    }
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source1->width | (source1->height << 16)));

    VG_LITE_RETURN_ERROR(push_rectangle(&s_context, point_min.x, point_min.y, point_max.x - point_min.x,
                                        point_max.y - point_min.y));
    flush_target();
    vglitemDUMP_BUFFER("image", (size_t)source0->address, source0->memory, 0,
                       (source0->stride) * (source0->height));
    vglitemDUMP_BUFFER("image", (size_t)source1->address, source1->memory, 0,
                       (source1->stride) * (source1->height));
#if DUMP_IMAGE
    dump_img(source0->memory, source0->width, source0->height, source0->format);
    dump_img(source1->memory, source1->width, source1->height, source1->format);
#endif

    return error;
}

vg_lite_error_t vg_lite_blit(vg_lite_buffer_t *target,
                             vg_lite_buffer_t *source,
                             vg_lite_matrix_t *matrix,
                             vg_lite_blend_t blend,
                             vg_lite_color_t color,
                             vg_lite_filter_t filter)
{
    vg_lite_error_t error;
    vg_lite_point_t point_min, point_max, temp;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    uint32_t imageMode;
    int32_t stride;
    uint32_t blend_mode = blend;
    uint32_t transparency_mode = 0;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    int left, top, right, bottom;
    uint32_t tiled;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    uint32_t im_fastclear_enable = 0;
    uint32_t ahb_read_split = 0;
    uint32_t compress_mode = 0;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_IM_INPUT))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (source->format == VG_LITE_RGBA8888_ETC2_EAC)
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_RGBA8_ETC2_EAC))
        {
            return VG_LITE_NOT_SUPPORT;
        }
        if (source->width % 16 || source->height % 4)
        {
            return VG_LITE_INVALID_ARGUMENT;
        }
    }
    if ((target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) ||
        (source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658))
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }

    if (blend && (target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 ||
                  target->format == VG_LITE_YUY2_TILED
                  || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    VG_LITE_RETURN_ERROR(_check_compress_valid(source->format, source->compress_mode, source->tiled,
                                               source->width, source->height));

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    /* Check if the specified matrix has rotation or perspective. */
    if ((matrix != NULL)
        && ((matrix->m[0][1] != 0.0f)
            || (matrix->m[1][0] != 0.0f)
            || (matrix->m[2][0] != 0.0f)
            || (matrix->m[2][1] != 0.0f)
            || (matrix->m[2][2] != 1.0f)
           )
        && (blend == VG_LITE_BLEND_NONE
            || blend == VG_LITE_BLEND_SRC_IN
            || blend == VG_LITE_BLEND_DST_IN
           )
       )
    {
        if (vg_lite_query_feature(gcFEATURE_BIT_VG_BORDER_CULLING))
        {
            /* Mark that we have rotation. */
            transparency_mode = 0x8000;
        }
        else
        {
            blend_mode = VG_LITE_BLEND_SRC_OVER;
        }

    }

    /* Check whether L8 is supported or not. */
    if ((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) &&
                                           (source->format != VG_LITE_A8)))
    {
        conversion = 0x80000000;
    }

    /* determine if source specify bytes are aligned */
    error = _check_source_aligned(source->format, source->stride);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }
    /* Transform image (0,0) to screen. */
    if (!transform(&temp, 0.0f, 0.0f, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Set initial point. */
    point_min = temp;
    point_max = temp;

    /* Transform image (0,height) to screen. */
    if (!transform(&temp, 0.0f, (vg_lite_float_t)source->height, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,height) to screen. */
    if (!transform(&temp, (vg_lite_float_t)source->width, (vg_lite_float_t)source->height, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,0) to screen. */
    if (!transform(&temp, (vg_lite_float_t)source->width, 0.0f, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Clip to target. */
    if (s_context.scissor_enabled)
    {
        left = s_context.scissor[0];
        top  = s_context.scissor[1];
        right = s_context.scissor[2];
        bottom = s_context.scissor[3];
    }
    else
    {
        left = top = 0;
        right = target->width;
        bottom = target->height;
    }

    point_min.x = MAX(point_min.x, left);
    point_min.y = MAX(point_min.y, top);
    point_max.x = MIN(point_max.x, right);
    point_max.y = MIN(point_max.y, bottom);

    if (blend)
    {
        s_context.use_dst = _check_if_use_dst(source->format, point_max.x);
    }

    /* No need to draw. */
    if ((point_max.x - point_min.x) <= 0 || (point_max.y - point_min.y) <= 0)
    {
        return VG_LITE_SUCCESS;
    }
    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }

    /* Compute inverse matrix. */
    if (!inverse(&inverse_matrix, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / source->width;
    x_step[1] = inverse_matrix.m[1][0] / source->height;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / source->width;
    y_step[1] = inverse_matrix.m[1][1] / source->height;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) /
                source->width;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) /
                source->height;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];

    /* Determine image mode (NORMAL, NONE or MULTIPLY) depending on the color. */
    imageMode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode ==
                                                                       VG_LITE_MULTIPLY_IMAGE_MODE) ? 0x00002000 : 0x00001000;
    blend_mode = convert_blend(blend_mode);
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;
    tiled = (target->tiled != VG_LITE_LINEAR) ? 0x40 : 0;
    if (blend_mode)
        /* The hw bit for improve read image buffer performance when enable alpha blending. */
    {
        ahb_read_split = 1 << 7;
    }

    compress_mode = (uint32_t)source->compress_mode << 25;

    /* Setup the command buffer. */
    if (vg_lite_query_feature(gcFEATURE_BIT_VG_GLOBAL_ALPHA))
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD1,
                                        s_context.dest_global_alpha_mode | s_context.dest_global_alpha_value
                                        | s_context.image_global_alpha_mode | s_context.image_global_alpha_value));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x00000001 | imageMode | blend_mode | transparency_mode | tiled));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if (((source->format >= VG_LITE_YUY2) &&
         (source->format <= VG_LITE_AYUY2)) ||
        ((source->format >= VG_LITE_YUY2_TILED) &&
         (source->format <= VG_LITE_AYUY2_TILED)))
    {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    if (source->fc_enable && vg_lite_query_feature(gcFEATURE_BIT_VG_IM_FASTCLAER))
    {
        im_fastclear_enable = (source->fc_enable == 0) ? 0 : 0x800000;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A25,
                                        convert_source_format(source->format) | filter | uv_swiz | yuv2rgb | conversion |
                                        im_fastclear_enable | ahb_read_split | compress_mode));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0ACF,
                                        source->fc_buffer[0].address));   /* FC buffer address. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD0,
                                        source->fc_buffer[0].color));     /* FC clear value. */
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A25,
                                        convert_source_format(source->format) | filter | uv_swiz | yuv2rgb | conversion | ahb_read_split |
                                        compress_mode));
    }
    if (source->yuv.uv_planar)
    {
        /* Program u plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
    }
    if (source->yuv.v_planar)
    {
        /* Program v plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
    }
    if (source->yuv.alpha_planar != 0)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.alpha_planar));
    }
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A27, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));
    /* 24bit format stride configured to 4bpp. */
    if (source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658)
    {
        stride = source->stride / 3 * 4;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, stride | tiled_source));
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, source->stride | tiled_source));
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2D, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2F, source->width | (source->height << 16)));
    VG_LITE_RETURN_ERROR(push_rectangle(&s_context, point_min.x, point_min.y, point_max.x - point_min.x,
                                        point_max.y - point_min.y));
    if (!s_context.flexa_mode)
    {
        error = flush_target();
    }
    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0,
                       (source->stride) * (source->height));

#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif

    return error;
}

vg_lite_error_t vg_lite_blit_rect(vg_lite_buffer_t *target,
                                  vg_lite_buffer_t *source,
                                  vg_lite_rectangle_t *rect,
                                  vg_lite_matrix_t *matrix,
                                  vg_lite_blend_t blend,
                                  vg_lite_color_t color,
                                  vg_lite_filter_t filter)
{
    vg_lite_error_t error;
    vg_lite_point_t point_min, point_max, temp;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    uint32_t imageMode;
    int32_t stride;
    uint32_t transparency_mode = 0;
    uint32_t blend_mode;
    vg_lite_blend_t forced_blending = blend;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    int left, top, right, bottom;
    uint32_t rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
    uint32_t tiled;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    uint32_t ahb_read_split = 0;
    uint32_t compress_mode;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_IM_INPUT))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (source->format == VG_LITE_RGBA8888_ETC2_EAC)
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_RGBA8_ETC2_EAC))
        {
            return VG_LITE_NOT_SUPPORT;
        }
        if (source->width % 16 || source->height % 4)
        {
            return VG_LITE_INVALID_ARGUMENT;
        }
    }
    if ((target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) ||
        (source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658))
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }

    if (blend && (target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 ||
                  target->format == VG_LITE_YUY2_TILED
                  || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    VG_LITE_RETURN_ERROR(_check_compress_valid(source->format, source->compress_mode, source->tiled,
                                               source->width, source->height));

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    /* Check if the specified matrix has rotation or perspective. */
    if ((matrix != NULL)
        && ((matrix->m[0][1] != 0.0f)
            || (matrix->m[1][0] != 0.0f)
            || (matrix->m[2][0] != 0.0f)
            || (matrix->m[2][1] != 0.0f)
            || (matrix->m[2][2] != 1.0f)
           )
        && (blend == VG_LITE_BLEND_NONE
            || blend == VG_LITE_BLEND_SRC_IN
            || blend == VG_LITE_BLEND_DST_IN
           )
       )
    {
        if (vg_lite_query_feature(gcFEATURE_BIT_VG_BORDER_CULLING))
        {
            /* Mark that we have rotation. */
            transparency_mode = 0x8000;
        }
        else
        {
            blend_mode = VG_LITE_BLEND_SRC_OVER;
        }

    }

    /* Check whether L8 is supported or not. */
    if ((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) &&
                                           (source->format != VG_LITE_A8)))
    {
        conversion = 0x80000000;
    }

    /* determine if source specify bytes are aligned */
    error = _check_source_aligned(source->format, source->stride);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }
    /* Set source region. */
    if (rect != NULL)
    {
        rect_x = rect->x;
        rect_y = rect->y;
        rect_w = rect->width;
        rect_h = rect->height;

        if ((rect_x > (uint32_t)source->width) || (rect_y > (uint32_t)source->height) ||
            (rect_w == 0) || (rect_h == 0))
        {
            /*No intersection*/
            return VG_LITE_INVALID_ARGUMENT;
        }

        if (rect_x + rect_w > (uint32_t)source->width)
        {
            rect_w = source->width - rect_x;
        }

        if (rect_y + rect_h > (uint32_t)source->height)
        {
            rect_h = source->height - rect_y;
        }
    }
    else
    {
        rect_x = rect_y = 0;
        rect_w = source->width;
        rect_h = source->height;
    }

    /* Transform image (0,0) to screen. */
    if (!transform(&temp, 0.0f, 0.0f, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Set initial point. */
    point_min = temp;
    point_max = temp;

    /* Transform image (0,height) to screen. */
    if (!transform(&temp, 0.0f, (vg_lite_float_t)rect_h, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,height) to screen. */
    if (!transform(&temp, (vg_lite_float_t)rect_w, (vg_lite_float_t)rect_h, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Transform image (width,0) to screen. */
    if (!transform(&temp, (vg_lite_float_t)rect_w, 0.0f, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Determine min/max. */
    if (temp.x < point_min.x) { point_min.x = temp.x; }
    if (temp.y < point_min.y) { point_min.y = temp.y; }
    if (temp.x > point_max.x) { point_max.x = temp.x; }
    if (temp.y > point_max.y) { point_max.y = temp.y; }

    /* Clip to target. */
    if (s_context.scissor_enabled)
    {
        left = s_context.scissor[0];
        top  = s_context.scissor[1];
        right = s_context.scissor[2];
        bottom = s_context.scissor[3];
    }
    else
    {
        left = top = 0;
        right = target->width;
        bottom = target->height;
    }

    point_min.x = MAX(point_min.x, left);
    point_min.y = MAX(point_min.y, top);
    point_max.x = MIN(point_max.x, right);
    point_max.y = MIN(point_max.y, bottom);

    if (blend)
    {
        s_context.use_dst = _check_if_use_dst(source->format, point_max.x);
    }

    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }

    /* Compute inverse matrix. */
    if (!inverse(&inverse_matrix, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / rect_w;
    x_step[1] = inverse_matrix.m[1][0] / rect_h;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / rect_w;
    y_step[1] = inverse_matrix.m[1][1] / rect_h;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) /
                rect_w;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) /
                rect_h;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];

    /* Determine image mode (NORMAL, NONE or MULTIPLY) depending on the color. */
    imageMode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode ==
                                                                       VG_LITE_MULTIPLY_IMAGE_MODE) ? 0x00002000 : 0x00001000;
    blend_mode = convert_blend(forced_blending);
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;
    tiled = (target->tiled != VG_LITE_LINEAR) ? 0x40 : 0;
    if (blend_mode)
        /* The hw bit for improve read image buffer performance when enable alpha blending. */
    {
        ahb_read_split = 1 << 7;
    }
    compress_mode = (uint32_t)source->compress_mode << 25;

    /* Setup the command buffer. */
    if (vg_lite_query_feature(gcFEATURE_BIT_VG_GLOBAL_ALPHA))
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD1,
                                        s_context.dest_global_alpha_mode | s_context.dest_global_alpha_value
                                        | s_context.image_global_alpha_mode | s_context.image_global_alpha_value));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x10000001 | imageMode | blend_mode | transparency_mode | tiled));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if (((source->format >= VG_LITE_YUY2) &&
         (source->format <= VG_LITE_AYUY2)) ||
        ((source->format >= VG_LITE_YUY2_TILED) &&
         (source->format <= VG_LITE_AYUY2_TILED)))
    {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    if (((source->format >= VG_LITE_YUY2) &&
         (source->format <= VG_LITE_AYUY2)) ||
        ((source->format >= VG_LITE_YUY2_TILED) &&
         (source->format <= VG_LITE_AYUY2_TILED)))
    {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A25,
                                    convert_source_format(source->format) | filter | uv_swiz | yuv2rgb | conversion | 0x01000100 |
                                    ahb_read_split | compress_mode));
    if (source->yuv.uv_planar)
    {
        /* Program u plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
    }
    if (source->yuv.v_planar)
    {
        /* Program v plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A27, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));
    /* 24bit format stride configured to 4bpp. */
    if (source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658)
    {
        stride = source->stride / 3 * 4;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, stride | tiled_source));
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, source->stride | tiled_source));
    }
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2D, rect_x | (rect_y << 16)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2F, rect_w | (rect_h << 16)));
    VG_LITE_RETURN_ERROR(push_rectangle(&s_context, point_min.x, point_min.y, point_max.x - point_min.x,
                                        point_max.y - point_min.y));
    if (!s_context.flexa_mode)
    {
        error = flush_target();
    }
    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0,
                       (source->stride) * (source->height));
#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif

    return error;
}

/* Program initial states for tessellation buffer. */
static vg_lite_error_t program_tessellation(vg_lite_context_t *context)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t tessellation_size, vg_count_buffer_size;

    tessellation_size =  context->tsbuffer.tessellation_buffer_size;
    vg_count_buffer_size = context->tsbuffer.vg_count_buffer_size;

    /* Program tessellation control: for TS module. */
    VG_LITE_RETURN_ERROR(push_state(context, 0x0A35, context->tsbuffer.tessellation_buffer_gpu));
    VG_LITE_RETURN_ERROR(push_state(context, 0x0AC8, tessellation_size));
    VG_LITE_RETURN_ERROR(push_state(context, 0x0ACB,
                                    context->tsbuffer.tessellation_buffer_gpu + tessellation_size));
    VG_LITE_RETURN_ERROR(push_state(context, 0x0ACC, vg_count_buffer_size));

    return error;
}

vg_lite_error_t vg_lite_init(int32_t tessellation_width,
                             int32_t tessellation_height)
{
    vg_lite_error_t error;
    vg_lite_kernel_initialize_t initialize;
    uint8_t i = 0;

    s_context.rtbuffer = (vg_lite_buffer_t *)malloc(sizeof(vg_lite_buffer_t));
    memset(s_context.rtbuffer, 0, sizeof(vg_lite_buffer_t));

    if (tessellation_width <= 0)
    {
        tessellation_width = 0;
        tessellation_height = 0;
    }
    if (tessellation_height <= 0)
    {
        tessellation_height = 0;
        tessellation_width = 0;
    }
    tessellation_width  = VG_LITE_ALIGN(tessellation_width, 16);

    /* Allocate a command buffer and a tessellation buffer. */
    initialize.command_buffer_size = command_buffer_size;
    initialize.tessellation_width = tessellation_width;
    initialize.tessellation_height = tessellation_height;
    initialize.context = &s_context.context;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_INITIALIZE, &initialize));

    /* Save draw context. */
    s_context.capabilities = initialize.capabilities;
    s_context.command_buffer[0] = (uint8_t *)initialize.command_buffer[0];
    s_context.command_buffer[1] = (uint8_t *)initialize.command_buffer[1];
    s_context.command_buffer_size = initialize.command_buffer_size;
    s_context.command_offset[0] = 0;
    s_context.command_offset[1] = 0;

    if ((tessellation_width  > 0) &&
        (tessellation_height > 0))
    {
        /* Set and Program Tessellation Buffer states. */
        s_context.tsbuffer.tessellation_buffer_gpu = initialize.tessellation_buffer_gpu;
        s_context.tsbuffer.tessellation_buffer_logic = initialize.tessellation_buffer_logic;
        s_context.tsbuffer.tessellation_width_height = initialize.tessellation_width_height;
        s_context.tsbuffer.tessellation_buffer_size = initialize.tessellation_buffer_size;
        s_context.tsbuffer.vg_count_buffer_size = initialize.vg_count_buffer_size;

        VG_LITE_RETURN_ERROR(program_tessellation(&s_context));
    }

    /* Fill feature table. */
    if (!s_ftable.ftflag)
    {
        VG_LITE_RETURN_ERROR(fill_feature_table(s_ftable.ftable));
    }

    /* Disable . */
    for (i = 0; i < 4; i++)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A90 + i, 0));
    }

    s_context.custom_ts_buf = 0;
    s_context.custom_cmd_buf = 0;

    /* Init scissor rect. */
    s_context.scissor[0] =
        s_context.scissor[1] =
            s_context.scissor[2] =
                s_context.scissor[3] = 0;
#if DUMP_CAPTURE
    _SetDumpFileInfo();
#endif
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_draw_path_type(vg_lite_path_t *path, vg_lite_draw_path_type_t path_type)
{
    if (!path || (path_type < VG_LITE_DRAW_FILL_PATH && path_type > VG_LITE_DRAW_STROKE_PATH + 1))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    path->path_type = path_type;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_draw(vg_lite_buffer_t *target,
                             vg_lite_path_t *path,
                             vg_lite_fill_t fill_rule,
                             vg_lite_matrix_t *matrix,
                             vg_lite_blend_t blend,
                             vg_lite_color_t color)
{
    uint32_t blend_mode;
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;
    vg_lite_error_t error;
    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int width, height;
    uint8_t ts_is_fullscreen = 0;
    uint32_t return_offset = 0;
    vg_lite_kernel_free_t free_memory;
    vg_lite_kernel_allocate_t memory;
    float new_matrix[6];
    float scale, bias;
    uint32_t tiled;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_QUALITY_8X) && path->quality == VG_LITE_UPPER)
    {
        return VG_LITE_NOT_SUPPORT;
    }
    if (target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658)
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }
    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }

    if ((target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 ||
         target->format == VG_LITE_YUY2_TILED
         || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED)
        && path->quality != VG_LITE_LOW)
    {
        path->quality = VG_LITE_LOW;
        //printf("If target is YUV group , the path qulity should use VG_LITE_LOW.\n");
    }

    width = target->width;
    height = target->height;

    if (width == 0 || height == 0)
    {
        return VG_LITE_NO_CONTEXT;
    }
    if ((target->width <= width) && (target->height <= height))
    {
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    if (ts_is_fullscreen == 0)
    {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        if (point_min.x < 0) { point_min.x = 0; }
        if (point_min.y < 0) { point_min.y = 0; }
        if (point_max.x > target->width) { point_max.x = target->width; }
        if (point_max.y > target->height) { point_max.y = target->height; }

        if (s_context.scissor_enabled)
        {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[3]);
        }
    }

    width = point_max.x - point_min.x;
    height = point_max.y - point_min.y;
    scale = 1.0f;
    bias = 0.0f;
    new_matrix[0] = matrix->m[0][0] * scale;
    new_matrix[1] = matrix->m[0][1] * scale;
    new_matrix[2] = (matrix->m[0][0] + matrix->m[0][1]) * bias + matrix->m[0][2];
    new_matrix[3] = matrix->m[1][0] * scale;
    new_matrix[4] = matrix->m[1][1] * scale;
    new_matrix[5] = (matrix->m[1][0] + matrix->m[1][1]) * bias + matrix->m[1][2];

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tsbuffer.tessellation_buffer_size;
    tiled = (target->tiled != VG_LITE_LINEAR) ? 0x40 : 0;

    /* Setup the command buffer. */
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x10000000 | s_context.capabilities.cap.tiled | blend_mode | tiled));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A02, color));
    /* Program tessellation control: for TS module. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *) &matrix->m[1][2]));

    /* DDRLess does not support uploading path data. */
    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        if (path->path_changed != 0)
        {
            if (path->uploaded.handle != NULL)
            {
                free_memory.memory_handle = path->uploaded.handle;
                VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free_memory));
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0,
                           path->uploaded.bytes);
    }
    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tsbuffer.tessellation_buffer_gpu,
                s_context.tsbuffer.tessellation_buffer_size);
    /* Tessellate path. */
    s_context.tsbuffer.tessellation_width_height = width | (height << 16);
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y << 16)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tsbuffer.tessellation_width_height));

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
    }
    else
    {
        if ((path->path_type & 0x1) == VG_LITE_DRAW_FILL_PATH)
        {
            push_data(&s_context, path->path_length, path->path);
        }
        if (path->path_type == VG_LITE_DRAW_STROKE_PATH ||
            path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH)
        {
            format = convert_path_format(VG_LITE_FP32);
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
            push_data(&s_context, path->stroke_path_size, path->stroke_path_data);
        }
    }

    return error;
}

vg_lite_error_t vg_lite_close(void)
{
    vg_lite_error_t error;
    vg_lite_kernel_terminate_t terminate;

    if (s_context.custom_cmd_buf)
    {
        vg_lite_kernel_unmap_memory_t unmap = {0};
        unmap.bytes = s_context.command_buffer_size * 2;
        unmap.logical = s_context.command_buffer[0];
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_UNMAP_MEMORY, &unmap));
    }

    if (s_context.custom_ts_buf)
    {
        vg_lite_kernel_unmap_memory_t unmap = {0};
        unmap.bytes = s_context.tsbuffer.tessellation_buffer_size + s_context.tsbuffer.vg_count_buffer_size;
        unmap.logical = s_context.tsbuffer.tessellation_buffer_logic;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_UNMAP_MEMORY, &unmap));
    }

    /* Termnate the draw context. */
    terminate.context = &s_context.context;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_TERMINATE, &terminate));

    if (s_context.rtbuffer)
    {
        free(s_context.rtbuffer);
    }

    submit_flag = 0;

    /* Reset the draw context. */
    _memset(&s_context, 0, sizeof(s_context));

#if DUMP_CAPTURE
    _SetDumpFileInfo();
#endif
    return VG_LITE_SUCCESS;
}

/* Handle tiled & yuv allocation. Currently including NV12, ANV12, YV12, YV16, NV16, YV24. */
static  vg_lite_error_t _allocate_tiled_yuv_planar(vg_lite_buffer_t *buffer)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t    yplane_size = 0;
    vg_lite_kernel_allocate_t allocate, uv_allocate, v_allocate;

    if ((buffer->format < VG_LITE_NV12) || (buffer->format > VG_LITE_ANV12_TILED)
        || (buffer->format == VG_LITE_AYUY2) || (buffer->format == VG_LITE_YUY2_TILED))
    {
        return error;
    }

    /* For NV12, there are 2 planes (Y, UV);
     For ANV12, there are 3 planes (Y, UV, Alpha).
     Each plane must be aligned by (4, 8).
     Then Y plane must be aligned by (8, 8).
     For YVxx, there are 3 planes (Y, U, V).
     YV12 is similar to NV12, both YUV420 format.
     YV16 and NV16 are YUV422 format.
     YV24 is YUV444 format.
     */
    buffer->width = VG_LITE_ALIGN(buffer->width, 8);
    buffer->height = VG_LITE_ALIGN(buffer->height, 8);
    buffer->stride = VG_LITE_ALIGN(buffer->width, 128);

    switch (buffer->format)
    {
    case VG_LITE_NV12:
    case VG_LITE_ANV12:
    case VG_LITE_NV12_TILED:
    case VG_LITE_ANV12_TILED:
        buffer->yuv.uv_stride = buffer->stride;
        buffer->yuv.alpha_stride = buffer->stride;
        buffer->yuv.uv_height = buffer->height / 2;
        break;

    case VG_LITE_NV16:
        buffer->yuv.uv_stride = buffer->stride;
        buffer->yuv.uv_height = buffer->height;
        break;

    case VG_LITE_YV12:
        buffer->yuv.uv_stride =
            buffer->yuv.v_stride = buffer->stride / 2;
        buffer->yuv.uv_height =
            buffer->yuv.v_height = buffer->height / 2;
        break;

    case VG_LITE_YV16:
        buffer->yuv.uv_stride =
            buffer->yuv.v_stride = buffer->stride;
        buffer->yuv.uv_height =
            buffer->yuv.v_height = buffer->height / 2;
        break;

    case VG_LITE_YV24:
        buffer->yuv.uv_stride =
            buffer->yuv.v_stride = buffer->stride;
        buffer->yuv.uv_height =
            buffer->yuv.v_height = buffer->height;
        break;

    default:
        return error;
    }

    yplane_size = buffer->stride * buffer->height;

    /* Allocate buffer memory: Y. */
    allocate.bytes = yplane_size;
    allocate.contiguous = 1;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &allocate));

    /* Save the allocation. */
    buffer->handle  = allocate.memory_handle;
    buffer->memory  = allocate.memory;
    buffer->address = allocate.memory_gpu;

    if ((buffer->format == VG_LITE_NV12) || (buffer->format == VG_LITE_ANV12)
        || (buffer->format == VG_LITE_NV16) || (buffer->format == VG_LITE_NV12_TILED)
        || (buffer->format == VG_LITE_ANV12_TILED))
    {
        /* Allocate buffer memory: UV. */
        uv_allocate.bytes = buffer->yuv.uv_stride * buffer->yuv.uv_height;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &uv_allocate));
        buffer->yuv.uv_handle = uv_allocate.memory_handle;
        buffer->yuv.uv_memory = uv_allocate.memory;
        buffer->yuv.uv_planar = uv_allocate.memory_gpu;

        if ((buffer->format == VG_LITE_ANV12) || (buffer->format == VG_LITE_ANV12_TILED))
        {
            uv_allocate.bytes = yplane_size;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &uv_allocate));
            buffer->yuv.alpha_planar = uv_allocate.memory_gpu;
        }
    }
    else
    {
        /* Allocate buffer memory: U, V. */
        uv_allocate.bytes = buffer->yuv.uv_stride * buffer->yuv.uv_height;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &uv_allocate));
        buffer->yuv.uv_handle = uv_allocate.memory_handle;
        buffer->yuv.uv_memory = uv_allocate.memory;
        buffer->yuv.uv_planar = uv_allocate.memory_gpu;

        v_allocate.bytes = buffer->yuv.v_stride * buffer->yuv.v_height;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &v_allocate));
        buffer->yuv.v_handle = v_allocate.memory_handle;
        buffer->yuv.v_memory = v_allocate.memory;
        buffer->yuv.v_planar = v_allocate.memory_gpu;
    }

    return error;
}

vg_lite_error_t vg_lite_allocate(vg_lite_buffer_t *buffer)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_allocate_t allocate;

    if (buffer->format == VG_LITE_RGBA8888_ETC2_EAC)
    {
        if (buffer->width % 16 || buffer->height % 4)
        {
            return VG_LITE_INVALID_ARGUMENT;
        }
    }

    /* Width should be 16-pixel aligned. */
    buffer->width = ((buffer->width + 15) & ~0xf);

    /* Reset planar. */
    buffer->yuv.uv_planar =
        buffer->yuv.v_planar =
            buffer->yuv.alpha_planar = 0;

    /* Align height in case format is tiled. */
    if (buffer->format >= VG_LITE_YUY2 && buffer->format <= VG_LITE_NV16)
    {
        buffer->height = VG_LITE_ALIGN(buffer->height, 4);
        buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
    }

    if (buffer->format >= VG_LITE_YUY2_TILED && buffer->format <= VG_LITE_AYUY2_TILED)
    {
        buffer->height = VG_LITE_ALIGN(buffer->height, 4);
        buffer->tiled = VG_LITE_TILED;
        buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
    }

    if ((buffer->format >= VG_LITE_NV12 && buffer->format <= VG_LITE_ANV12_TILED
         && buffer->format != VG_LITE_AYUY2 && buffer->format != VG_LITE_YUY2_TILED))
    {
        _allocate_tiled_yuv_planar(buffer);
    }
    else
    {
        /* Driver need compute the stride always with RT500 project. */
        uint32_t mul, div, align;
        vg_lite_float_t ratio = 1.0f;
        get_format_bytes(buffer->format, &mul, &div, &align);
        buffer->stride = VG_LITE_ALIGN((buffer->width * mul / div), align);

        /* Allocate the buffer. */
        if (buffer->compress_mode)
        {
            ratio = _calc_decnano_compress_ratio(buffer->format, buffer->compress_mode);
        }
        allocate.bytes = buffer->stride * buffer->height * ratio;
#if VG_TARGET_FAST_CLEAR
        allocate.bytes = VG_LITE_ALIGN(allocate.bytes, 64);
#endif
        allocate.contiguous = 1;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &allocate));

        /* Save the buffer allocation. */
        buffer->handle  = allocate.memory_handle;
        buffer->memory  = allocate.memory;
        buffer->address = allocate.memory_gpu;

        if ((buffer->format == VG_LITE_AYUY2) || (buffer->format == VG_LITE_AYUY2_TILED))
        {
            allocate.bytes = buffer->stride * buffer->height;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &allocate));
            buffer->yuv.alpha_planar = allocate.memory_gpu;
        }

    }

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_free(vg_lite_buffer_t *buffer)
{
    vg_lite_error_t error;
    vg_lite_kernel_free_t free, uv_free, v_free;

    if (buffer == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }
    if (!(memcmp(s_context.rtbuffer, buffer, sizeof(vg_lite_buffer_t))))
    {
        if (VG_LITE_SUCCESS == submit(&s_context))
        {
            VG_LITE_RETURN_ERROR(stall(&s_context, 0, ~0));
        }
        vglitemDUMP("@[swap 0x%08X %dx%d +%u]",
                    s_context.rtbuffer->address,
                    s_context.rtbuffer->width, s_context.rtbuffer->height,
                    s_context.rtbuffer->stride);
        vglitemDUMP_BUFFER(
            "framebuffer",
            (size_t)s_context.rtbuffer->address, s_context.rtbuffer->memory,
            0,
            s_context.rtbuffer->stride * (s_context.rtbuffer->height));

        memset(s_context.rtbuffer, 0, sizeof(vg_lite_buffer_t));
    }

    if (buffer->yuv.uv_planar)
    {
        /* Free UV(U) planar buffer. */
        uv_free.memory_handle = buffer->yuv.uv_handle;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &uv_free));

        /* Mark the buffer as freed. */
        buffer->yuv.uv_handle = NULL;
        buffer->yuv.uv_memory = NULL;
    }

    if (buffer->yuv.v_planar)
    {
        /* Free V planar buffer. */
        v_free.memory_handle = buffer->yuv.v_handle;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &v_free));

        /* Mark the buffer as freed. */
        buffer->yuv.v_handle = NULL;
        buffer->yuv.v_memory = NULL;
    }

#if VG_TARGET_FAST_CLEAR
    if (buffer->fc_buffer[0].handle != 0)
    {
#if VG_TARGET_FC_DUMP
        vglitemDUMP_BUFFER(
            "fcbuffer",
            buffer->fc_buffer[0].address, buffer->fc_buffer[0].memory,
            0,
            buffer->fc_buffer[0].stride * (buffer->fc_buffer[0].height));
#endif
        _free_fc_buffer(&buffer->fc_buffer[0]);
    }
#endif

    /* Make sure we have a valid memory handle. */
    if (buffer->handle == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Free the buffer. */
    free.memory_handle = buffer->handle;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free));

    /* Mark the buffer as freed. */
    buffer->handle = NULL;
    buffer->memory = NULL;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_map(vg_lite_buffer_t *buffer)
{
    vg_lite_error_t error;
    vg_lite_kernel_map_t map;

    /* We either need a logical or physical address. */
    if (buffer->memory == NULL && buffer->address == 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Check if we need to compute the stride. Usually map a pre-allocated memory, so the stride
       usually should be set*/
    if (buffer->stride == 0)
    {
        uint32_t mul, div, align;
        get_format_bytes(buffer->format, &mul, &div, &align);
        /* Compute the stride to be aligned. */
        buffer->stride = VG_LITE_ALIGN((buffer->width * mul / div), align);
    }

    /* Map the buffer. */
    map.bytes = buffer->stride * buffer->height;
    map.logical = buffer->memory;
    map.physical = buffer->address;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_MAP, &map));

    /* Save the buffer allocation. */
    buffer->handle  = map.memory_handle;
    buffer->address = map.memory_gpu;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_unmap(vg_lite_buffer_t *buffer)
{
    vg_lite_error_t error;
    vg_lite_kernel_unmap_t unmap;

    /* Make sure we have a valid memory handle. */
    if (buffer->handle == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Unmap the buffer. */
    unmap.memory_handle = buffer->handle;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_UNMAP, &unmap));

    /* Mark the buffer as freed. */
    buffer->handle = NULL;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_get_register(uint32_t address, uint32_t *result)
{
    vg_lite_error_t error;
    vg_lite_kernel_info_t data;

    /* Get input register address. */
    data.addr = address;

    /* Get register info. */
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_CHECK, &data));

    /* Return register info. */
    *result = data.reg;

    return VG_LITE_SUCCESS;
}

void vg_lite_get_info(vg_lite_info_t *info)
{
    if (info != NULL)
    {
        info->api_version = VGLITE_API_VERSION_2_0;
        info->header_version = VGLITE_HEADER_VERSION;
        info->release_version = VGLITE_RELEASE_VERSION;
        info->reserved = 0;
    }
}

uint32_t vg_lite_get_product_info(char *name, uint32_t *chip_id, uint32_t *chip_rev)
{
    const char *product_name = "GCNanoLiteV";
    uint32_t name_len;
    uint32_t rev = 0, id = 0;

    vg_lite_get_register(0x24, &rev);
    vg_lite_get_register(0x20, &id);

    name_len = strlen(product_name) + 1;
    if (name != NULL)
    {
        memcpy(name, product_name, name_len);
    }

    if (chip_id != NULL)
    {
        *chip_id = id;
    }

    if (chip_rev != NULL)
    {
        *chip_rev = rev;
    }
    return name_len;
}

uint32_t vg_lite_query_feature(vg_lite_feature_t feature)
{
    uint32_t result;

    if (feature < gcFEATURE_COUNT)
    {
        result = s_ftable.ftable[feature];
    }
    else
    {
        result = 0;
    }

    return result;
}

vg_lite_error_t vg_lite_finish()
{
    vg_lite_error_t  error;

    /* Return if there is nothing to submit. */
    if (CMDBUF_OFFSET(s_context) == 0)
    {
        if (submit_flag)
        {
            VG_LITE_RETURN_ERROR(stall(&s_context, 0, (uint32_t)~0));
        }
        return VG_LITE_SUCCESS;
    }

    /* Flush is moved from each draw to here. */
    VG_LITE_RETURN_ERROR(flush_target());
    VG_LITE_RETURN_ERROR(submit(&s_context));
    VG_LITE_RETURN_ERROR(stall(&s_context, 0, (uint32_t)~0));

#if VG_TARGET_FAST_CLEAR
    /*Only used in cmodel/fpga. In final SOC this SW FC decoder should be removed. */
    if (s_context.rtbuffer != NULL)
    {
#if VG_TARGET_FC_DUMP
        /*fc_buf_dump(s_context.rtbuffer, &s_context.fcBuffer); */
#endif /* VG_TARGET_FC_DUMP */
    }
#endif

    CMDBUF_SWAP(s_context);
    /* Reset command buffer. */
    CMDBUF_OFFSET(s_context) = 0;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_flush(void)
{
    vg_lite_error_t error;

    /* Return if there is nothing to submit. */
    if (CMDBUF_OFFSET(s_context) == 0)
    {
        return VG_LITE_SUCCESS;
    }

    /* Wait if GPU has not completed previous CMD buffer */
    if (submit_flag)
    {
        VG_LITE_RETURN_ERROR(stall(&s_context, 0, (uint32_t)~0));
    }

    /* Submit the current command buffer. */
    VG_LITE_RETURN_ERROR(flush_target());
    VG_LITE_RETURN_ERROR(submit(&s_context));
    CMDBUF_SWAP(s_context);
    /* Reset command buffer. */
    CMDBUF_OFFSET(s_context) = 0;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_init_arc_path(vg_lite_path_t *path,
                                      vg_lite_format_t data_format,
                                      vg_lite_quality_t quality,
                                      uint32_t path_length,
                                      void    *path_data,
                                      vg_lite_float_t min_x, vg_lite_float_t min_y,
                                      vg_lite_float_t max_x, vg_lite_float_t max_y)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t i = 0, command = 0, offset = 0;
    vg_lite_float_t moveToX, moveToY, lineToX, lineToY, controlX, controlY, quadToX, quadToY;
    vg_lite_float_t controlX1, controlY1, controlX2, controlY2, cubicToX, cubicToY;
    vg_lite_float_t horRadius, verRadius, rotAngle, endX, endY;
    float *pfloat, *fpath;
    char *cpath, *pathdata;
    vg_lite_control_coord_t coords;
    if (path == NULL || path_data == NULL || data_format != VG_LITE_FP32)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    memset(path, 0, sizeof(*path));
    memset(&coords, 0, sizeof(vg_lite_control_coord_t));
    pathdata = (char *)malloc(path_length);
    if (pathdata == NULL)
    {
        return VG_LITE_OUT_OF_MEMORY;
    }
    pfloat = (vg_lite_float_t *)path_data;
    while (i < path_length)
    {
        cpath = (char *)pfloat;
        command = (uint32_t) * cpath;
        pfloat++;
        switch (command)
        {
        case VLC_OP_END:
            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_END;
            offset += _commandSize_float[VLC_OP_END];
            i += _commandSize_float[VLC_OP_END];
            break;
        case VLC_OP_CLOSE:
            /* Update the control coordinates. */
            coords.lastX    = coords.startX;
            coords.lastY    = coords.startY;
            coords.controlX = coords.startX;
            coords.controlY = coords.startY;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_CLOSE;
            offset += _commandSize_float[VLC_OP_CLOSE];
            i += _commandSize_float[VLC_OP_CLOSE];
            break;
        case VLC_OP_MOVE:
            moveToX = *pfloat++;
            moveToY = *pfloat++;

            /* Update the control coordinates. */
            coords.startX   = moveToX;
            coords.startY   = moveToY;
            coords.lastX    = moveToX;
            coords.lastY    = moveToY;
            coords.controlX = moveToX;
            coords.controlY = moveToY;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_MOVE;
            fpath++;
            *fpath++ = moveToX;
            *fpath++ = moveToY;
            offset += _commandSize_float[VLC_OP_MOVE];
            i += _commandSize_float[VLC_OP_MOVE];
            break;
        case VLC_OP_MOVE_REL:
            moveToX = *pfloat++;
            moveToY = *pfloat++;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_MOVE_REL;
            fpath++;
            *fpath++ = moveToX;
            *fpath++ = moveToY;
            offset += _commandSize_float[VLC_OP_MOVE_REL];
            i += _commandSize_float[VLC_OP_MOVE_REL];

            /* Determine the absolute coordinates. */
            moveToX += coords.lastX;
            moveToY += coords.lastY;

            /* Update the control coordinates. */
            coords.startX   = moveToX;
            coords.startY   = moveToY;
            coords.lastX    = moveToX;
            coords.lastY    = moveToY;
            coords.controlX = moveToX;
            coords.controlY = moveToY;
            break;
        case VLC_OP_LINE:
            lineToX = *pfloat++;
            lineToY = *pfloat++;

            /* Update the control coordinates. */
            coords.lastX    = lineToX;
            coords.lastY    = lineToY;
            coords.controlX = lineToX;
            coords.controlY = lineToY;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_LINE;
            fpath++;
            *fpath++ = lineToX;
            *fpath++ = lineToY;
            offset += _commandSize_float[VLC_OP_LINE];
            i += _commandSize_float[VLC_OP_LINE];
            break;
        case VLC_OP_LINE_REL:
            lineToX = *pfloat++;
            lineToY = *pfloat++;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_LINE_REL;
            fpath++;
            *fpath++ = lineToX;
            *fpath++ = lineToY;
            offset += _commandSize_float[VLC_OP_LINE_REL];
            i += _commandSize_float[VLC_OP_LINE_REL];

            /* Determine the absolute coordinates. */
            lineToX += coords.lastX;
            lineToY += coords.lastY;

            /* Update the control coordinates. */
            coords.lastX    = lineToX;
            coords.lastY    = lineToY;
            coords.controlX = lineToX;
            coords.controlY = lineToY;
            break;
        case VLC_OP_QUAD:
            controlX = *pfloat++;
            controlY = *pfloat++;
            quadToX  = *pfloat++;
            quadToY  = *pfloat++;

            /* Update the control coordinates. */
            coords.lastX    = quadToX;
            coords.lastY    = quadToY;
            coords.controlX = controlX;
            coords.controlY = controlY;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_QUAD;
            fpath++;
            *fpath++ = controlX;
            *fpath++ = controlY;
            *fpath++ = quadToX;
            *fpath++ = quadToY;
            offset += _commandSize_float[VLC_OP_QUAD];
            i += _commandSize_float[VLC_OP_QUAD];
            break;
        case VLC_OP_QUAD_REL:
            controlX = *pfloat++;
            controlY = *pfloat++;
            quadToX  = *pfloat++;
            quadToY  = *pfloat++;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_QUAD_REL;
            fpath++;
            *fpath++ = controlX;
            *fpath++ = controlY;
            *fpath++ = quadToX;
            *fpath++ = quadToY;
            offset += _commandSize_float[VLC_OP_QUAD_REL];
            i += _commandSize_float[VLC_OP_QUAD_REL];

            /* Determine the absolute coordinates. */
            controlX += coords.lastX;
            controlY += coords.lastY;
            quadToX  += coords.lastX;
            quadToY  += coords.lastY;

            /* Update the control coordinates. */
            coords.lastX    = quadToX;
            coords.lastY    = quadToY;
            coords.controlX = controlX;
            coords.controlY = controlY;
            break;
        case VLC_OP_CUBIC:
            controlX1 = *pfloat++;
            controlY1 = *pfloat++;
            controlX2 = *pfloat++;
            controlY2 = *pfloat++;
            cubicToX  = *pfloat++;
            cubicToY  = *pfloat++;

            /* Update the control coordinates. */
            coords.lastX    = cubicToX;
            coords.lastY    = cubicToY;
            coords.controlX = controlX2;
            coords.controlY = controlY2;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_CUBIC;
            fpath++;
            *fpath++ = controlX1;
            *fpath++ = controlY1;
            *fpath++ = controlX2;
            *fpath++ = controlY2;
            *fpath++ = cubicToX;
            *fpath++ = cubicToY;
            offset += _commandSize_float[VLC_OP_CUBIC];
            i += _commandSize_float[VLC_OP_CUBIC];
            break;
        case VLC_OP_CUBIC_REL:
            controlX1 = *pfloat++;
            controlY1 = *pfloat++;
            controlX2 = *pfloat++;
            controlY2 = *pfloat++;
            cubicToX  = *pfloat++;
            cubicToY  = *pfloat++;

            cpath = (char *)pathdata + offset;
            fpath = (vg_lite_float_t *)cpath;
            *cpath = VLC_OP_CUBIC_REL;
            fpath++;
            *fpath++ = controlX1;
            *fpath++ = controlY1;
            *fpath++ = controlX2;
            *fpath++ = controlY2;
            *fpath++ = cubicToX;
            *fpath++ = cubicToY;
            offset += _commandSize_float[VLC_OP_CUBIC_REL];
            i += _commandSize_float[VLC_OP_CUBIC_REL];

            /* Determine the absolute coordinates. */
            controlX2 += coords.lastX;
            controlY2 += coords.lastY;
            cubicToX  += coords.lastX;
            cubicToY  += coords.lastY;

            /* Update the control coordinates. */
            coords.lastX    = cubicToX;
            coords.lastY    = cubicToY;
            coords.controlX = controlX2;
            coords.controlY = controlY2;
            break;
        case VLC_OP_SCCWARC:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, FALSE, FALSE, FALSE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        case VLC_OP_SCCWARC_REL:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC_REL];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, FALSE, FALSE, TURE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        case VLC_OP_SCWARC:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC_REL];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, TURE, FALSE, FALSE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        case VLC_OP_SCWARC_REL:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC_REL];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, TURE, FALSE, TURE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        case VLC_OP_LCCWARC:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC_REL];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, FALSE, TURE, FALSE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        case VLC_OP_LCCWARC_REL:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC_REL];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, FALSE, TURE, TURE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        case VLC_OP_LCWARC:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC_REL];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, TURE, TURE, FALSE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        case VLC_OP_LCWARC_REL:
            horRadius = *pfloat++;
            verRadius = *pfloat++;
            rotAngle = *pfloat++;
            endX = *pfloat++;
            endY = *pfloat++;
            i += _commandSize_float[VLC_OP_SCCWARC_REL];
            VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, TURE, TURE, TURE,
                                               &coords, (void *)&pathdata, &offset, path_length - i));
            break;
        default:
            break;
        }
    }

    path->format = data_format;
    path->quality = quality;
    path->bounding_box[0] = min_x;
    path->bounding_box[1] = min_y;
    path->bounding_box[2] = max_x;
    path->bounding_box[3] = max_y;

    path->path_length = offset;
    path->path = pathdata;
    path->pdata_internal = 1;
    path->path_changed = 1;
    path->uploaded.address = 0;
    path->uploaded.bytes = 0;
    path->uploaded.handle = NULL;
    path->uploaded.memory = NULL;

    return VG_LITE_SUCCESS;

ErrorHandler:
    free(pathdata);
    pathdata = NULL;
    return error;
}

vg_lite_error_t vg_lite_init_path(vg_lite_path_t *path,
                                  vg_lite_format_t data_format,
                                  vg_lite_quality_t quality,
                                  uint32_t path_length,
                                  void    *path_data,
                                  vg_lite_float_t min_x, vg_lite_float_t min_y,
                                  vg_lite_float_t max_x, vg_lite_float_t max_y)
{
    if (path == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    memset(path, 0, sizeof(*path));
    path->format = data_format;
    path->quality = quality;
    path->bounding_box[0] = min_x;
    path->bounding_box[1] = min_y;
    path->bounding_box[2] = max_x;
    path->bounding_box[3] = max_y;

    path->path_length = path_length;
    path->path         = path_data;

    path->path_changed = 1;
    path->uploaded.address = 0;
    path->uploaded.bytes = 0;
    path->uploaded.handle = NULL;
    path->uploaded.memory = NULL;
    path->pdata_internal = 0;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_clear_path(vg_lite_path_t *path)
{
    vg_lite_error_t error;
    if (path->uploaded.handle != NULL)
    {
        vg_lite_kernel_free_t free_cmd;
        free_cmd.memory_handle = path->uploaded.handle;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free_cmd));
    }

    path->uploaded.address = 0;
    path->uploaded.bytes = 0;
    path->uploaded.handle = NULL;
    path->uploaded.memory = NULL;
    if (path->pdata_internal == 1 && path->path != NULL)
    {
        free(path->path);
    }
    path->path = NULL;

    if (path->stroke_path_data)
    {
        free(path->stroke_path_data);
        path->stroke_path_data = NULL;

        if (path->stroke_conversion.path_point_list)
        {
            vg_lite_path_point_ptr temp_point;
            while (path->stroke_conversion.path_point_list)
            {
                temp_point = path->stroke_conversion.path_point_list->next;
                free(path->stroke_conversion.path_point_list);
                path->stroke_conversion.path_point_list = temp_point;
            }
            temp_point = NULL;
        }

        if (path->stroke_conversion.stroke_sub_path_list)
        {
            vg_lite_sub_path_ptr temp_sub_path;
            while (path->stroke_conversion.stroke_sub_path_list)
            {
                temp_sub_path = path->stroke_conversion.stroke_sub_path_list->next;
                if (path->stroke_conversion.stroke_sub_path_list->point_list)
                {
                    vg_lite_path_point_ptr temp_point;
                    while (path->stroke_conversion.stroke_sub_path_list->point_list)
                    {
                        temp_point = path->stroke_conversion.stroke_sub_path_list->point_list->next;
                        free(path->stroke_conversion.stroke_sub_path_list->point_list);
                        path->stroke_conversion.stroke_sub_path_list->point_list = temp_point;
                    }
                    temp_point = NULL;
                }
                free(path->stroke_conversion.stroke_sub_path_list);
                path->stroke_conversion.stroke_sub_path_list = temp_sub_path;
            }
            temp_sub_path = NULL;
        }
    }

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_CLUT(uint32_t count,
                                 uint32_t *colors)
{
    vg_lite_error_t error =  VG_LITE_SUCCESS;

    uint32_t addr = 0x0B00;

    if (!s_ftable.ftable[gcFEATURE_BIT_VG_IM_INDEX_FORMAT])
    {
        return VG_LITE_NOT_SUPPORT;
    }

    switch (count)
    {
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

    VG_LITE_RETURN_ERROR(push_states(&s_context, addr, count, colors));

    return error;
}

vg_lite_error_t vg_lite_draw_pattern(vg_lite_buffer_t *target,
                                     vg_lite_path_t *path,
                                     vg_lite_fill_t fill_rule,
                                     vg_lite_matrix_t *matrix0,
                                     vg_lite_buffer_t *source,
                                     vg_lite_matrix_t *matrix1,
                                     vg_lite_blend_t blend,
                                     vg_lite_pattern_mode_t pattern_mode,
                                     vg_lite_color_t  pattern_color,
                                     vg_lite_filter_t filter)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    uint32_t imageMode;
    uint32_t blend_mode;
    int32_t stride;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    vg_lite_matrix_t *matrix = matrix1;
    uint32_t pattern_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t tiled;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int width, height;
    uint8_t ts_is_fullscreen = 0;

    float new_matrix[6];
    float Scale, Bias;
    uint32_t ahb_read_split = 0;
    uint32_t compress_mode;

    if ((!vg_lite_query_feature(gcFEATURE_BIT_VG_QUALITY_8X) && path->quality == VG_LITE_UPPER) ||
        !vg_lite_query_feature(gcFEATURE_BIT_VG_IM_INPUT))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (source->format == VG_LITE_RGBA8888_ETC2_EAC)
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_RGBA8_ETC2_EAC))
        {
            return VG_LITE_NOT_SUPPORT;
        }
        if (source->width % 16 || source->height % 4)
        {
            return VG_LITE_INVALID_ARGUMENT;
        }
    }
    if ((target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658) ||
        (source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658))
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }

    VG_LITE_RETURN_ERROR(_check_compress_valid(source->format, source->compress_mode, source->tiled,
                                               source->width, source->height));

    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }

    if ((target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 ||
         target->format == VG_LITE_YUY2_TILED
         || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED)
        && path->quality != VG_LITE_LOW)
    {
        path->quality = VG_LITE_LOW;
        //printf("If target is YUV group , the path qulity should use VG_LITE_LOW.\n");
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = target->width;
    height = target->height;

    if (width == 0 || height == 0)
    {
        return VG_LITE_NO_CONTEXT;
    }
    if ((target->width <= width) && (target->height <= height))
    {
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if ((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) &&
                                           (source->format != VG_LITE_A8)))
    {
        conversion = 0x80000000;
    }

    /* Compute inverse matrix. */
    if (!inverse(&inverse_matrix, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / source->width;
    x_step[1] = inverse_matrix.m[1][0] / source->height;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / source->width;
    y_step[1] = inverse_matrix.m[1][1] / source->height;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) /
                source->width;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) /
                source->height;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];

    /* Determine image mode (NORMAL or MULTIPLY) depending on the color. */
    imageMode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode ==
                                                                       VG_LITE_MULTIPLY_IMAGE_MODE) ? 0x00002000 : 0x00001000;
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;
    compress_mode = (uint32_t)source->compress_mode << 25;

    if (pattern_mode == VG_LITE_PATTERN_COLOR)
    {
        uint8_t a, r, g, b;
        pattern_tile = 0;
        a = pattern_color >> 24;
        r = pattern_color >> 16;
        g = pattern_color >> 8;
        b = pattern_color;
        pattern_color = (a << 24) | (b << 16) | (g << 8) | r;
    }
    else if (pattern_mode == VG_LITE_PATTERN_PAD)
    {
        pattern_tile = 0x1000;
    }
    else
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Setup the command buffer. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if (((source->format >= VG_LITE_YUY2) &&
         (source->format <= VG_LITE_AYUY2)) ||
        ((source->format >= VG_LITE_YUY2_TILED) &&
         (source->format <= VG_LITE_AYUY2_TILED)))
    {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    if (blend)
        /* The hw bit for improve read image buffer performance when enable alpha blending. */
    {
        ahb_read_split = 1 << 7;
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A25,
                                    convert_source_format(source->format) | filter | pattern_tile | uv_swiz | yuv2rgb | conversion |
                                    0x01000100 | ahb_read_split | compress_mode));
    if (source->yuv.uv_planar)
    {
        /* Program u plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
    }
    if (source->yuv.v_planar)
    {
        /* Program v plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A27, pattern_color));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A29, source->address));
    /* 24bit format stride configured to 4bpp. */
    if (source->format >= VG_LITE_RGB888 && source->format <= VG_LITE_RGBA5658)
    {
        stride = source->stride / 3 * 4;
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, stride | tiled_source));
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2B, source->stride | tiled_source));
    }
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2D, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2F, source->width | (source->height << 16)));

    /* Work on path states. */
    matrix = matrix0;

    if (ts_is_fullscreen == 0)
    {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, target->width);
        point_max.y = MIN(point_max.y, target->height);

        if (s_context.scissor_enabled)
        {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[3]);
        }
    }

    width = point_max.x - point_min.x;
    height = point_max.y - point_min.y;
    Scale = 1.0f;
    Bias = 0.0f;
    new_matrix[0] = matrix->m[0][0] * Scale;
    new_matrix[1] = matrix->m[0][1] * Scale;
    new_matrix[2] = (matrix->m[0][0] + matrix->m[0][1]) * Bias + matrix->m[0][2];
    new_matrix[3] = matrix->m[1][0] * Scale;
    new_matrix[4] = matrix->m[1][1] * Scale;
    new_matrix[5] = (matrix->m[1][0] + matrix->m[1][1]) * Bias + matrix->m[1][2];

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tsbuffer.tessellation_buffer_size;
    tiled = (target->tiled != VG_LITE_LINEAR) ? 0x40 : 0;

    /* Setup the command buffer. */
    if (vg_lite_query_feature(gcFEATURE_BIT_VG_GLOBAL_ALPHA))
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AD1,
                                        s_context.dest_global_alpha_mode | s_context.dest_global_alpha_value
                                        | s_context.image_global_alpha_mode | s_context.image_global_alpha_value));
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x10000000 | s_context.capabilities.cap.tiled | 0x00000002 | imageMode | blend_mode |
                                    transparency_mode | tiled));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B,
                                    0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C,
                                    0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *) &matrix->m[1][2]));

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        if (path->path_changed != 0)
        {
            if (path->uploaded.handle != NULL)
            {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {

        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0,
                           path->uploaded.bytes);
    }

    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tsbuffer.tessellation_buffer_gpu,
                s_context.tsbuffer.tessellation_buffer_size);

    /* Tessellate path. */
    s_context.tsbuffer.tessellation_width_height = width | (height << 16);
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y << 16)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tsbuffer.tessellation_width_height));

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
    }
    else
    {
        if ((path->path_type & 0x1) == VG_LITE_DRAW_FILL_PATH)
        {
            push_data(&s_context, path->path_length, path->path);
        }
        if (path->path_type == VG_LITE_DRAW_STROKE_PATH ||
            path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH)
        {
            format = convert_path_format(VG_LITE_FP32);
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
            push_data(&s_context, path->stroke_path_size, path->stroke_path_data);
        }
    }

    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0,
                       (source->stride) * (source->height));
#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif
    return error;
}

vg_lite_error_t vg_lite_draw_linear_gradient(vg_lite_buffer_t *target,
                                             vg_lite_path_t *path,
                                             vg_lite_fill_t fill_rule,
                                             vg_lite_matrix_t *path_matrix,
                                             vg_lite_linear_gradient_ext_t *grad,
                                             vg_lite_color_t paint_color,
                                             vg_lite_blend_t blend,
                                             vg_lite_filter_t filter)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t image_mode;
    uint32_t blend_mode;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    vg_lite_buffer_t *source = &grad->image;
    vg_lite_matrix_t *matrix = &grad->matrix;
    uint32_t linear_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    void *data;

    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int width, height;
    uint8_t ts_is_fullscreen = 0;

    float new_matrix[6];
    float Scale, Bias;

    vg_lite_float_t dx, dy, dxdx_dydy;
    vg_lite_float_t lg_step_x_lin, lg_step_y_lin, lg_constant_lin;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_QUALITY_8X) && path->quality == VG_LITE_UPPER)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (source->format == VG_LITE_A4 || source->format == VG_LITE_A8)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658)
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }

    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }
    else if (error == VG_LITE_NO_CONTEXT)
    {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = s_context.tsbuffer.tessellation_width_height & 0xFFFF;
    height = s_context.tsbuffer.tessellation_width_height >> 16;

    if (width == 0 || height == 0)
    {
        return VG_LITE_NO_CONTEXT;
    }
    if ((target->width <= width) && (target->height <= height))
    {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if ((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) &&
                                           (source->format != VG_LITE_A8)))
    {
        conversion = 0x80000000;
    }

    /* Determine image mode (NORMAL or MULTIPLY) depending on the color. */
    image_mode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode ==
                                                                        VG_LITE_MULTIPLY_IMAGE_MODE) ? 0x00002000 : 0x00001000;
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;

    linear_tile = (grad->spread_mode == VG_LITE_RADIAL_GRADIENT_SPREAD_FILL) ? 0 :
                  (grad->spread_mode == VG_LITE_RADIAL_GRADIENT_SPREAD_PAD) ? 0x1000 :
                  (grad->spread_mode == VG_LITE_RADIAL_GRADIENT_SPREAD_REPEAT) ? 0x2000 : 0x3000;

    /* compute linear gradient paremeters */
    dx = grad->linear_gradient.X1 - grad->linear_gradient.X0;
    dy = grad->linear_gradient.Y1 - grad->linear_gradient.Y0;
    dxdx_dydy = dx * dx + dy * dy;

    /*
    **      dx (T(x) - x0) + dy (T(y) - y0)
    **  g = -------------------------------
    **                dx^2 + dy^2
    **
    **  where
    **
    **      dx := x1 - x0
    **      dy := y1 - y0
    **      T(x) := (x + 0.5) m00 + (y + 0.5) m01 + m02
    **            = x m00 + y m01 + 0.5 (m00 + m01) + m02
    **      T(y) := (x + 0.5) m10 + (y + 0.5) m11 + m12
    **            = x m10 + y m11 + 0.5 (m10 + m11) + m12.
    **
    **  We can factor the top line into:
    **
    **      = dx (x m00 + y m01 + 0.5 (m00 + m01) + m02 - x0)
    **      + dy (x m10 + y m11 + 0.5 (m10 + m11) + m12 - y0)
    **
    **      = x (dx m00 + dy m10)
    **      + y (dx m01 + dy m11)
    **      + dx (0.5 (m00 + m01) + m02 - x0)
    **      + dy (0.5 (m10 + m11) + m12 - y0).
    */

    lg_step_x_lin
        = (dx * MAT(matrix, 0, 0) + dy * MAT(matrix, 1, 0))
          / dxdx_dydy;

    lg_step_y_lin
        = (dx * MAT(matrix, 0, 1) + dy * MAT(matrix, 1, 1))
          / dxdx_dydy;

    lg_constant_lin =
        (
            (
                0.5f * (MAT(matrix, 0, 0) + MAT(matrix, 0, 1))
                + MAT(matrix, 0, 2) - grad->linear_gradient.X0
            ) * dx

            +

            (
                0.5f * (MAT(matrix, 1, 0) + MAT(matrix, 1, 1))
                + MAT(matrix, 1, 2) - grad->linear_gradient.Y0
            ) * dy
        )
        / dxdx_dydy;

    /* Setup the command buffer. */

    /* linear gradient parameters*/
    data = &lg_constant_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A04, *(uint32_t *) data));
    data = &lg_step_x_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A06, *(uint32_t *) data));
    data = &lg_step_y_lin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A08, *(uint32_t *) data));

    /* Compute inverse matrix. */
    if (!inverse(&inverse_matrix, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / source->width;
    x_step[1] = inverse_matrix.m[1][0] / source->height;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / source->width;
    y_step[1] = inverse_matrix.m[1][1] / source->height;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) /
                source->width;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) /
                source->height;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];

    /* Setup the command buffer. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if (((source->format >= VG_LITE_YUY2) &&
         (source->format <= VG_LITE_AYUY2)) ||
        ((source->format >= VG_LITE_YUY2_TILED) &&
         (source->format <= VG_LITE_AYUY2_TILED)))
    {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    if (source->yuv.uv_planar)
    {
        /* Program u plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
    }
    if (source->yuv.v_planar)
    {
        /* Program v plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24, convert_source_format(source->format) |
                                    filter | uv_swiz | yuv2rgb | linear_tile | conversion | 0x01000100));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, paint_color));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, tiled_source));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width));

    /* Work on path states. */
    matrix = path_matrix;

    if (ts_is_fullscreen == 0)
    {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, target->width);
        point_max.y = MIN(point_max.y, target->height);

        if (s_context.scissor_enabled)
        {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[0] + s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[1] + s_context.scissor[3]);
        }
    }

    Scale = 1.0f;
    Bias = 0.0f;
    new_matrix[0] = matrix->m[0][0] * Scale;
    new_matrix[1] = matrix->m[0][1] * Scale;
    new_matrix[2] = (matrix->m[0][0] + matrix->m[0][1]) * Bias + matrix->m[0][2];
    new_matrix[3] = matrix->m[1][0] * Scale;
    new_matrix[4] = matrix->m[1][1] * Scale;
    new_matrix[5] = (matrix->m[1][0] + matrix->m[1][1]) * Bias + matrix->m[1][2];

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tsbuffer.tessellation_buffer_size;

    /* Setup the command buffer. */
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x11000000 | s_context.capabilities.cap.tiled | 0x00000002 | image_mode | blend_mode |
                                    transparency_mode));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000400 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B,
                                    0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C,
                                    0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *) &matrix->m[1][2]));

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        if (path->path_changed != 0)
        {
            if (path->uploaded.handle != NULL)
            {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {

        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0,
                           path->uploaded.bytes);
    }

    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tsbuffer.tessellation_buffer_gpu,
                s_context.tsbuffer.tessellation_buffer_size);

    /* Tessellate path. */
    s_context.tsbuffer.tessellation_width_height = width | (height << 16);
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tsbuffer.tessellation_width_height));

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
#if  (DUMP_COMMAND && VG_COMMAND_CALL)
        if (strncmp(filename, "Commandbuffer", 13))
        {
            sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
        }

        fp = fopen(filename, "a");

        if (fp == NULL)
        {
            printf("error!\n");
        }

        fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
                ((uint32_t *) memory.memory)[0], 0);

        unsigned char *pt = (unsigned char *) memory.memory;

        for (int i = 8; i <= return_offset * 4 - 1; i = i + 4)
        {
            if (i % 8 == 0)
            {
                fprintf(fp, "Command buffer: ");
            }

            if (i % 4 == 0)
            {
                fprintf(fp, "0x");
            }

            for (int j = 3; j >= 0; --j)
            {
                fprintf(fp, "%02x", pt[i + j]);
            }

            if ((i / 4 + 1) % 2 == 0)
            {
                fprintf(fp, ",\n");
            }
            else
            {
                fprintf(fp, ", ");
            }
        }

        fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
                ((uint32_t *) memory.memory)[return_offset], 0);

        fclose(fp);
        fp = NULL;
#endif
    }
    else
    {
        if ((path->path_type & 0x1) == VG_LITE_DRAW_FILL_PATH)
        {
            push_data(&s_context, path->path_length, path->path);
        }
        if (path->path_type == VG_LITE_DRAW_STROKE_PATH ||
            path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH)
        {
            format = convert_path_format(VG_LITE_FP32);
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
            push_data(&s_context, path->stroke_path_size, path->stroke_path_data);
        }
    }

    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));

    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0,
                       (source->stride) * (source->height));
#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif
    return error;
}

vg_lite_error_t vg_lite_draw_radial_gradient(vg_lite_buffer_t *target,
                                             vg_lite_path_t *path,
                                             vg_lite_fill_t fill_rule,
                                             vg_lite_matrix_t *path_matrix,
                                             vg_lite_radial_gradient_t *grad,
                                             vg_lite_color_t paint_color,
                                             vg_lite_blend_t blend,
                                             vg_lite_filter_t filter)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t imageMode;
    uint32_t blend_mode;
    uint32_t conversion = 0;
    uint32_t tiled_source;
    vg_lite_matrix_t inverse_matrix;
    vg_lite_float_t x_step[3];
    vg_lite_float_t y_step[3];
    vg_lite_float_t c_step[3];
    vg_lite_buffer_t *source = &grad->image;
    vg_lite_matrix_t *matrix = &grad->matrix;
    uint32_t rad_tile = 0;
    uint32_t transparency_mode = 0;
    uint32_t yuv2rgb = 0;
    uint32_t uv_swiz = 0;
    void *data;
    uint32_t compress_mode;

    /* The following code is from "draw path" */
    uint32_t format, quality, tiling, fill;
    uint32_t tessellation_size;

    vg_lite_kernel_allocate_t memory;
    vg_lite_kernel_free_t free_memory;
    uint32_t return_offset = 0;

    vg_lite_point_t point_min = {0}, point_max = {0}, temp = {0};
    int width, height;
    uint8_t ts_is_fullscreen = 0;

    float new_matrix[6];
    float Scale, Bias;

    vg_lite_float_t radius;

    vg_lite_float_t centerX, centerY;
    vg_lite_float_t focalX, focalY;
    vg_lite_float_t fx, fy;
    vg_lite_float_t fxfy_2;
    vg_lite_float_t radius2;
    vg_lite_float_t r2_fx2, r2_fy2;
    vg_lite_float_t r2_fx2_2, r2_fy2_2;
    vg_lite_float_t r2_fx2_fy2;
    vg_lite_float_t r2_fx2_fy2sq;
    vg_lite_float_t cx, cy;

    vg_lite_float_t rgConstantLin, rgStepXLin, rgStepYLin;
    vg_lite_float_t rgConstantRad, rgStepXRad, rgStepYRad;
    vg_lite_float_t rgStepXXRad, rgStepYYRad, rgStepXYRad;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_RADIAL_GRADIENT) ||
        !vg_lite_query_feature(gcFEATURE_BIT_VG_IM_INPUT))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_QUALITY_8X) && path->quality == VG_LITE_UPPER)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    if (source->format == VG_LITE_A4 || source->format == VG_LITE_A8)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    VG_LITE_RETURN_ERROR(_check_compress_valid(source->format, source->compress_mode, source->tiled,
                                               source->width, source->height));

    radius = grad->radialGradient.r;
    if (radius <= 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    if (target->format >= VG_LITE_RGB888 && target->format <= VG_LITE_RGBA5658)
    {
        if (!vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT))
        {
            return VG_LITE_NOT_SUPPORT;
        }
    }

    error = set_render_target(target);
    if (error != VG_LITE_SUCCESS)
    {
        return error;
    }
    else if (error == VG_LITE_NO_CONTEXT)
    {
        /* If scissoring is enabled and no valid scissoring rectangles
           are present, no drawing occurs */
        return VG_LITE_SUCCESS;
    }

    if ((target->format == VG_LITE_YUYV || target->format == VG_LITE_YUY2 ||
         target->format == VG_LITE_YUY2_TILED
         || target->format == VG_LITE_AYUY2 || target->format == VG_LITE_AYUY2_TILED)
        && path->quality != VG_LITE_LOW)
    {
        path->quality = VG_LITE_LOW;
        //printf("If target is YUV group , the path qulity should use VG_LITE_LOW.\n");
    }

    transparency_mode = (source->transparency_mode == VG_LITE_IMAGE_TRANSPARENT ? 0x8000 : 0);
    width = s_context.tsbuffer.tessellation_width_height & 0xFFFF;
    height = s_context.tsbuffer.tessellation_width_height >> 16;

    if (width == 0 || height == 0)
    {
        return VG_LITE_NO_CONTEXT;
    }
    if ((target->width <= width) && (target->height <= height))
    {
        ts_is_fullscreen = 1;
        point_min.x = 0;
        point_min.y = 0;
        point_max.x = target->width;
        point_max.y = target->height;
    }

    /* If target is L8 and source is in YUV or RGB (not L8 or A8) then we have to convert RGB into L8. */
    if ((target->format == VG_LITE_L8) && ((source->format != VG_LITE_L8) &&
                                           (source->format != VG_LITE_A8)))
    {
        conversion = 0x80000000;
    }

    /* Determine image mode (NORMAL or MULTIPLY) depending on the color. */
    imageMode = (source->image_mode == VG_LITE_NONE_IMAGE_MODE) ? 0 : (source->image_mode ==
                                                                       VG_LITE_MULTIPLY_IMAGE_MODE) ? 0x00002000 : 0x00001000;
    tiled_source = (source->tiled != VG_LITE_LINEAR) ? 0x10000000 : 0 ;

    rad_tile = (grad->SpreadMode == VG_LITE_RADIAL_GRADIENT_SPREAD_FILL) ? 0 :
               (grad->SpreadMode == VG_LITE_RADIAL_GRADIENT_SPREAD_PAD) ? 0x1000 :
               (grad->SpreadMode == VG_LITE_RADIAL_GRADIENT_SPREAD_REPEAT) ? 0x2000 : 0x3000;
    compress_mode = (uint32_t)source->compress_mode << 25;

    /* compute radial gradient paremeters */

    /* Make shortcuts to the gradient information. */
    centerX = grad->radialGradient.cx;
    centerY = grad->radialGradient.cy;
    focalX  = grad->radialGradient.fx;
    focalY  = grad->radialGradient.fy;

    /* Compute constants of the equation. */
    fx           = focalX - centerX;
    fy           = focalY - centerY;
    radius2      = radius * radius;
    if (fx * fx + fy * fy > radius2)
    {
        /* If the focal point is outside the circle, let's move it
            to inside the circle. Per vg11 spec pg125 "If (fx, fy) lies outside ...
            For here, we set it at 0.9 ratio to the center.
        */
        vg_lite_float_t fr = (vg_lite_float_t)sqrt(fx * fx + fy * fy);
        fx = radius * fx / fr * 0.9f;
        fy = radius * fy / fr * 0.9f;
        focalX = grad->radialGradient.fx + fx;
        focalY = grad->radialGradient.fy + fy;
    }

    fxfy_2       = 2.0f * fx * fy;
    r2_fx2       = radius2 - fx * fx;
    r2_fy2       = radius2 - fy * fy;
    r2_fx2_2     = 2.0f * r2_fx2;
    r2_fy2_2     = 2.0f * r2_fy2;
    r2_fx2_fy2   = r2_fx2  - fy * fy;
    r2_fx2_fy2sq = r2_fx2_fy2 * r2_fx2_fy2;

    /*                        _____________________________________
    **      dx fx + dy fy + \/r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  g = -------------------------------------------------------
    **                         r^2 - fx^2 - fy^2
    **
    **  Where
    **
    **      dx := F(x) - focalX
    **      dy := F(y) - focalY
    **      fx := focalX - centerX
    **      fy := focalX - centerY
    **
    **  and
    **
    **      F(x) := (x + 0.5) m00 + (y + 0.5) m01 + m02
    **      F(y) := (x + 0.5) m10 + (y + 0.5) m11 + m12
    **
    **  So, dx can be factored into
    **
    **      dx = (x + 0.5) m00 + (y + 0.5) m01 + m02 - focalX
    **         = x m00 + y m01 + 0.5 m00 + 0.5 m01 + m02 - focalX
    **
    **         = x m00 + y m01 + cx
    **
    **  where
    **
    **      cx := 0.5 m00 + 0.5 m01 + m02 - focalX
    **
    **  The same way we can factor dy into
    **
    **      dy = x m10 + y m11 + cy
    **
    **  where
    **
    **      cy := 0.5 m10 + 0.5 m11 + m12 - focalY.
    **
    **  Now we can rewrite g as
    **                               ______________________________________
    **        dx fx + dy fy         / r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  g = ----------------- + \  /  -------------------------------------
    **      r^2 - fx^2 - fy^2    \/           (r^2 - fx^2 - fy^2)^2
    **               ____
    **    = gLin + \/gRad
    **
    **  where
    **
    **                dx fx + dy fy
    **      gLin := -----------------
    **              r^2 - fx^2 - fy^2
    **
    **              r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **      gRad := -------------------------------------
    **                      (r^2 - fx^2 - fy^2)^2
    */

    cx
        = 0.5f * (MAT(matrix, 0, 0) + MAT(matrix, 0, 1))
          + MAT(matrix, 0, 2)
          - focalX;

    cy
        = 0.5f * (MAT(matrix, 1, 0) + MAT(matrix, 1, 1))
          + MAT(matrix, 1, 2)
          - focalY;

    /*
    **            dx fx + dy fy
    **  gLin := -----------------
    **          r^2 - fx^2 - fy^2
    **
    **  We can factor the top half into
    **
    **      = (x m00 + y m01 + cx) fx + (x m10 + y m11 + cy) fy
    **
    **      = x (m00 fx + m10 fy)
    **      + y (m01 fx + m11 fy)
    **      + cx fx + cy fy.
    */

    rgStepXLin
        = (MAT(matrix, 0, 0) * fx + MAT(matrix, 1, 0) * fy)
          / r2_fx2_fy2;

    rgStepYLin
        = (MAT(matrix, 0, 1) * fx + MAT(matrix, 1, 1) * fy)
          / r2_fx2_fy2;

    rgConstantLin = (cx * fx  + cy * fy) / r2_fx2_fy2;

    /*
    **          r^2 (dx^2 + dy^2) - (dx fy - dy fx)^2
    **  gRad := -------------------------------------
    **                  (r^2 - fx^2 - fy^2)^2
    **
    **          r^2 (dx^2 + dy^2) - dx^2 fy^2 - dy^2 fx^2 + 2 dx dy fx fy
    **       := ---------------------------------------------------------
    **                            (r^2 - fx^2 - fy^2)^2
    **
    **          dx^2 (r^2 - fy^2) + dy^2 (r^2 - fx^2) + 2 dx dy fx fy
    **       := -----------------------------------------------------
    **                          (r^2 - fx^2 - fy^2)^2
    **
    **  First, lets factor dx^2 into
    **
    **      dx^2 = (x m00 + y m01 + cx)^2
    **           = x^2 m00^2 + y^2 m01^2 + 2 x y m00 m01
    **           + 2 x m00 cx + 2 y m01 cx + cx^2
    **
    **           = x^2 (m00^2)
    **           + y^2 (m01^2)
    **           + x y (2 m00 m01)
    **           + x (2 m00 cx)
    **           + y (2 m01 cx)
    **           + cx^2.
    **
    **  The same can be done for dy^2:
    **
    **      dy^2 = x^2 (m10^2)
    **           + y^2 (m11^2)
    **           + x y (2 m10 m11)
    **           + x (2 m10 cy)
    **           + y (2 m11 cy)
    **           + cy^2.
    **
    **  Let's also factor dx dy into
    **
    **      dx dy = (x m00 + y m01 + cx) (x m10 + y m11 + cy)
    **            = x^2 m00 m10 + y^2 m01 m11 + x y m00 m11 + x y m01 m10
    **            + x m00 cy + x m10 cx + y m01 cy + y m11 cx + cx cy
    **
    **            = x^2 (m00 m10)
    **            + y^2 (m01 m11)
    **            + x y (m00 m11 + m01 m10)
    **            + x (m00 cy + m10 cx)
    **            + y (m01 cy + m11 cx)
    **            + cx cy.
    **
    **  Now that we have all this, lets look at the top of gRad.
    **
    **      = dx^2 (r^2 - fy^2) + dy^2 (r^2 - fx^2) + 2 dx dy fx fy
    **      = x^2 m00^2 (r^2 - fy^2) + y^2 m01^2 (r^2 - fy^2)
    **      + x y 2 m00 m01 (r^2 - fy^2) + x 2 m00 cx (r^2 - fy^2)
    **      + y 2 m01 cx (r^2 - fy^2) + cx^2 (r^2 - fy^2)
    **      + x^2 m10^2 (r^2 - fx^2) + y^2 m11^2 (r^2 - fx^2)
    **      + x y 2 m10 m11 (r^2 - fx^2) + x 2 m10 cy (r^2 - fx^2)
    **      + y 2 m11 cy (r^2 - fx^2) + cy^2 (r^2 - fx^2)
    **      + x^2 m00 m10 2 fx fy + y^2 m01 m11 2 fx fy
    **      + x y (m00 m11 + m01 m10) 2 fx fy
    **      + x (m00 cy + m10 cx) 2 fx fy + y (m01 cy + m11 cx) 2 fx fy
    **      + cx cy 2 fx fy
    **
    **      = x^2 ( m00^2 (r^2 - fy^2)
    **            + m10^2 (r^2 - fx^2)
    **            + m00 m10 2 fx fy
    **            )
    **      + y^2 ( m01^2 (r^2 - fy^2)
    **            + m11^2 (r^2 - fx^2)
    **            + m01 m11 2 fx fy
    **            )
    **      + x y ( 2 m00 m01 (r^2 - fy^2)
    **            + 2 m10 m11 (r^2 - fx^2)
    **            + (m00 m11 + m01 m10) 2 fx fy
    **            )
    **      + x ( 2 m00 cx (r^2 - fy^2)
    **          + 2 m10 cy (r^2 - fx^2)
    **          + (m00 cy + m10 cx) 2 fx fy
    **          )
    **      + y ( 2 m01 cx (r^2 - fy^2)
    **          + 2 m11 cy (r^2 - fx^2)
    **          + (m01 cy + m11 cx) 2 fx fy
    **          )
    **      + cx^2 (r^2 - fy^2) + cy^2 (r^2 - fx^2) + cx cy 2 fx fy.
    */

    rgStepXXRad =
        (
            MAT(matrix, 0, 0) * MAT(matrix, 0, 0) * r2_fy2
            + MAT(matrix, 1, 0) * MAT(matrix, 1, 0) * r2_fx2
            + MAT(matrix, 0, 0) * MAT(matrix, 1, 0) * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepYYRad =
        (
            MAT(matrix, 0, 1) * MAT(matrix, 0, 1) * r2_fy2
            + MAT(matrix, 1, 1) * MAT(matrix, 1, 1) * r2_fx2
            + MAT(matrix, 0, 1) * MAT(matrix, 1, 1) * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepXYRad =
        (
            MAT(matrix, 0, 0) * MAT(matrix, 0, 1) * r2_fy2_2
            + MAT(matrix, 1, 0) * MAT(matrix, 1, 1) * r2_fx2_2
            + (
                MAT(matrix, 0, 0) * MAT(matrix, 1, 1)
                + MAT(matrix, 0, 1) * MAT(matrix, 1, 0)
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepXRad =
        (
            MAT(matrix, 0, 0) * cx * r2_fy2_2
            + MAT(matrix, 1, 0) * cy * r2_fx2_2
            + (
                MAT(matrix, 0, 0) * cy
                + MAT(matrix, 1, 0) * cx
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgStepYRad =
        (
            MAT(matrix, 0, 1) * cx * r2_fy2_2
            + MAT(matrix, 1, 1) * cy * r2_fx2_2
            + (
                MAT(matrix, 0, 1) * cy
                + MAT(matrix, 1, 1) * cx
            )
            * fxfy_2
        )
        / r2_fx2_fy2sq;

    rgConstantRad =
        (
            cx * cx * r2_fy2
            + cy * cy * r2_fx2
            + cx * cy * fxfy_2
        )
        / r2_fx2_fy2sq;

    /* Setup the command buffer. */

    /* rad gradient parameters*/
    data = &rgConstantLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A04, *(uint32_t *) data));
    data = &rgStepXLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A06, *(uint32_t *) data));
    data = &rgStepYLin;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A08, *(uint32_t *) data));
    data = &rgConstantRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A05, *(uint32_t *) data));
    data = &rgStepXRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A07, *(uint32_t *) data));
    data = &rgStepYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A09, *(uint32_t *) data));
    data = &rgStepXXRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A03, *(uint32_t *) data));
    data = &rgStepYYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0A, *(uint32_t *) data));
    data = &rgStepXYRad;
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A0B, *(uint32_t *) data));

    /* Compute inverse matrix. */
    if (!inverse(&inverse_matrix, matrix))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    /* Compute interpolation steps. */
    x_step[0] = inverse_matrix.m[0][0] / source->width;
    x_step[1] = inverse_matrix.m[1][0] / source->height;
    x_step[2] = inverse_matrix.m[2][0];
    y_step[0] = inverse_matrix.m[0][1] / source->width;
    y_step[1] = inverse_matrix.m[1][1] / source->height;
    y_step[2] = inverse_matrix.m[2][1];
    c_step[0] = (0.5f * (inverse_matrix.m[0][0] + inverse_matrix.m[0][1]) + inverse_matrix.m[0][2]) /
                source->width;
    c_step[1] = (0.5f * (inverse_matrix.m[1][0] + inverse_matrix.m[1][1]) + inverse_matrix.m[1][2]) /
                source->height;
    c_step[2] = 0.5f * (inverse_matrix.m[2][0] + inverse_matrix.m[2][1]) + inverse_matrix.m[2][2];

    /* Setup the command buffer. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A18, (void *) &c_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A19, (void *) &c_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1A, (void *) &c_step[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1C, (void *) &x_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1D, (void *) &x_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A1E, (void *) &x_step[2]));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1F, 0x00000001));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A20, (void *) &y_step[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A21, (void *) &y_step[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A22, (void *) &y_step[2]));

    if (((source->format >= VG_LITE_YUY2) &&
         (source->format <= VG_LITE_AYUY2)) ||
        ((source->format >= VG_LITE_YUY2_TILED) &&
         (source->format <= VG_LITE_AYUY2_TILED)))
    {
        yuv2rgb = convert_yuv2rgb(source->yuv.yuv2rgb);
        uv_swiz = convert_uv_swizzle(source->yuv.swizzle);
    }

    if (source->yuv.uv_planar)
    {
        /* Program u plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A51, source->yuv.uv_planar));
    }
    if (source->yuv.v_planar)
    {
        /* Program v plane address if necessary. */
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A53, source->yuv.v_planar));
    }

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A24, convert_source_format(source->format) |
                                    filter | uv_swiz | yuv2rgb | rad_tile | conversion | 0x01000100 | compress_mode));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A26, paint_color));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A28, source->address));

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2A, tiled_source));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2C, 0));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A2E, source->width));

    /* Work on path states. */
    matrix = path_matrix;

    if (ts_is_fullscreen == 0)
    {
        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        point_min = point_max = temp;

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[1],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[2], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        transform(&temp, (vg_lite_float_t)path->bounding_box[0], (vg_lite_float_t)path->bounding_box[3],
                  matrix);
        if (temp.x < point_min.x) { point_min.x = temp.x; }
        if (temp.y < point_min.y) { point_min.y = temp.y; }
        if (temp.x > point_max.x) { point_max.x = temp.x; }
        if (temp.y > point_max.y) { point_max.y = temp.y; }

        point_min.x = MAX(point_min.x, 0);
        point_min.y = MAX(point_min.y, 0);
        point_max.x = MIN(point_max.x, target->width);
        point_max.y = MIN(point_max.y, target->height);

        if (s_context.scissor_enabled)
        {
            point_min.x = MAX(point_min.x, s_context.scissor[0]);
            point_min.y = MAX(point_min.y, s_context.scissor[1]);
            point_max.x = MIN(point_max.x, s_context.scissor[0] + s_context.scissor[2]);
            point_max.y = MIN(point_max.y, s_context.scissor[1] + s_context.scissor[3]);
        }
    }

    Scale = 1.0f;
    Bias = 0.0f;
    new_matrix[0] = matrix->m[0][0] * Scale;
    new_matrix[1] = matrix->m[0][1] * Scale;
    new_matrix[2] = (matrix->m[0][0] + matrix->m[0][1]) * Bias + matrix->m[0][2];
    new_matrix[3] = matrix->m[1][0] * Scale;
    new_matrix[4] = matrix->m[1][1] * Scale;
    new_matrix[5] = (matrix->m[1][0] + matrix->m[1][1]) * Bias + matrix->m[1][2];

    /* Convert states into hardware values. */
    blend_mode = convert_blend(blend);
    format = convert_path_format(path->format);
    quality = convert_path_quality(path->quality);
    tiling = (s_context.capabilities.cap.tiled == 2) ? 0x2000000 : 0;
    fill = (fill_rule == VG_LITE_FILL_EVEN_ODD) ? 0x10 : 0;
    tessellation_size = s_context.tsbuffer.tessellation_buffer_size;

    /* Setup the command buffer. */
    /* Program color register. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A00,
                                    0x12000000 | s_context.capabilities.cap.tiled | 0x00000002 | imageMode | blend_mode |
                                    transparency_mode));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000000 | format | quality | tiling | fill));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3B,
                                    0x3F800000));      /* Path tessellation SCALE. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3C,
                                    0x00000000));      /* Path tessellation BIAS.  */
    /* Program matrix. */
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A40, (void *) &new_matrix[0]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A41, (void *) &new_matrix[1]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A42, (void *) &new_matrix[2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A43, (void *) &new_matrix[3]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A44, (void *) &new_matrix[4]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0A45, (void *) &new_matrix[5]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACD, (void *) &matrix->m[0][2]));
    VG_LITE_RETURN_ERROR(push_state_ptr(&s_context, 0x0ACE, (void *) &matrix->m[1][2]));

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        if (path->path_changed != 0)
        {
            if (path->uploaded.handle != NULL)
            {
                free_memory.memory_handle = path->uploaded.handle;
                vg_lite_kernel(VG_LITE_FREE, &free_memory);
                path->uploaded.address = 0;
                path->uploaded.memory = NULL;
                path->uploaded.handle = NULL;
            }
            /* Allocate memory for the path data. */
            memory.bytes = 16 + VG_LITE_ALIGN(path->path_length, 8);
            return_offset = (8 + VG_LITE_ALIGN(path->path_length, 8)) / 4;
            memory.contiguous = 1;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_ALLOCATE, &memory));
            ((uint64_t *) memory.memory)[(path->path_length + 7) / 8] = 0;
            ((uint32_t *) memory.memory)[0] = VG_LITE_DATA((path->path_length + 7) / 8);
            ((uint32_t *) memory.memory)[1] = 0;
            memcpy((uint8_t *) memory.memory + 8, path->path, path->path_length);
            ((uint32_t *) memory.memory)[return_offset] = VG_LITE_RETURN();
            ((uint32_t *) memory.memory)[return_offset + 1] = 0;

            path->uploaded.handle = memory.memory_handle;
            path->uploaded.memory = memory.memory;
            path->uploaded.address = memory.memory_gpu;
            path->uploaded.bytes  = memory.bytes;
            path->path_changed = 0;
        }
    }

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {

        vglitemDUMP_BUFFER("path", (size_t)path->uploaded.address, (uint8_t *)(path->uploaded.memory), 0,
                           path->uploaded.bytes);
    }

    vglitemDUMP("@[memory 0x%08X 0x%08X]", s_context.tsbuffer.tessellation_buffer_gpu,
                s_context.tsbuffer.tessellation_buffer_size);

    /* Tessellate path. */
    s_context.tsbuffer.tessellation_width_height = width | (height << 16);
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A1B, 0x00011000));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3D, tessellation_size / 64));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A39, point_min.x | (point_min.y)));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A3A, s_context.tsbuffer.tessellation_width_height));

    if (VLM_PATH_GET_UPLOAD_BIT(*path) == 1)
    {
        VG_LITE_RETURN_ERROR(push_call(&s_context, path->uploaded.address, path->uploaded.bytes));
#if  (DUMP_COMMAND && VG_COMMAND_CALL)
        if (strncmp(filename, "Commandbuffer", 13))
        {
            sprintf(filename, "Commandbuffer_pid%d.txt", getpid());
        }

        fp = fopen(filename, "a");

        if (fp == NULL)
        {
            printf("error!\n");
        }

        fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
                ((uint32_t *) memory.memory)[0], 0);

        unsigned char *pt = (unsigned char *) memory.memory;

        for (int i = 8; i <= return_offset * 4 - 1; i = i + 4)
        {
            if (i % 8 == 0)
            {
                fprintf(fp, "Command buffer: ");
            }

            if (i % 4 == 0)
            {
                fprintf(fp, "0x");
            }

            for (int j = 3; j >= 0; --j)
            {
                fprintf(fp, "%02x", pt[i + j]);
            }

            if ((i / 4 + 1) % 2 == 0)
            {
                fprintf(fp, ",\n");
            }
            else
            {
                fprintf(fp, ", ");
            }
        }

        fprintf(fp, "Command buffer: 0x%08x, 0x%08x,\n",
                ((uint32_t *) memory.memory)[return_offset], 0);

        fclose(fp);
        fp = NULL;
#endif
    }
    else
    {
        if ((path->path_type & 0x1) == VG_LITE_DRAW_FILL_PATH)
        {
            push_data(&s_context, path->path_length, path->path);
        }
        if (path->path_type == VG_LITE_DRAW_STROKE_PATH ||
            path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH)
        {
            format = convert_path_format(VG_LITE_FP32);
            VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0x01000200 | format | quality | tiling | 0x0));
            push_data(&s_context, path->stroke_path_size, path->stroke_path_data);
        }
    }

    /* Finialize command buffer. */
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A34, 0));

    vglitemDUMP_BUFFER("image", (size_t)source->address, source->memory, 0,
                       (source->stride) * (source->height));
#if DUMP_IMAGE
    dump_img(source->memory, source->width, source->height, source->format);
#endif
    return error;
}

vg_lite_error_t vg_lite_init_grad(vg_lite_linear_gradient_t *grad)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    /* Set the member values according to driver defaults. */
    grad->image.width = VLC_GRADIENT_BUFFER_WIDTH;
    grad->image.height = 1;
    grad->image.stride = 0;
    grad->image.format = VG_LITE_BGRA8888;

    /* Allocate the image for gradient. */
    error = vg_lite_allocate(&grad->image);

    grad->count = 0;

    return error;
}

vg_lite_error_t vg_lite_set_linear_grad(vg_lite_linear_gradient_ext_t *grad,
                                        uint32_t count,
                                        vg_lite_color_ramp_t *vg_color_ramp,
                                        vg_lite_linear_gradient_parameter_t linear_gradient,
                                        vg_lite_radial_gradient_spreadmode_t spread_mode,
                                        uint8_t color_ramp_premultiplied)
{
    int i;
    static vg_lite_color_ramp_t default_ramp[] =
    {
        {
            0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        },
        {
            1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        }
    };

    uint32_t trg_count;
    vg_lite_float_t prev_stop;
    vg_lite_color_ramp_ptr src_ramp;
    vg_lite_color_ramp_ptr src_ramp_last;
    vg_lite_color_ramp_ptr trg_ramp;

    /* Reset the count. */
    trg_count = 0;

    if ((linear_gradient.X0 == linear_gradient.X1) && (linear_gradient.Y0 == linear_gradient.Y1))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    grad->linear_gradient = linear_gradient;
    grad->color_ramp_premultiplied = color_ramp_premultiplied;
    grad->spread_mode = spread_mode;

    if (!count || count > MAX_COLOR_RAMP_STOPS || vg_color_ramp == NULL)
    {
        goto Empty_sequence_handler;
    }

    for (i = 0; i < count; i++)
    {
        grad->vg_color_ramp[i] = vg_color_ramp[i];
    }
    grad->vg_color_ramp_length = count;

    /* Determine the last source ramp. */
    src_ramp_last
        = grad->vg_color_ramp
          + grad->vg_color_ramp_length;

    /* Set the initial previous stop. */
    prev_stop = -1;

    /* Reset the count. */
    trg_count = 0;

    /* Walk through the source ramp. */
    for (
        src_ramp = grad->vg_color_ramp, trg_ramp = grad->int_color_ramp;
        (src_ramp < src_ramp_last) && (trg_count < MAX_COLOR_RAMP_STOPS + 2);
        src_ramp += 1
    )
    {
        /* Must be in increasing order. */
        if (src_ramp->stop < prev_stop)
        {
            /* Ignore the entire sequence. */
            trg_count = 0;
            break;
        }

        /* Update the previous stop value. */
        prev_stop = src_ramp->stop;

        /* Must be within [0..1] range. */
        if ((src_ramp->stop < 0.0f) || (src_ramp->stop > 1.0f))
        {
            /* Ignore. */
            continue;
        }

        /* Clamp color. */
        ClampColor(COLOR_FROM_RAMP(src_ramp), COLOR_FROM_RAMP(trg_ramp), 0);

        /* First stop greater then zero? */
        if ((trg_count == 0) && (src_ramp->stop > 0.0f))
        {
            /* Force the first stop to 0.0f. */
            trg_ramp->stop = 0.0f;

            /* Replicate the entry. */
            trg_ramp[1] = *trg_ramp;
            trg_ramp[1].stop = src_ramp->stop;

            /* Advance. */
            trg_ramp  += 2;
            trg_count += 2;
        }
        else
        {
            /* Set the stop value. */
            trg_ramp->stop = src_ramp->stop;

            /* Advance. */
            trg_ramp  += 1;
            trg_count += 1;
        }
    }

    /* Empty sequence? */
    if (trg_count == 0)
    {
        memcpy(grad->int_color_ramp, default_ramp, sizeof(default_ramp));
        grad->int_color_ramp_length = sizeof(default_ramp) / 5;
    }
    else
    {
        /* The last stop must be at 1.0. */
        if (trg_ramp[-1].stop != 1.0f)
        {
            /* Replicate the last entry. */
            *trg_ramp = trg_ramp[-1];

            /* Force the last stop to 1.0f. */
            trg_ramp->stop = 1.0f;

            /* Update the final entry count. */
            trg_count += 1;
        }

        /* Set new length. */
        grad->int_color_ramp_length = trg_count;
    }
    return VG_LITE_SUCCESS;

Empty_sequence_handler:
    memcpy(grad->int_color_ramp, default_ramp, sizeof(default_ramp));
    grad->int_color_ramp_length = sizeof(default_ramp) / 5;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_update_linear_grad(vg_lite_linear_gradient_ext_t *grad)
{
    uint32_t color_ramp_length;
    vg_lite_color_ramp_ptr color_ramp;
    uint32_t common, stop;
    uint32_t i, width;
    uint8_t *bits;
    vg_lite_float_t x0, y0, x1, y1, length;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    /* Get shortcuts to the color ramp. */
    color_ramp_length = grad->int_color_ramp_length;
    color_ramp       = grad->int_color_ramp;

    x0 = grad->linear_gradient.X0;
    y0 = grad->linear_gradient.Y0;
    x1 = grad->linear_gradient.X1;
    y1 = grad->linear_gradient.Y1;
    length = (vg_lite_float_t)sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));

    if (length <= 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }
    /* Find the common denominator of the color ramp stops. */
    if (length < 1)
    {
        common = 1;
    }
    else
    {
        common = (uint32_t)length;
    }

    for (i = 0; i < color_ramp_length; ++i)
    {
        if (color_ramp[i].stop != 0.0f)
        {
            vg_lite_float_t mul  = common * color_ramp[i].stop;
            vg_lite_float_t frac = mul - (vg_lite_float_t) floor(mul);
            if (frac > 0.00013f)    /* Suppose error for zero is 0.00013 */
            {
                common = MAX(common, (uint32_t)(1.0f / frac + 0.5f));
            }
        }
    }

    /* Compute the width of the required color array. */
    width = common + 1;

    /* Allocate the color ramp surface. */
    grad->image.width = width;
    grad->image.height = 1;
    grad->image.stride = 0;
    grad->image.image_mode = VG_LITE_NONE_IMAGE_MODE;
    grad->image.format = VG_LITE_ABGR8888;

    /* Allocate the image for gradient. */
    VG_LITE_RETURN_ERROR(vg_lite_allocate(&grad->image));

    //width = grad->image.width;
    width = common + 1;
    /* Set pointer to color array. */
    bits = (uint8_t *)grad->image.memory;

    /* Start filling the color array. */
    stop = 0;
    for (i = 0; i < width; ++i)
    {
        vg_lite_float_t gradient;
        vg_lite_float_t color[4];
        vg_lite_float_t color1[4];
        vg_lite_float_t color2[4];
        vg_lite_float_t weight;

        if (i == 241)
        {
            i = 241;
        }
        /* Compute gradient for current color array entry. */
        gradient = (vg_lite_float_t) i / (vg_lite_float_t)(width - 1);

        /* Find the entry in the color ramp that matches or exceeds this
        ** gradient. */
        while (gradient > color_ramp[stop].stop)
        {
            ++stop;
        }

        if (gradient == color_ramp[stop].stop)
        {
            /* Perfect match weight 1.0. */
            weight = 1.0f;

            /* Use color ramp color. */
            color1[3] = color_ramp[stop].alpha;
            color1[2] = color_ramp[stop].blue;
            color1[1] = color_ramp[stop].green;
            color1[0] = color_ramp[stop].red;

            color2[3] =
                color2[2] =
                    color2[1] =
                        color2[0] = 0.0f;
        }
        else
        {
            /* Compute weight. */
            weight = (color_ramp[stop].stop - gradient)
                     / (color_ramp[stop].stop - color_ramp[stop - 1].stop);

            /* Grab color ramp color of previous stop. */
            color1[3] = color_ramp[stop - 1].alpha;
            color1[2] = color_ramp[stop - 1].blue;
            color1[1] = color_ramp[stop - 1].green;
            color1[0] = color_ramp[stop - 1].red;

            /* Grab color ramp color of current stop. */
            color2[3] = color_ramp[stop].alpha;
            color2[2] = color_ramp[stop].blue;
            color2[1] = color_ramp[stop].green;
            color2[0] = color_ramp[stop].red;
        }

        if (grad->color_ramp_premultiplied)
        {
            /* Pre-multiply the first color. */
            color1[2] *= color1[3];
            color1[1] *= color1[3];
            color1[0] *= color1[3];

            /* Pre-multiply the second color. */
            color2[2] *= color2[3];
            color2[1] *= color2[3];
            color2[0] *= color2[3];
        }

        /* Filter the colors per channel. */
        color[3] = LERP(color1[3], color2[3], weight);
        color[2] = LERP(color1[2], color2[2], weight);
        color[1] = LERP(color1[1], color2[1], weight);
        color[0] = LERP(color1[0], color2[0], weight);

        /* Pack the final color. */
        *bits++ = PackColorComponent(color[3]);
        *bits++ = PackColorComponent(color[2]);
        *bits++ = PackColorComponent(color[1]);
        *bits++ = PackColorComponent(color[0]);
    }
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_rad_grad(vg_lite_radial_gradient_t *grad,
                                     uint32_t count,
                                     vg_lite_color_ramp_t *vgColorRamp,
                                     vg_lite_radial_gradient_parameter_t radialGradient,
                                     vg_lite_radial_gradient_spreadmode_t SpreadMode,
                                     uint8_t colorRampPremultiplied)
{
    int i;
    static vg_lite_color_ramp_t defaultRamp[] =
    {
        {
            0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        },
        {
            1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        }
    };

    uint32_t trgCount;
    vg_lite_float_t prevStop;
    vg_lite_color_ramp_ptr srcRamp;
    vg_lite_color_ramp_ptr srcRampLast;
    vg_lite_color_ramp_ptr trgRamp;

    /* Reset the count. */
    trgCount = 0;

    if (radialGradient.r <= 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    grad->radialGradient = radialGradient;
    grad->colorRampPremultiplied = colorRampPremultiplied;
    grad->SpreadMode = SpreadMode;

    if (!count || count > MAX_COLOR_RAMP_STOPS || vgColorRamp == NULL)
    {
        goto Empty_sequence_handler;
    }

    for (i = 0; i < count; i++)
    {
        grad->vgColorRamp[i] = vgColorRamp[i];
    }
    grad->vgColorRampLength = count;

    /* Determine the last source ramp. */
    srcRampLast
        = grad->vgColorRamp
          + grad->vgColorRampLength;

    /* Set the initial previous stop. */
    prevStop = -1;

    /* Reset the count. */
    trgCount = 0;

    /* Walk through the source ramp. */
    for (
        srcRamp = grad->vgColorRamp, trgRamp = grad->intColorRamp;
        (srcRamp < srcRampLast) && (trgCount < MAX_COLOR_RAMP_STOPS + 2);
        srcRamp += 1
    )
    {
        /* Must be in increasing order. */
        if (srcRamp->stop < prevStop)
        {
            /* Ignore the entire sequence. */
            trgCount = 0;
            break;
        }

        /* Update the previous stop value. */
        prevStop = srcRamp->stop;

        /* Must be within [0..1] range. */
        if ((srcRamp->stop < 0.0f) || (srcRamp->stop > 1.0f))
        {
            /* Ignore. */
            continue;
        }

        /* Clamp color. */
        ClampColor(COLOR_FROM_RAMP(srcRamp), COLOR_FROM_RAMP(trgRamp), 0);

        /* First stop greater then zero? */
        if ((trgCount == 0) && (srcRamp->stop > 0.0f))
        {
            /* Force the first stop to 0.0f. */
            trgRamp->stop = 0.0f;

            /* Replicate the entry. */
            trgRamp[1] = *trgRamp;
            trgRamp[1].stop = srcRamp->stop;

            /* Advance. */
            trgRamp  += 2;
            trgCount += 2;
        }
        else
        {
            /* Set the stop value. */
            trgRamp->stop = srcRamp->stop;

            /* Advance. */
            trgRamp  += 1;
            trgCount += 1;
        }
    }

    /* Empty sequence? */
    if (trgCount == 0)
    {
        memcpy(grad->intColorRamp, defaultRamp, sizeof(defaultRamp));
        grad->intColorRampLength = sizeof(defaultRamp) / 5;
    }
    else
    {
        /* The last stop must be at 1.0. */
        if (trgRamp[-1].stop != 1.0f)
        {
            /* Replicate the last entry. */
            *trgRamp = trgRamp[-1];

            /* Force the last stop to 1.0f. */
            trgRamp->stop = 1.0f;

            /* Update the final entry count. */
            trgCount += 1;
        }

        /* Set new length. */
        grad->intColorRampLength = trgCount;
    }
    return VG_LITE_SUCCESS;

Empty_sequence_handler:
    memcpy(grad->intColorRamp, defaultRamp, sizeof(defaultRamp));
    grad->intColorRampLength = sizeof(defaultRamp) / 5;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_update_rad_grad(vg_lite_radial_gradient_t *grad)
{
    uint32_t colorRampLength;
    vg_lite_color_ramp_ptr colorRamp;
    uint32_t common, stop;
    uint32_t i, width;
    uint8_t *bits;
    vg_lite_float_t r;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    /* Get shortcuts to the color ramp. */
    colorRampLength = grad->intColorRampLength;
    colorRamp       = grad->intColorRamp;

    r = grad->radialGradient.r;

    if (r <= 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }
    /* Find the common denominator of the color ramp stops. */
    if (r < 1)
    {
        common = 1;
    }
    else
    {
        common = (uint32_t)r;
    }

    for (i = 0; i < colorRampLength; ++i)
    {
        if (colorRamp[i].stop != 0.0f)
        {
            vg_lite_float_t mul  = common * colorRamp[i].stop;
            vg_lite_float_t frac = mul - (vg_lite_float_t) floor(mul);
            if (frac > 0.00013f)    /* Suppose error for zero is 0.00013 */
            {
                common = MAX(common, (uint32_t)(1.0f / frac + 0.5f));
            }
        }
    }

    /* Compute the width of the required color array. */
    width = common + 1;

    /* Allocate the color ramp surface. */
    grad->image.width = width;
    grad->image.height = 1;
    grad->image.stride = 0;
    grad->image.image_mode = VG_LITE_NONE_IMAGE_MODE;
    grad->image.format = VG_LITE_ABGR8888;

    /* Allocate the image for gradient. */
    VG_LITE_RETURN_ERROR(vg_lite_allocate(&grad->image));

    width = grad->image.width;

    /* Set pointer to color array. */
    bits = (uint8_t *)grad->image.memory;

    /* Start filling the color array. */
    stop = 0;
    for (i = 0; i < width; ++i)
    {
        vg_lite_float_t gradient;
        vg_lite_float_t color[4];
        vg_lite_float_t color1[4];
        vg_lite_float_t color2[4];
        vg_lite_float_t weight;

        /* Compute gradient for current color array entry. */
        gradient = (vg_lite_float_t) i / (vg_lite_float_t)(width - 1);

        /* Find the entry in the color ramp that matches or exceeds this
        ** gradient. */
        while (gradient > colorRamp[stop].stop)
        {
            ++stop;
        }

        if (gradient == colorRamp[stop].stop)
        {
            /* Perfect match weight 1.0. */
            weight = 1.0f;

            /* Use color ramp color. */
            color1[3] = colorRamp[stop].alpha;
            color1[2] = colorRamp[stop].blue;
            color1[1] = colorRamp[stop].green;
            color1[0] = colorRamp[stop].red;

            color2[3] =
                color2[2] =
                    color2[1] =
                        color2[0] = 0.0f;
        }
        else
        {
            /* Compute weight. */
            weight = (colorRamp[stop].stop - gradient)
                     / (colorRamp[stop].stop - colorRamp[stop - 1].stop);

            /* Grab color ramp color of previous stop. */
            color1[3] = colorRamp[stop - 1].alpha;
            color1[2] = colorRamp[stop - 1].blue;
            color1[1] = colorRamp[stop - 1].green;
            color1[0] = colorRamp[stop - 1].red;

            /* Grab color ramp color of current stop. */
            color2[3] = colorRamp[stop].alpha;
            color2[2] = colorRamp[stop].blue;
            color2[1] = colorRamp[stop].green;
            color2[0] = colorRamp[stop].red;
        }

        if (grad->colorRampPremultiplied)
        {
            /* Pre-multiply the first color. */
            color1[2] *= color1[3];
            color1[1] *= color1[3];
            color1[0] *= color1[3];

            /* Pre-multiply the second color. */
            color2[2] *= color2[3];
            color2[1] *= color2[3];
            color2[0] *= color2[3];
        }

        /* Filter the colors per channel. */
        color[3] = LERP(color1[3], color2[3], weight);
        color[2] = LERP(color1[2], color2[2], weight);
        color[1] = LERP(color1[1], color2[1], weight);
        color[0] = LERP(color1[0], color2[0], weight);

        /* Pack the final color. */
        *bits++ = PackColorComponent(color[3]);
        *bits++ = PackColorComponent(color[2]);
        *bits++ = PackColorComponent(color[1]);
        *bits++ = PackColorComponent(color[0]);
    }
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_grad(vg_lite_linear_gradient_t *grad,
                                 uint32_t count,
                                 uint32_t *colors,
                                 uint32_t *stops)
{
    uint32_t i;

    grad->count = 0;    /* Opaque B&W gradient */
    if (!count || count > VLC_MAX_GRAD || colors == NULL || stops == NULL)
    {
        return VG_LITE_SUCCESS;
    }
    /* Check stops validity */
    for (i = 0; i < count; i++)
        if (stops[i] <= 255)
        {
            if (!grad->count || stops[i] > grad->stops[grad->count - 1])
            {
                grad->stops[grad->count] = stops[i];
                grad->colors[grad->count] = colors[i];
                grad->count++;
            }
            else if (stops[i] == grad->stops[grad->count - 1])
            {
                /* Equal stops : use the color corresponding to the last stop
                in the sequence */
                grad->colors[grad->count - 1] = colors[i];
            }
        }
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_update_grad(vg_lite_linear_gradient_t *grad)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    int32_t r0, g0, b0, a0;
    int32_t r1, g1, b1, a1;
    int32_t lr, lg, lb, la;
    uint32_t i;
    int32_t j;
    int32_t ds, dr, dg, db, da;
    uint32_t *buffer = (uint32_t *)grad->image.memory;

    if (grad->count == 0)
    {
        /* If no valid stops have been specified (e.g., due to an empty input
        * array, out-of-range, or out-of-order stops), a stop at 0 with color
        * 0xFF000000 (opaque black) and a stop at 255 with color 0xFFFFFFFF
        * (opaque white) are implicitly defined. */
        grad->stops[0] = 0;
        grad->colors[0] = 0xFF000000;   /* Opaque black */
        grad->stops[1] = 255;
        grad->colors[1] = 0xFFFFFFFF;   /* Opaque white */
        grad->count = 2;
    }
    else if (grad->count && grad->stops[0] != 0)
    {
        /* If at least one valid stop has been specified, but none has been
        * defined with an offset of 0, an implicit stop is added with an
        * offset of 0 and the same color as the first user-defined stop. */
        for (i = 0; i < grad->stops[0]; i++)
        {
            buffer[i] = grad->colors[0];
        }
    }
    a0 = A(grad->colors[0]);
    r0 = R(grad->colors[0]);
    g0 = G(grad->colors[0]);
    b0 = B(grad->colors[0]);

    /* Calculate the colors for each pixel of the image. */
    for (i = 0; i < grad->count - 1; i++)
    {
        buffer[grad->stops[i]] = grad->colors[i];
        ds = grad->stops[i + 1] - grad->stops[i];
        a1 = A(grad->colors[i + 1]);
        r1 = R(grad->colors[i + 1]);
        g1 = G(grad->colors[i + 1]);
        b1 = B(grad->colors[i + 1]);

        da = a1 - a0;
        dr = r1 - r0;
        dg = g1 - g0;
        db = b1 - b0;

        for (j = 1; j < ds; j++)
        {
            la = a0 + da * j / ds;
            lr = r0 + dr * j / ds;
            lg = g0 + dg * j / ds;
            lb = b0 + db * j / ds;

            buffer[grad->stops[i] + j] = ARGB(la, lr, lg, lb);
        }

        a0 = a1;
        r0 = r1;
        g0 = g1;
        b0 = b1;
    }
    /* If at least one valid stop has been specified, but none has been defined
    * with an offset of 255, an implicit stop is added with an offset of 255
    * and the same color as the last user-defined stop. */
    for (i = grad->stops[grad->count - 1]; i < 255; i++)
    {
        buffer[i] = grad->colors[grad->count - 1];
    }
    /* Last pixel */
    buffer[i] = grad->colors[grad->count - 1];
    return error;
}

vg_lite_error_t vg_lite_clear_linear_grad(vg_lite_linear_gradient_ext_t *grad)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    grad->count = 0;
    /* Release the image resource. */
    if (grad->image.handle != NULL)
    {
        error = vg_lite_free(&grad->image);
    }

    return error;
}

vg_lite_error_t vg_lite_clear_grad(vg_lite_linear_gradient_t *grad)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    grad->count = 0;
    /* Release the image resource. */
    if (grad->image.handle != NULL)
    {
        error = vg_lite_free(&grad->image);
    }

    return error;
}

vg_lite_error_t vg_lite_clear_rad_grad(vg_lite_radial_gradient_t *grad)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    grad->count = 0;
    /* Release the image resource. */
    if (grad->image.handle != NULL)
    {
        error = vg_lite_free(&grad->image);
    }

    return error;
}

vg_lite_matrix_t *vg_lite_get_linear_grad_matrix(vg_lite_linear_gradient_ext_t *grad)
{
    return &grad->matrix;
}

vg_lite_matrix_t *vg_lite_get_grad_matrix(vg_lite_linear_gradient_t *grad)
{
    return &grad->matrix;
}

vg_lite_matrix_t *vg_lite_get_rad_grad_matrix(vg_lite_radial_gradient_t *grad)
{
    return &grad->matrix;
}

vg_lite_error_t vg_lite_draw_gradient(vg_lite_buffer_t *target,
                                      vg_lite_path_t *path,
                                      vg_lite_fill_t fill_rule,
                                      vg_lite_matrix_t *matrix,
                                      vg_lite_linear_gradient_t *grad,
                                      vg_lite_blend_t blend)
{
    return vg_lite_draw_pattern(target, path, fill_rule, matrix,
                                &grad->image, &grad->matrix, blend, VG_LITE_PATTERN_PAD, 0, VG_LITE_FILTER_LINEAR);
}

vg_lite_error_t vg_lite_set_command_buffer_size(uint32_t size)
{
    if (command_buffer_size == 0)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    command_buffer_size = size;
    return VG_LITE_SUCCESS;
}
vg_lite_error_t vg_lite_set_scissor(int32_t x, int32_t y, int32_t right, int32_t bottom)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    /* Save scissor Box States. */
    s_context.scissor[0] = x;
    s_context.scissor[1] = y;
    s_context.scissor[2] = right;
    s_context.scissor[3] = bottom;

    /* Scissor dirty. */
    s_context.scissor_dirty = 1;

    return error;
}

vg_lite_error_t vg_lite_enable_scissor()
{
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    /* Enable scissor Mode. */
    s_context.scissor_enabled = 1;

    /* Scissor dirty. */
    s_context.scissor_dirty = 1;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_disable_scissor()
{
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_SCISSOR))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    /* disable scissor Mode. */
    s_context.scissor_enabled = 0;

    /* Scissor dirty. */
    s_context.scissor_dirty = 1;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_mem_avail(uint32_t *size)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_mem_t mem;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_QUERY_MEM, &mem));
    *size = mem.bytes;

    return error;
}

vg_lite_error_t vg_lite_set_image_global_alpha(vg_lite_global_alpha_t alpha_mode,
                                               uint8_t alpha_value)
{
    uint32_t image_alpha_mode;
    uint32_t image_alpha_value;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_GLOBAL_ALPHA))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    image_alpha_mode = (uint8_t)alpha_mode;
    image_alpha_value = alpha_value << 2;

    s_context.image_global_alpha_mode = image_alpha_mode;
    s_context.image_global_alpha_value = image_alpha_value;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_dest_global_alpha(vg_lite_global_alpha_t alpha_mode,
                                              uint8_t alpha_value)
{
    uint32_t dest_alpha_mode;
    uint32_t dest_alpha_value;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_GLOBAL_ALPHA))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    dest_alpha_mode = (alpha_mode == VG_LITE_NORMAL) ? 0 : (alpha_mode == VG_LITE_GLOBAL) ? 0x00000400 :
                      0x00000800;
    dest_alpha_value = alpha_value << 12;

    s_context.dest_global_alpha_mode = dest_alpha_mode;
    s_context.dest_global_alpha_value = dest_alpha_value;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_set_color_key(vg_lite_color_key4_t colorkey)
{
    uint8_t i;
    uint32_t value_low = 0;
    uint32_t value_high = 0;
    uint8_t r, g, b, a, e;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_COLOR_KEY))
    {
        return VG_LITE_NOT_SUPPORT;
    }

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
}

vg_lite_error_t vg_lite_enable_flexa()
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_info_t data;
    uint32_t reset_bit;
    vg_lite_kernel_flexa_info_t flexa_data;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_FLEXA))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    flexa_data.sbi_mode = s_context.sbi_mode = 0x1;
    flexa_data.sync_mode = s_context.sync_mode = BIT(14);
    flexa_data.flexa_mode = s_context.flexa_mode = 0x1;
    flexa_data.segment_buffer_address = s_context.segment_buffer_address;
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

    if (reset_bit == 1)
    {
        error = VG_LITE_TIMEOUT;
        return error;
    }
    s_context.flexa_dirty = 1;

    return error;
}

vg_lite_error_t vg_lite_set_flexa_stream_id(uint8_t stream_id)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_FLEXA))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    s_context.stream_id = stream_id << 1;

    return error;
}

vg_lite_error_t vg_lite_set_flexa_current_background_buffer(uint8_t stream_id,
                                                            vg_lite_buffer_t *buffer, uint32_t background_segment_count, uint32_t background_segment_size)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_FLEXA))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    flexa_data.sbi_mode = s_context.sbi_mode;
    flexa_data.segment_buffer_address = s_context.segment_buffer_address = buffer->address;
    flexa_data.segment_count = s_context.segment_count = background_segment_count;
    flexa_data.segment_size = s_context.segment_size = background_segment_size;
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.start_flag = s_context.start_flag;
    flexa_data.stop_flag = s_context.stop_flag;
    flexa_data.reset_flag = s_context.reset_flag ;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_SET_BACKGROUND_ADDRESS, &flexa_data));

    return error;
}

vg_lite_error_t vg_lite_set_flexa_stop_frame()
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_FLEXA))
    {
        return VG_LITE_NOT_SUPPORT;
    }

    flexa_data.stop_flag = s_context.stop_flag = BIT(11);
    flexa_data.stream_id = s_context.stream_id;
    flexa_data.sbi_mode = s_context.sbi_mode;
    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FLEXA_STOP_FRAME, &flexa_data));

    return error;
}

vg_lite_error_t vg_lite_disable_flexa()
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_flexa_info_t flexa_data;
    vg_lite_kernel_info_t data;
    uint32_t reset_bit;
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_FLEXA))
    {
        return VG_LITE_NOT_SUPPORT;
    }
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
    if (reset_bit == 1)
    {
        error = VG_LITE_TIMEOUT;
        return error;
    }
    s_context.flexa_dirty = 1;

    return error;
}

vg_lite_error_t vg_lite_set_dither(int enable)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t table_low = 0x7B48F3C0;
    uint32_t table_high = 0x596AD1E2;
    int dither_enable = enable;
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_DITHER))
    {
        return VG_LITE_NOT_SUPPORT;
    }
    if (dither_enable)
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5A, table_low));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5B, table_high));
    }
    else
    {
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5A, 0xFFFFFFFF));
        VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A5B, 0xFFFFFFFF));
    }

    return error;
}

vg_lite_error_t vg_lite_set_command_buffer(uint32_t physical, uint32_t size)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_map_memory_t map = {0};

    if ((physical == 0) || (size == 0) || (physical % 64) || (size % 128))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    map.bytes = size;
    map.physical = physical;

    if (s_context.command_buffer[0])
    {
        vg_lite_kernel_unmap_memory_t unmap = {0};
        if (submit_flag)
        {
            VG_LITE_RETURN_ERROR(stall(&s_context, 0, (uint32_t)~0));
        }
        if (!s_context.custom_cmd_buf)
        {
            vg_lite_kernel_free_t free;
            free.memory_handle = s_context.context.command_buffer[0];
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free));
            s_context.context.command_buffer[0] = 0;

            free.memory_handle = s_context.context.command_buffer[1];
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free));
            s_context.context.command_buffer[1] = 0;
        }
        unmap.bytes = s_context.command_buffer_size;
        unmap.logical = s_context.command_buffer[0];
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_UNMAP_MEMORY, &unmap));
        unmap.bytes = s_context.command_buffer_size;
        unmap.logical = s_context.command_buffer[1];
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_UNMAP_MEMORY, &unmap));
    }

    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_MAP_MEMORY, &map));

    s_context.context.command_buffer_logical[0] = map.logical;
    s_context.context.command_buffer_physical[0] = map.physical;

    s_context.context.command_buffer_logical[1] = (void *)((uint8_t *)map.logical + map.bytes / 2);
    s_context.context.command_buffer_physical[1] = map.physical + map.bytes / 2;

    s_context.command_buffer[0] = s_context.context.command_buffer_logical[0];
    s_context.command_buffer[1] = s_context.context.command_buffer_logical[1];
    s_context.command_offset[0] = 0;
    s_context.command_offset[1] = 0;
    s_context.command_buffer_current = 0;
    s_context.command_buffer_size = map.bytes / 2;
    s_context.custom_cmd_buf = 1;
    return error;
}

vg_lite_error_t vg_lite_set_ts_buffer(uint32_t physical, uint32_t size)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_map_memory_t map = {0};

    if ((physical == 0) || (size == 0) || (physical % 64) || (size % 64) || (size < MIN_TS_SIZE))
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    map.bytes = size;
    map.physical = physical;

    if (s_context.tsbuffer.tessellation_buffer_logic)
    {
        vg_lite_kernel_unmap_memory_t unmap = {0};
        if (submit_flag)
        {
            VG_LITE_RETURN_ERROR(stall(&s_context, 0, (uint32_t)~0));
        }
        if (!s_context.custom_ts_buf)
        {
            vg_lite_kernel_free_t free;
            free.memory_handle = s_context.context.tessellation_buffer;
            VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_FREE, &free));
            s_context.context.tessellation_buffer = 0;
        }
        unmap.bytes = s_context.tsbuffer.tessellation_buffer_size + s_context.tsbuffer.vg_count_buffer_size;
        unmap.logical = s_context.tsbuffer.tessellation_buffer_logic;
        VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_UNMAP_MEMORY, &unmap));
    }

    VG_LITE_RETURN_ERROR(vg_lite_kernel(VG_LITE_MAP_MEMORY, &map));

    s_context.tsbuffer.tessellation_buffer_logic = map.logical;
    s_context.tsbuffer.tessellation_buffer_gpu = map.physical;
    s_context.tsbuffer.vg_count_buffer_size = size * 3 / 128;
    s_context.tsbuffer.vg_count_buffer_size = VG_LITE_ALIGN(s_context.tsbuffer.vg_count_buffer_size,
                                                            64);
    s_context.tsbuffer.tessellation_buffer_size = map.bytes - s_context.tsbuffer.vg_count_buffer_size;

    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0A35, s_context.tsbuffer.tessellation_buffer_gpu));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0AC8, s_context.tsbuffer.tessellation_buffer_size));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0ACB,
                                    s_context.tsbuffer.tessellation_buffer_gpu + s_context.tsbuffer.tessellation_buffer_size));
    VG_LITE_RETURN_ERROR(push_state(&s_context, 0x0ACC, s_context.tsbuffer.vg_count_buffer_size));

    s_context.custom_ts_buf = 1;
    return error;
}
