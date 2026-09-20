/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_api_dc.h"
#include "gui_post_process.h"
#include "gui_matrix.h"

#ifndef SW_ARM2D_BLUR_DOWNSCALE_FACTOR
#define SW_ARM2D_BLUR_DOWNSCALE_FACTOR 2U
#endif

#if SW_ARM2D_BLUR_DOWNSCALE_FACTOR < 1 || SW_ARM2D_BLUR_DOWNSCALE_FACTOR > 16
#error "SW_ARM2D_BLUR_DOWNSCALE_FACTOR must be in the range 1..16"
#endif

#define SW_ARM2D_ROUND_UP_8(x) (((x) + 7U) & ~7U)

typedef struct
{
    int16_t iX;
    int16_t iY;
} arm2d_local_location_t;

typedef struct
{
    int16_t iWidth;
    int16_t iHeight;
} arm2d_local_size;

typedef struct arm2d_local_region_t
{
    arm2d_local_location_t tLocation;
    arm2d_local_size tSize;
} arm2d_local_region_t;

typedef struct arm2d_local_scratch_mem_t
{
    struct
    {
        uint32_t u24SizeInByte      : 24;
        uint32_t u2ItemSize         : 3;
        uint32_t u2Align            : 3;
    } tInfo;

    uintptr_t pBuffer;
    uint16_t hwTargetWidth;
    uint16_t hwTargetHeight;
    uint8_t chStatusBytesPerPixel;
#if SW_ARM2D_BLUR_DOWNSCALE_FACTOR > 1
    uintptr_t pScaledBuffer;
    uint16_t hwScaledStride;
    uint16_t hwScaledRows;
    uint8_t chBytesPerPixel;
    size_t scaledBufferSize;
#endif
} arm2d_local_scratch_mem_t;

typedef struct arm2d_local_filter_iir_blur_descriptor_t
{
    struct
    {
        uint8_t bForwardHorizontal  : 1;
        uint8_t bForwardVertical    : 1;
        uint8_t bReverseHorizontal  : 1;
        uint8_t bReverseVertical    : 1;
    } dir;

    uint8_t chBlurDegree;
    arm2d_local_scratch_mem_t tScratchMemory;

} arm2d_local_filter_iir_blur_descriptor_t;

typedef struct arm2d_color_cccn888_t
{
    uint16_t hwB;
    uint16_t hwG;
    uint16_t hwR;
} arm2d_color_cccn888_t;

typedef arm2d_color_cccn888_t arm2d_color_rgb565_t;

/* 32-bit ARGB8888 IIR blur accumulator */
typedef struct arm2d_color_argb8888_acc_t
{
    uint16_t hwB;
    uint16_t hwG;
    uint16_t hwR;
} arm2d_color_argb8888_acc_t;

static void *arm2d_local_allocate_scratch_memory(uint32_t wSize,
                                                 uint_fast8_t nAlign)
{
    GUI_UNUSED(nAlign);

    /* ensure nAlign is 2^n */
    GUI_ASSERT((((~nAlign) + 1) & nAlign) == nAlign);

    void *pBuff = malloc(wSize);
    GUI_ASSERT(0 == ((uintptr_t)pBuff & (nAlign - 1)));

    return pBuff;
}

static void arm2d_local_free_scratch_memory(void *pBuff)
{
    free(pBuff);
}


static arm2d_local_scratch_mem_t *arm2d_local_scratch_memory_new(arm2d_local_scratch_mem_t
                                                                 *ptMemory,
                                                                 uint16_t hwItemSize,
                                                                 uint16_t hwItemCount,
                                                                 uint16_t hwAlignment)
{
    size_t tSize = (size_t)hwItemSize * (size_t)hwItemCount;
    do
    {
        if (NULL == ptMemory)
        {
            GUI_ASSERT(false);
            break;
        }
        else if (0 == tSize)
        {
            break;
        }

        tSize = (tSize + 3) & ~3;

        ptMemory->pBuffer
            = (uintptr_t)arm2d_local_allocate_scratch_memory(tSize + 4,
                                                             hwAlignment);

        ptMemory->tInfo.u24SizeInByte = tSize;
        ptMemory->tInfo.u2Align = hwAlignment;
        ptMemory->tInfo.u2ItemSize = hwItemSize;

        if (NULL != (void *)(ptMemory->pBuffer))
        {
            /* add canary */
            *(volatile uint32_t *)((uintptr_t)(ptMemory->pBuffer) + tSize) = 0xCAFE0ACE;
        }

        return ptMemory;
    }
    while (0);

    return NULL;
}

static arm2d_local_scratch_mem_t *arm2d_local_scratch_memory_free(arm2d_local_scratch_mem_t
                                                                  *ptMemory)
{
    do
    {
        if (NULL == ptMemory)
        {
            break;
        }
        if (NULL == (void *)(ptMemory->pBuffer))
        {
            break;
        }

        size_t tSize = ptMemory->tInfo.u24SizeInByte;

        /* check canary */
        if (*(volatile uint32_t *)((uintptr_t)(ptMemory->pBuffer) + tSize) != 0xCAFE0ACE)
        {
            GUI_ASSERT(false);
        }

        arm2d_local_free_scratch_memory((void *)(ptMemory->pBuffer));
        memset(ptMemory, 0, sizeof(arm2d_local_scratch_mem_t));

    }
    while (0);

    return ptMemory;
}

typedef struct
{
    uint16_t        B;
    uint16_t        G;
    uint16_t        R;
    uint16_t        A;
} arm2d_local_color_fast_rgb_t;

typedef union arm_2d_color_rgb565_t
{
    uint16_t tValue;
    struct
    {
        uint16_t u5B : 5;
        uint16_t u6G : 6;
        uint16_t u5R : 5;
    } BGR;
} arm_2d_color_rgb565_t;

static inline uint16_t arm2d_local_rgb565_pack(arm2d_local_color_fast_rgb_t *ptRGB)
{
    // GUI_ASSERT(NULL != ptRGB);

    arm_2d_color_rgb565_t tOutput =
    {
        .BGR.u5B = (uint16_t)(ptRGB->B >> 3),
        .BGR.u6G = (uint16_t)(ptRGB->G >> 2),
        .BGR.u5R = (uint16_t)(ptRGB->R >> 3),
    };
    return tOutput.tValue;
}
static inline bool arm2d_local_reverse_h_allowed(
    const arm2d_local_region_t *ptValid,
    const arm2d_local_region_t *ptTarget)
{
    return (ptValid->tLocation.iX <= ptTarget->tLocation.iX)
           && ((ptValid->tLocation.iX + ptValid->tSize.iWidth)
               >= (ptTarget->tLocation.iX + ptTarget->tSize.iWidth));
}

static inline bool arm2d_local_reverse_v_allowed(
    const arm2d_local_region_t *ptValid,
    const arm2d_local_region_t *ptTarget)
{
    return (ptValid->tLocation.iY <= ptTarget->tLocation.iY)
           && ((ptValid->tLocation.iY + ptValid->tSize.iHeight)
               >= (ptTarget->tLocation.iY + ptTarget->tSize.iHeight));
}

static inline void arm2d_local_rgb565_unpack_comp(uint16_t hwColor,
                                                  arm2d_local_color_fast_rgb_t *ptRGB)
{
    // GUI_ASSERT(NULL != ptRGB);

    /* uses explicit extraction, leading to a more efficient autovectorized code */
    uint16_t maskRunpk = 0x001f, maskGunpk = 0x003f;

    if (hwColor)
    {
        ptRGB->B = ((uint16_t)((hwColor & maskRunpk) << 3)) | 0x7;
        ptRGB->R = ((uint16_t)((hwColor >> 11) << 3)) | 0x7;
        ptRGB->G = ((uint16_t)(((hwColor >> 5) & maskGunpk)) << 2) | 0x03;
    }
    else
    {
        ptRGB->B = (uint16_t)((hwColor & maskRunpk) << 3);
        ptRGB->R = (uint16_t)((hwColor >> 11) << 3);
        ptRGB->G = (uint16_t)(((hwColor >> 5) & maskGunpk) << 2);
    }

    ptRGB->A = 0xFF;
}

void arm2d_local_rgb565_filter_iir_blur(
    uint16_t *__restrict phwTarget,
    int16_t iTargetStride,
    arm2d_local_region_t *__restrict ptValidRegionOnVirtualScreen,
    arm2d_local_region_t *ptTargetRegionOnVirtualScreen,
    uint8_t chBlurDegree,
    arm2d_local_filter_iir_blur_descriptor_t *ptThis)
{
    arm2d_local_scratch_mem_t *ptScratchMemory = &ptThis->tScratchMemory;

    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    if (0 == chBlurDegree)
    {
        return;
    }

    int16_t iY, iX;
    uint16_t hwRatio = 256 - chBlurDegree;
    arm2d_color_rgb565_t tAcc;
    arm2d_color_rgb565_t *ptStatusH = NULL;
    arm2d_color_rgb565_t *ptStatusV = NULL;
    arm2d_local_color_fast_rgb_t tPixel;

    if (NULL != (void *)(ptScratchMemory->pBuffer))
    {
        ptStatusH = (arm2d_color_rgb565_t *)ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ptTargetRegionOnVirtualScreen->tSize.iWidth;
    }

    /* calculate the offset between the target region and the valid region */
    arm2d_local_location_t tOffset =
    {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    const bool bAllowReverseH = arm2d_local_reverse_h_allowed(
                                    ptValidRegionOnVirtualScreen,
                                    ptTargetRegionOnVirtualScreen);
    const bool bAllowReverseV = arm2d_local_reverse_v_allowed(
                                    ptValidRegionOnVirtualScreen,
                                    ptTargetRegionOnVirtualScreen);

    if (ptThis->dir.bForwardHorizontal)
    {
        uint16_t *phwPixel = phwTarget;

        if (NULL != ptStatusV)
        {
            /* rows direct path */
            ptStatusV += tOffset.iY;
        }

        for (iY = 0; iY < iHeight; iY++)
        {

            if (NULL != ptStatusV && tOffset.iX > 0)
            {
                /* recover the previous statues */
                tAcc = *ptStatusV;
            }
            else
            {

                arm2d_local_rgb565_unpack_comp(*phwPixel, &tPixel);
                tAcc = *(arm2d_color_rgb565_t *)&tPixel;

            }

            uint16_t *phwTargetPixel = phwPixel;

            for (iX = 0; iX < iWidth; iX++)
            {

                arm2d_local_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;

                tPixel.B = tAcc.hwB;
                tPixel.G = tAcc.hwG;
                tPixel.R = tAcc.hwR;

                *phwTargetPixel = arm2d_local_rgb565_pack(&tPixel);

                phwTargetPixel++;
            }

            if (NULL != ptStatusV)
            {
                /* save the last pixel */
                *ptStatusV++ = tAcc;
            }

            phwPixel += iTargetStride;
        }
    }

    /* rows reverse path (right to left, bottom to top) */
    if (ptThis->dir.bReverseHorizontal && bAllowReverseH)
    {
        uint16_t *phwPixel = &phwTarget[(iWidth - 1) + (iHeight - 1) * iTargetStride];

        for (iY = iHeight - 1; iY >= 0; iY--)
        {
            /* seed the accumulator from the right-most pixel of the row */
            arm2d_local_rgb565_unpack_comp(*phwPixel, &tPixel);
            tAcc = *(arm2d_color_rgb565_t *)&tPixel;

            uint16_t *phwTargetPixel = phwPixel;

            for (iX = 0; iX < iWidth; iX++)
            {
                arm2d_local_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;

                tPixel.B = tAcc.hwB;
                tPixel.G = tAcc.hwG;
                tPixel.R = tAcc.hwR;

                *phwTargetPixel = arm2d_local_rgb565_pack(&tPixel);

                phwTargetPixel--;
            }

            phwPixel -= iTargetStride;
        }
    }

    if (ptThis->dir.bForwardVertical)
    {
        uint16_t *phwPixel = phwTarget;

        if (NULL != ptStatusH)
        {
            ptStatusH += tOffset.iX;
        }

        /* columns direct path */
        for (iX = 0; iX < iWidth; iX++)
        {

            if (NULL != ptStatusH && tOffset.iY > 0)
            {
                /* recover the previous statues */
                tAcc = *ptStatusH;
            }
            else
            {

                arm2d_local_rgb565_unpack_comp(*phwPixel, &tPixel);
                tAcc = *(arm2d_color_rgb565_t *)&tPixel;

            }

            uint16_t *phwTargetPixel = phwPixel;

            for (iY = 0; iY < iHeight; iY++)
            {

                arm2d_local_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;

                tPixel.B = tAcc.hwB;
                tPixel.G = tAcc.hwG;
                tPixel.R = tAcc.hwR;

                *phwTargetPixel = arm2d_local_rgb565_pack(&tPixel);

                phwTargetPixel += iTargetStride;
            }

            phwPixel++;

            if (NULL != ptStatusH)
            {
                /* save the last pixel */
                *ptStatusH++ = tAcc;
            }
        }
    }

    /* columns reverse path (bottom to top, right to left) */
    if (ptThis->dir.bReverseVertical && bAllowReverseV)
    {
        uint16_t *phwPixel = &phwTarget[(iWidth - 1) + (iHeight - 1) * iTargetStride];

        for (iX = iWidth - 1; iX >= 0; iX--)
        {
            /* seed the accumulator from the bottom pixel of the column */
            arm2d_local_rgb565_unpack_comp(*phwPixel, &tPixel);
            tAcc = *(arm2d_color_rgb565_t *)&tPixel;

            uint16_t *phwTargetPixel = phwPixel;

            for (iY = 0; iY < iHeight; iY++)
            {
                arm2d_local_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;

                tPixel.B = tAcc.hwB;
                tPixel.G = tAcc.hwG;
                tPixel.R = tAcc.hwR;

                *phwTargetPixel = arm2d_local_rgb565_pack(&tPixel);

                phwTargetPixel -= iTargetStride;
            }

            phwPixel--;
        }
    }
}

static void arm2d_local_argb8888_filter_iir_blur(
    uint32_t *__restrict pwTarget,
    int16_t iTargetStride,
    arm2d_local_region_t *__restrict ptValidRegionOnVirtualScreen,
    arm2d_local_region_t *ptTargetRegionOnVirtualScreen,
    uint8_t chBlurDegree,
    arm2d_local_filter_iir_blur_descriptor_t *ptThis)
{
    arm2d_local_scratch_mem_t *ptScratchMemory = &ptThis->tScratchMemory;

    if (0 == chBlurDegree)
    {
        return;
    }

    int_fast16_t iWidth  = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    int16_t iY, iX;
    uint16_t hwRatio = 256 - chBlurDegree;
    arm2d_color_argb8888_acc_t tAcc;
    arm2d_color_argb8888_acc_t *ptStatusH = NULL;
    arm2d_color_argb8888_acc_t *ptStatusV = NULL;

    if (NULL != (void *)(ptScratchMemory->pBuffer))
    {
        ptStatusH = (arm2d_color_argb8888_acc_t *)ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH + ptTargetRegionOnVirtualScreen->tSize.iWidth;
    }

    arm2d_local_location_t tOffset =
    {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

    const bool bAllowReverseH = arm2d_local_reverse_h_allowed(
                                    ptValidRegionOnVirtualScreen,
                                    ptTargetRegionOnVirtualScreen);
    const bool bAllowReverseV = arm2d_local_reverse_v_allowed(
                                    ptValidRegionOnVirtualScreen,
                                    ptTargetRegionOnVirtualScreen);

    /* Forward horizontal pass */
    if (ptThis->dir.bForwardHorizontal)
    {
        uint32_t *pwPixel = pwTarget;

        if (NULL != ptStatusV)
        {
            ptStatusV += tOffset.iY;
        }

        for (iY = 0; iY < iHeight; iY++)
        {
            if (NULL != ptStatusV && tOffset.iX > 0)
            {
                tAcc = *ptStatusV;
            }
            else
            {
                uint8_t *p = (uint8_t *)pwPixel;
                tAcc.hwB = p[0];
                tAcc.hwG = p[1];
                tAcc.hwR = p[2];
            }

            uint32_t *pwTargetPixel = pwPixel;

            for (iX = 0; iX < iWidth; iX++)
            {
                uint8_t *p = (uint8_t *)pwTargetPixel;
                tAcc.hwB += (p[0] - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (p[1] - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (p[2] - tAcc.hwR) * hwRatio >> 8;

                p[0] = (uint8_t)tAcc.hwB;
                p[1] = (uint8_t)tAcc.hwG;
                p[2] = (uint8_t)tAcc.hwR;
                pwTargetPixel++;
            }

            if (NULL != ptStatusV)
            {
                *ptStatusV++ = tAcc;
            }

            pwPixel += iTargetStride;
        }
    }

    /* Reverse horizontal pass (right to left, bottom to top) */
    if (ptThis->dir.bReverseHorizontal && bAllowReverseH)
    {
        uint32_t *pwPixel = &pwTarget[(iWidth - 1) + (iHeight - 1) * iTargetStride];

        for (iY = iHeight - 1; iY >= 0; iY--)
        {
            /* seed the accumulator from the right-most pixel of the row */
            uint8_t *p = (uint8_t *)pwPixel;
            tAcc.hwB = p[0];
            tAcc.hwG = p[1];
            tAcc.hwR = p[2];

            uint32_t *pwTargetPixel = pwPixel;

            for (iX = 0; iX < iWidth; iX++)
            {
                p = (uint8_t *)pwTargetPixel;
                tAcc.hwB += (p[0] - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (p[1] - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (p[2] - tAcc.hwR) * hwRatio >> 8;

                p[0] = (uint8_t)tAcc.hwB;
                p[1] = (uint8_t)tAcc.hwG;
                p[2] = (uint8_t)tAcc.hwR;
                pwTargetPixel--;
            }

            pwPixel -= iTargetStride;
        }
    }

    /* Forward vertical pass */
    if (ptThis->dir.bForwardVertical)
    {
        uint32_t *pwPixel = pwTarget;

        if (NULL != ptStatusH)
        {
            ptStatusH += tOffset.iX;
        }

        for (iX = 0; iX < iWidth; iX++)
        {
            if (NULL != ptStatusH && tOffset.iY > 0)
            {
                tAcc = *ptStatusH;
            }
            else
            {
                uint8_t *p = (uint8_t *)pwPixel;
                tAcc.hwB = p[0];
                tAcc.hwG = p[1];
                tAcc.hwR = p[2];
            }

            uint32_t *pwTargetPixel = pwPixel;

            for (iY = 0; iY < iHeight; iY++)
            {
                uint8_t *p = (uint8_t *)pwTargetPixel;
                tAcc.hwB += (p[0] - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (p[1] - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (p[2] - tAcc.hwR) * hwRatio >> 8;

                p[0] = (uint8_t)tAcc.hwB;
                p[1] = (uint8_t)tAcc.hwG;
                p[2] = (uint8_t)tAcc.hwR;
                pwTargetPixel += iTargetStride;
            }

            pwPixel++;

            if (NULL != ptStatusH)
            {
                *ptStatusH++ = tAcc;
            }
        }
    }

    /* Reverse vertical pass (bottom to top, right to left) */
    if (ptThis->dir.bReverseVertical && bAllowReverseV)
    {
        uint32_t *pwPixel = &pwTarget[(iWidth - 1) + (iHeight - 1) * iTargetStride];

        for (iX = iWidth - 1; iX >= 0; iX--)
        {
            /* seed the accumulator from the bottom pixel of the column */
            uint8_t *p = (uint8_t *)pwPixel;
            tAcc.hwB = p[0];
            tAcc.hwG = p[1];
            tAcc.hwR = p[2];

            uint32_t *pwTargetPixel = pwPixel;

            for (iY = 0; iY < iHeight; iY++)
            {
                p = (uint8_t *)pwTargetPixel;
                tAcc.hwB += (p[0] - tAcc.hwB) * hwRatio >> 8;
                tAcc.hwG += (p[1] - tAcc.hwG) * hwRatio >> 8;
                tAcc.hwR += (p[2] - tAcc.hwR) * hwRatio >> 8;

                p[0] = (uint8_t)tAcc.hwB;
                p[1] = (uint8_t)tAcc.hwG;
                p[2] = (uint8_t)tAcc.hwR;
                pwTargetPixel -= iTargetStride;
            }

            pwPixel--;
        }
    }
}

static void sw_arm2d_run_blur(uint8_t *buffer, int16_t stride,
                              arm2d_local_region_t *valid, arm2d_local_region_t *target,
                              uint16_t bit_depth, uint8_t blur_degree,
                              arm2d_local_scratch_mem_t *scratch_mem)
{
    arm2d_local_filter_iir_blur_descriptor_t dsc = {0};
    dsc.tScratchMemory = *scratch_mem;
    dsc.dir.bForwardHorizontal = 1;
    dsc.dir.bForwardVertical = 1;
    dsc.dir.bReverseHorizontal = 1;
    dsc.dir.bReverseVertical = 0;

    if (bit_depth == 32)
    {
        arm2d_local_argb8888_filter_iir_blur((uint32_t *)buffer, stride, valid, target,
                                             blur_degree, &dsc);
    }
    else if (bit_depth == 16)
    {
        arm2d_local_rgb565_filter_iir_blur((uint16_t *)buffer, stride, valid, target,
                                           blur_degree, &dsc);
    }
}

static uint8_t sw_arm2d_bytes_per_pixel(uint16_t bit_depth)
{
    if ((bit_depth == 16U) || (bit_depth == 32U))
    {
        return bit_depth / 8U;
    }
    return 0U;
}

static bool sw_arm2d_scratch_ensure(arm2d_local_scratch_mem_t *scratch_mem,
                                    uint16_t target_width, uint16_t target_height,
                                    uint8_t bytes_per_pixel)
{
    if ((scratch_mem->pBuffer != 0U)
        && (scratch_mem->hwTargetWidth >= target_width)
        && (scratch_mem->hwTargetHeight >= target_height)
        && (scratch_mem->chStatusBytesPerPixel == bytes_per_pixel))
    {
        return true;
    }

#if SW_ARM2D_BLUR_DOWNSCALE_FACTOR > 1
    uintptr_t scaled_buffer = scratch_mem->pScaledBuffer;
    uint16_t scaled_stride = scratch_mem->hwScaledStride;
    uint16_t scaled_rows = scratch_mem->hwScaledRows;
    uint8_t scaled_bytes_per_pixel = scratch_mem->chBytesPerPixel;
    size_t scaled_buffer_size = scratch_mem->scaledBufferSize;
#endif
    if (scratch_mem->pBuffer != 0U)
    {
        arm2d_local_scratch_memory_free(scratch_mem);
    }
    else
    {
        memset(&scratch_mem->tInfo, 0, sizeof(scratch_mem->tInfo));
    }
#if SW_ARM2D_BLUR_DOWNSCALE_FACTOR > 1
    scratch_mem->pScaledBuffer = scaled_buffer;
    scratch_mem->hwScaledStride = scaled_stride;
    scratch_mem->hwScaledRows = scaled_rows;
    scratch_mem->chBytesPerPixel = scaled_bytes_per_pixel;
    scratch_mem->scaledBufferSize = scaled_buffer_size;
#endif

    uint16_t item_size = bytes_per_pixel == 4U
                         ? sizeof(arm2d_color_argb8888_acc_t)
                         : sizeof(arm2d_color_rgb565_t);
    if (NULL == arm2d_local_scratch_memory_new(
            scratch_mem, item_size, target_width + target_height,
            __alignof__(uint32_t)))
    {
        return false;
    }
    scratch_mem->hwTargetWidth = target_width;
    scratch_mem->hwTargetHeight = target_height;
    scratch_mem->chStatusBytesPerPixel = bytes_per_pixel;
    return true;
}

#if SW_ARM2D_BLUR_DOWNSCALE_FACTOR > 1
static void sw_arm2d_nearest_scale(uint8_t *target_buffer,
                                   const uint8_t *source_buffer,
                                   uint16_t source_width,
                                   uint16_t source_height,
                                   uint16_t source_stride,
                                   uint16_t target_stride,
                                   uint8_t factor,
                                   uint16_t target_width,
                                   uint16_t target_height,
                                   uint8_t bytes_per_pixel,
                                   bool scale_up)
{
    struct acc_engine *acc = gui_get_acc();
    if ((acc != NULL) && (acc->blur != NULL) && (acc->blur->scale != NULL)
        && acc->blur->scale(target_buffer, source_buffer, source_width, source_height,
                            source_stride, target_stride, factor, target_width,
                            target_height, bytes_per_pixel, scale_up))
    {
        return;
    }

    uint8_t copy_size = (scale_up && (bytes_per_pixel == 4U)) ? 3U : bytes_per_pixel;
    for (uint32_t y = 0; y < target_height; y++)
    {
        uint32_t source_y = scale_up ? y / factor : y * factor;
        if (source_y >= source_height)
        {
            source_y = source_height - 1U;
        }
        for (uint32_t x = 0; x < target_width; x++)
        {
            uint32_t source_x = scale_up ? x / factor : x * factor;
            if (source_x >= source_width)
            {
                source_x = source_width - 1U;
            }
            uint8_t *destination = target_buffer
                                   + ((size_t)y * target_stride + x) * bytes_per_pixel;
            const uint8_t *source = source_buffer
                                    + ((size_t)source_y * source_stride + source_x)
                                    * bytes_per_pixel;
            memcpy(destination, source, copy_size);
        }
    }
}

static bool sw_arm2d_scaled_buffer_ensure(arm2d_local_scratch_mem_t *scratch_mem,
                                          uint16_t scaled_width, uint16_t scaled_height,
                                          uint8_t bytes_per_pixel)
{
    uint16_t stride = SW_ARM2D_ROUND_UP_8((uint32_t)scaled_width + 7U);
    uint16_t rows = SW_ARM2D_ROUND_UP_8((uint32_t)scaled_height + 7U);
    size_t required_size = (size_t)stride * rows * bytes_per_pixel;
    if ((scratch_mem->pScaledBuffer != 0U)
        && (scratch_mem->scaledBufferSize >= required_size)
        && (scratch_mem->hwScaledStride >= stride)
        && (scratch_mem->hwScaledRows >= rows)
        && (scratch_mem->chBytesPerPixel == bytes_per_pixel))
    {
        return true;
    }

    gui_free((void *)scratch_mem->pScaledBuffer);
    scratch_mem->pScaledBuffer = (uintptr_t)gui_malloc(required_size);
    if (scratch_mem->pScaledBuffer == 0U)
    {
        return false;
    }
    scratch_mem->hwScaledStride = stride;
    scratch_mem->hwScaledRows = rows;
    scratch_mem->chBytesPerPixel = bytes_per_pixel;
    scratch_mem->scaledBufferSize = required_size;
    return true;
}

static bool sw_arm2d_scaled_blur(uint8_t *buffer, uint16_t buffer_stride,
                                 uint16_t bit_depth, const gui_rect_t *buffer_rect,
                                 const gui_rect_t *valid_rect, const gui_rect_t *target_rect,
                                 uint8_t blur_degree, arm2d_local_scratch_mem_t *scratch_mem)
{
    uint8_t bytes_per_pixel = sw_arm2d_bytes_per_pixel(bit_depth);
    uint32_t factor = SW_ARM2D_BLUR_DOWNSCALE_FACTOR;
    uint16_t target_width = target_rect->x2 - target_rect->x1 + 1;
    uint16_t target_height = target_rect->y2 - target_rect->y1 + 1;
    uint16_t scaled_target_width = ((uint32_t)target_width + factor - 1U) / factor;
    uint16_t scaled_target_height = ((uint32_t)target_height + factor - 1U) / factor;
    if ((bytes_per_pixel == 0U) || (scratch_mem == NULL))
    {
        return false;
    }

    bool full_target = (valid_rect->x1 == target_rect->x1)
                       && (valid_rect->x2 == target_rect->x2)
                       && (valid_rect->y1 == target_rect->y1)
                       && (valid_rect->y2 == target_rect->y2);
    bool full_width_y_slice = (valid_rect->x1 == target_rect->x1)
                              && (valid_rect->x2 == target_rect->x2)
                              && (valid_rect->y1 >= target_rect->y1)
                              && (valid_rect->y2 <= target_rect->y2);
    if (!full_target && !full_width_y_slice)
    {
        return false;
    }
    if (!sw_arm2d_scratch_ensure(scratch_mem, scaled_target_width,
                                 scaled_target_height, bytes_per_pixel))
    {
        return false;
    }

    uint16_t source_width = valid_rect->x2 - valid_rect->x1 + 1;
    uint16_t source_height = valid_rect->y2 - valid_rect->y1 + 1;
    uint32_t scaled_y = ((uint32_t)(valid_rect->y1 - target_rect->y1) + factor - 1U) / factor;
    uint32_t scaled_y_end = ((uint32_t)(valid_rect->y2 - target_rect->y1 + 1)
                             + factor - 1U) / factor;
    uint16_t scaled_height = (uint16_t)(scaled_y_end - scaled_y);
    if ((scaled_height == 0U) || (scaled_y + scaled_height > scaled_target_height)
        || !sw_arm2d_scaled_buffer_ensure(scratch_mem, scaled_target_width,
                                          scaled_height + 1U, bytes_per_pixel))
    {
        return false;
    }

    uint8_t *source_start = buffer
                            + (((size_t)(valid_rect->y1 - buffer_rect->y1) * buffer_stride)
                               + (valid_rect->x1 - buffer_rect->x1)) * bytes_per_pixel;
    uint8_t *scaled_start = (uint8_t *)scratch_mem->pScaledBuffer;
    sw_arm2d_nearest_scale(scaled_start, source_start, source_width, source_height,
                           buffer_stride, scratch_mem->hwScaledStride, factor,
                           scaled_target_width, scaled_height, bytes_per_pixel, false);

    arm2d_local_region_t scaled_valid =
    {
        .tLocation = {.iX = 0, .iY = (int16_t)scaled_y},
        .tSize = {.iWidth = scaled_target_width, .iHeight = scaled_height},
    };
    arm2d_local_region_t scaled_target =
    {
        .tLocation = {.iX = 0, .iY = 0},
        .tSize = {.iWidth = scaled_target_width, .iHeight = scaled_target_height},
    };
    sw_arm2d_run_blur(scaled_start, scratch_mem->hwScaledStride, &scaled_valid,
                      &scaled_target, bit_depth, blur_degree, scratch_mem);
    memcpy(scaled_start + (size_t)scaled_height * scratch_mem->hwScaledStride * bytes_per_pixel,
           scaled_start + (size_t)(scaled_height - 1U) * scratch_mem->hwScaledStride
           * bytes_per_pixel,
           (size_t)scratch_mem->hwScaledStride * bytes_per_pixel);

    sw_arm2d_nearest_scale(source_start, scaled_start, scaled_target_width,
                           scaled_height + 1U, scratch_mem->hwScaledStride,
                           buffer_stride, factor, source_width, source_height,
                           bytes_per_pixel, true);
    return true;
}
#endif

void sw_arm_2d_blur(uint8_t *buffer, uint16_t buffer_stride, uint16_t bit_depth,
                    const gui_rect_t *buffer_rect, const gui_rect_t *valid_rect,
                    const gui_rect_t *target_rect, uint8_t blur_degree, void *cache_mem)
{
    if ((buffer == NULL) || (buffer_rect == NULL) || (valid_rect == NULL)
        || (target_rect == NULL) || (buffer_stride == 0U)
        || (valid_rect->x1 < buffer_rect->x1) || (valid_rect->y1 < buffer_rect->y1)
        || (valid_rect->x2 > buffer_rect->x2) || (valid_rect->y2 > buffer_rect->y2)
        || (valid_rect->x1 < target_rect->x1) || (valid_rect->y1 < target_rect->y1)
        || (valid_rect->x2 > target_rect->x2) || (valid_rect->y2 > target_rect->y2))
    {
        return;
    }

    uint8_t bytes_per_pixel = sw_arm2d_bytes_per_pixel(bit_depth);
    if (bytes_per_pixel == 0U)
    {
        return;
    }
    uint8_t *valid_start = buffer
                           + (((size_t)(valid_rect->y1 - buffer_rect->y1) * buffer_stride)
                              + (valid_rect->x1 - buffer_rect->x1)) * bytes_per_pixel;
    arm2d_local_region_t valid =
    {
        .tLocation = {.iX = valid_rect->x1, .iY = valid_rect->y1},
        .tSize =
        {
            .iWidth = valid_rect->x2 - valid_rect->x1 + 1,
            .iHeight = valid_rect->y2 - valid_rect->y1 + 1,
        },
    };
    arm2d_local_region_t target =
    {
        .tLocation = {.iX = target_rect->x1, .iY = target_rect->y1},
        .tSize =
        {
            .iWidth = target_rect->x2 - target_rect->x1 + 1,
            .iHeight = target_rect->y2 - target_rect->y1 + 1,
        },
    };
    arm2d_local_scratch_mem_t local_scratch_mem = {0};
    arm2d_local_scratch_mem_t *mem_for_blur = (arm2d_local_scratch_mem_t *)cache_mem;
    if (mem_for_blur == NULL)
    {
        mem_for_blur = &local_scratch_mem;
    }

#if SW_ARM2D_BLUR_DOWNSCALE_FACTOR > 1
    if ((cache_mem != NULL)
        && sw_arm2d_scaled_blur(buffer, buffer_stride, bit_depth, buffer_rect,
                                valid_rect, target_rect, blur_degree, mem_for_blur))
    {
        return;
    }
#endif

    if ((cache_mem != NULL)
        && !sw_arm2d_scratch_ensure(mem_for_blur, target.tSize.iWidth,
                                    target.tSize.iHeight, bytes_per_pixel))
    {
        return;
    }
    sw_arm2d_run_blur(valid_start, buffer_stride, &valid, &target, bit_depth,
                      blur_degree, mem_for_blur);
}

void sw_arm_2d_create(void **mem)
{
    if ((mem == NULL) || (*mem != NULL))
    {
        return;
    }
    arm2d_local_scratch_mem_t *scratch_mem = gui_malloc(sizeof(arm2d_local_scratch_mem_t));
    GUI_ASSERT(scratch_mem != NULL);
    if (scratch_mem != NULL)
    {
        memset(scratch_mem, 0, sizeof(*scratch_mem));
    }
    *mem = (void *)scratch_mem;
}

void sw_arm_2d_depose(void **mem)
{
    if (*mem != NULL)
    {
        arm2d_local_scratch_mem_t *scratch_mem = (arm2d_local_scratch_mem_t *)*mem;
#if SW_ARM2D_BLUR_DOWNSCALE_FACTOR > 1
        gui_free((void *)scratch_mem->pScaledBuffer);
        scratch_mem->pScaledBuffer = 0U;
#endif
        if (scratch_mem->pBuffer != 0U)
        {
            arm2d_local_scratch_memory_free(scratch_mem);
        }
    }
    gui_free(*mem);
    *mem = NULL;
}

gui_blur_ops_t sw_arm2d_blur_ops =
{
    .prepare = sw_arm_2d_create,
    .process = sw_arm_2d_blur,
    .release = sw_arm_2d_depose,
};


