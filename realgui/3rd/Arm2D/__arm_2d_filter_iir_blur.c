/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        __arm_2d_filter_iir_blur.c
 * Description:  APIs for IIR Blur
 *
 * $Date:        25. April 2025
 * $Revision:    V.2.0.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */


/*============================ INCLUDES ======================================*/

#include <stdlib.h>
#include "__arm_2d_filter_iir_blur.h"
#include "gui_api_os.h"
#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#undef this
#define this            (*ptThis)

#ifndef ARM_2D_UNUSED
#   define ARM_2D_UNUSED(__VAR)     (void)(__VAR)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

void *__arm_2d_allocate_scratch_memory(uint32_t wSize,
                                       uint_fast8_t nAlign,
                                       arm_2d_mem_type_t tType)
{
    ARM_2D_UNUSED(nAlign);
    ARM_2D_UNUSED(tType);

    /* ensure nAlign is 2^n */
    assert((((~nAlign) + 1) & nAlign) == nAlign);

    void *pBuff = gui_malloc(wSize);
    assert(0 == ((uintptr_t)pBuff & (nAlign - 1)));

    return pBuff;
}

void __arm_2d_free_scratch_memory(arm_2d_mem_type_t tType,
                                  void *pBuff)
{
    ARM_2D_UNUSED(tType);

    gui_free(pBuff);
}

arm_2d_scratch_mem_t *arm_2d_scratch_memory_new(arm_2d_scratch_mem_t *ptMemory,
                                                uint16_t hwItemSize,
                                                uint16_t hwItemCount,
                                                uint16_t hwAlignment,
                                                arm_2d_mem_type_t tType)
{
    size_t tSize = (size_t)hwItemSize * (size_t)hwItemCount;
    do
    {
        if (NULL == ptMemory)
        {
            assert(false);
            break;
        }
        else if (0 == tSize)
        {
            break;
        }

        tSize = (tSize + 3) & ~3;

        ptMemory->pBuffer
            = (uintptr_t)__arm_2d_allocate_scratch_memory(tSize + 4,
                                                          hwAlignment,
                                                          tType);

        ptMemory->u24SizeInByte = tSize;
        ptMemory->u2Align = hwAlignment;
        ptMemory->u2ItemSize = hwItemSize;
        ptMemory->u2Type = tType;

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

arm_2d_scratch_mem_t *arm_2d_scratch_memory_free(arm_2d_scratch_mem_t *ptMemory)
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

        size_t tSize = ptMemory->u24SizeInByte;

        /* check canary */
        if (*(volatile uint32_t *)((uintptr_t)(ptMemory->pBuffer) + tSize) != 0xCAFE0ACE)
        {
            assert(false);
        }

        __arm_2d_free_scratch_memory((arm_2d_mem_type_t)ptMemory->u2Type,
                                     (void *)(ptMemory->pBuffer));
        memset(ptMemory, 0, sizeof(arm_2d_scratch_mem_t));

    }
    while (0);

    return ptMemory;
}

void arm_2dp_filter_iir_blur_mode_set(arm_2d_filter_iir_blur_descriptor_t *ptOP,
                                      uint_fast8_t chModeMask)
{
    arm_2d_filter_iir_blur_descriptor_t *ptThis = ptOP;

    this.blurmode.chBlurMode = chModeMask;

    if (0 == this.blurmode.chBlurMode)
    {
        /* use the default value */
        this.blurmode.dir.bForwardHorizontal = true;
        this.blurmode.dir.bForwardVertical = true;

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
        this.blurmode.dir.bReverseHorizontal = true;
        this.blurmode.dir.bReverseVertical = true;
#endif
    }

}

/* default low level implementation */
__WEAK void __arm_2d_impl_rgb565_filter_iir_blur(
    uint16_t *__restrict phwTarget,
    int16_t iTargetStride,
    arm_2d_region_t *__restrict ptValidRegionOnVirtualScreen,
    arm_2d_region_t *ptTargetRegionOnVirtualScreen,
    uint8_t chBlurDegree,
    arm_2d_filter_iir_blur_descriptor_t *ptThis)
{
    arm_2d_scratch_mem_t *ptScratchMemory = &this.tScratchMemory;

    int_fast16_t iWidth = ptValidRegionOnVirtualScreen->tSize.iWidth;
    int_fast16_t iHeight = ptValidRegionOnVirtualScreen->tSize.iHeight;

    int16_t iY, iX;
    uint16_t hwRatio = 256 - chBlurDegree;
    __arm_2d_iir_blur_acc_rgb565_t tAcc;
    __arm_2d_iir_blur_acc_rgb565_t *ptStatusH = NULL;
    __arm_2d_iir_blur_acc_rgb565_t *ptStatusV = NULL;
    __arm_2d_color_fast_rgb_t tPixel;

    if (NULL != (void *)(ptScratchMemory->pBuffer))
    {
        ptStatusH = (__arm_2d_iir_blur_acc_rgb565_t *)ptScratchMemory->pBuffer;
        ptStatusV = ptStatusH
#if __ARM_2D_HAS_HELIUM__
                    + 7
#endif
                    + ptTargetRegionOnVirtualScreen->tSize.iWidth;
    }

    /* calculate the offset between the target region and the valid region */
    arm_2d_location_t tOffset =
    {
        .iX = ptValidRegionOnVirtualScreen->tLocation.iX - ptTargetRegionOnVirtualScreen->tLocation.iX,
        .iY = ptValidRegionOnVirtualScreen->tLocation.iY - ptTargetRegionOnVirtualScreen->tLocation.iY,
    };

#if defined(__ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__)                    \
 && __ARM_2D_CFG_USE_IIR_BLUR_REVERSE_PATH__
    const bool bAllowReversePath = (1 == arm_2d_is_region_inside_target(
                                        ptTargetRegionOnVirtualScreen,
                                        ptValidRegionOnVirtualScreen));
#else
    const bool bAllowReversePath = false;
#endif
    /*
         Virtual Screen
         +--------------------------------------------------------------+
         |                                                              |
         |        Target Region                                         |
         |       +-------------------------------------------+          |
         |       |                                           |          |
         |       |                  +-------------------+    |          |
         |       |                  | Valid Region      |    |          |
         |       |                  |                   |    |          |
         |       |                  +-------------------+    |          |
         |       |                                           |          |
         |       |                                           |          |
         |       +-------------------------------------------+          |
         +--------------------------------------------------------------+

         NOTE: 1. Both the Target Region and the Valid Region are relative
                  regions of the virtual Screen in this function.
               2. The Valid region is always inside the Target Region.
               3. tOffset is the relative location between the Valid Region
                  and the Target Region.
               4. The Valid Region marks the location and size of the current
                  working buffer on the virtual screen. Only the valid region
                  contains a valid buffer.
     */
    if (this.blurmode.dir.bForwardHorizontal)
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

                __arm_2d_rgb565_unpack_comp(*phwPixel, &tPixel);
                tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;

            }

            uint16_t *phwTargetPixel = phwPixel;

            for (iX = 0; iX < iWidth; iX++)
            {

                __arm_2d_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

                *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);

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

    /* rows reverse path */
    if (this.blurmode.dir.bReverseHorizontal && bAllowReversePath)
    {
        uint16_t *phwPixel = &(phwTarget[(iWidth - 1) + (iHeight - 1) * iTargetStride]);

        for (iY = iHeight - 1; iY > 0; iY--)
        {

            /* initialize the accumulator */
            do
            {

                __arm_2d_rgb565_unpack_comp(*phwPixel, &tPixel);
                tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;

            }
            while (0);

            uint16_t *phwTargetPixel = phwPixel;

            for (iX = 0; iX < iWidth; iX++)
            {

                __arm_2d_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

                *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);

                phwTargetPixel--;
            }

            phwPixel -= iTargetStride;
        }
    }

    if (this.blurmode.dir.bForwardVertical)
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

                __arm_2d_rgb565_unpack_comp(*phwPixel, &tPixel);
                tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;

            }

            uint16_t *phwTargetPixel = phwPixel;

            for (iY = 0; iY < iHeight; iY++)
            {

                __arm_2d_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

                *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);

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

    if (this.blurmode.dir.bReverseVertical && bAllowReversePath)
    {
        uint16_t *phwPixel = &(phwTarget[iWidth - 1 + (iHeight - 1) * iTargetStride]);

        /* columns reverse path */
        for (iX = iWidth - 1; iX > 0; iX--)
        {
            /* initialize the accumulator */
            do
            {

                __arm_2d_rgb565_unpack_comp(*phwPixel, &tPixel);
                tAcc = *(__arm_2d_iir_blur_acc_rgb565_t *)&tPixel;

            }
            while (0);

            uint16_t *phwTargetPixel = phwPixel;

            for (iY = 0; iY < iHeight; iY++)
            {

                __arm_2d_rgb565_unpack_comp(*phwTargetPixel, &tPixel);

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

                *phwTargetPixel = __arm_2d_rgb565_pack(&tPixel);

                phwTargetPixel -= iTargetStride;
            }

            phwPixel--;
        }
    }
}

#ifdef   __cplusplus
}
#endif
