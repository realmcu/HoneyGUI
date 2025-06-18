/*
 * Copyright (C) 2024 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        __arm_2d_filter_iir_blur.h
 * Description:  APIs for IIR Blur
 *
 * $Date:        25. April 2025
 * $Revision:    V.2.0.0
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#ifndef __ARM_2D_FILTER_IIR_BLUR_H__
#define __ARM_2D_FILTER_IIR_BLUR_H__

/*============================ INCLUDES ======================================*/

#ifdef   __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>


/*============================ MACROS ========================================*/

#ifndef   __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif

#ifndef __WEAK
#define __WEAK  __attribute__((weak))
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/* arm_2d_general_types */

/*!
 * \brief the enumeration type for describing memory types
 *
 */
typedef enum
{
    ARM_2D_MEM_TYPE_UNSPECIFIED,                                                //!< normal memory, we don't know its characterisics
    ARM_2D_MEM_TYPE_SLOW,                                                       //!< for slow memories, such as SDRAM, DDRAM, external memory etc
    ARM_2D_MEM_TYPE_FAST,                                                       //!< for fast memories, such as TCM, SRAM etc.
} arm_2d_mem_type_t;


/*!
 * \brief a type for the size of an rectangular area
 *
 */
typedef struct arm_2d_size_t
{
    int16_t iWidth;                     //!< width of an rectangular area
    int16_t iHeight;                    //!< height of an rectangular area
} arm_2d_size_t;

typedef struct arm_2d_location_t
{
    int16_t iX;                         //!< x in Cartesian coordinate system
    int16_t iY;                         //!< y in Cartesian coordinate system
} arm_2d_location_t;

/*!
 * \brief a type for an rectangular area
 *
 */
typedef struct arm_2d_region_t
{
    arm_2d_location_t tLocation; //!< the location (top-left corner)
    arm_2d_size_t tSize;         //!< the size
} arm_2d_region_t;



/*!
 * \brief scratch memory descriptor
 * \note "manually" derived from __arm_2d_mem_info_t
 */
typedef struct arm_2d_scratch_mem_t
{
    uint32_t u24SizeInByte      : 24;    //!< the memory size in Byte
    uint32_t u2ItemSize         : 3;     //!< the size of the data item
    uint32_t u2Align            : 3;     //!< the alignment
    uint32_t u2Type             : 2;     //!< The memory type define in enum arm_2d_mem_type_t

    uintptr_t pBuffer;
} arm_2d_scratch_mem_t;

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

/* arm_2d_iir_blur_types*/
typedef enum
{
    ARM_IIR_BLUR_MODE_HORIZONTAL            = (1 << 0),
    ARM_IIR_BLUR_MODE_VERTICAL              = (1 << 1),

    ARM_IIR_BLUR_MODE_REVERSE_HORIZONTAL    = (1 << 2),
    ARM_IIR_BLUR_MODE_REVERSE_VERTICAL      = (1 << 3),

    ARM_IIR_BLUR_MODE_FORWARD               = ARM_IIR_BLUR_MODE_HORIZONTAL
                                              | ARM_IIR_BLUR_MODE_VERTICAL,

    ARM_IIR_BLUR_MODE_REVERSE               = ARM_IIR_BLUR_MODE_REVERSE_HORIZONTAL
                                              | ARM_IIR_BLUR_MODE_REVERSE_VERTICAL,

    ARM_IIR_BLUR_MODE_DEFAULT               = 0,
    ARM_IIR_BLUR_MODE_BEST                  = ARM_IIR_BLUR_MODE_FORWARD
                                              | ARM_IIR_BLUR_MODE_REVERSE,

    ARM_IIR_BLUR_MODE_BEST_HORIZONTAL       = ARM_IIR_BLUR_MODE_HORIZONTAL
                                              | ARM_IIR_BLUR_MODE_REVERSE_HORIZONTAL,

    ARM_IIR_BLUR_MODE_BEST_VERTICAL         = ARM_IIR_BLUR_MODE_VERTICAL
                                              | ARM_IIR_BLUR_MODE_REVERSE_VERTICAL,
} arm_2d_iir_blur_mode_t;


typedef struct arm_2d_filter_iir_blur_descriptor_t
{
    arm_2d_scratch_mem_t tScratchMemory;
    union
    {
        uint8_t chBlurMode;
        struct
        {
            uint8_t bForwardHorizontal  : 1;
            uint8_t bForwardVertical    : 1;
            uint8_t bReverseHorizontal  : 1;
            uint8_t bReverseVertical    : 1;
        } dir;
    } blurmode;

    uint8_t chBlurDegree;

} arm_2d_filter_iir_blur_descriptor_t;

typedef struct
{
    uint16_t        B;
    uint16_t        G;
    uint16_t        R;
    uint16_t        A;
} __arm_2d_color_fast_rgb_t;

typedef struct __arm_2d_iir_blur_acc_cccn888_t
{
    uint16_t hwB;
    uint16_t hwG;
    uint16_t hwR;
} __arm_2d_iir_blur_acc_cccn888_t;

typedef __arm_2d_iir_blur_acc_cccn888_t __arm_2d_iir_blur_acc_rgb565_t;


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

__STATIC_INLINE void __arm_2d_rgb565_unpack_comp(uint16_t hwColor,
                                                 __arm_2d_color_fast_rgb_t *ptRGB)
{
    assert(NULL != ptRGB);

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

__STATIC_INLINE uint16_t __arm_2d_rgb565_pack(__arm_2d_color_fast_rgb_t *ptRGB)
{
    assert(NULL != ptRGB);

    arm_2d_color_rgb565_t tOutput =
    {
        .BGR.u5B = (uint16_t)(ptRGB->B >> 3),
        .BGR.u6G = (uint16_t)(ptRGB->G >> 2),
        .BGR.u5R = (uint16_t)(ptRGB->R >> 3),
    };
    return tOutput.tValue;
}
/*!
 * \brief set IIR blur working mode
 * \param[in] ptOP the control block
 * \param[in] chModeMask working mode ARM_IIR_BLUR_MODE_xxxx
 * \return none
 */
arm_2d_scratch_mem_t *arm_2d_scratch_memory_new(arm_2d_scratch_mem_t *ptMemory,
                                                uint16_t hwItemSize,
                                                uint16_t hwItemCount,
                                                uint16_t hwAlignment,
                                                arm_2d_mem_type_t tType);

arm_2d_scratch_mem_t *arm_2d_scratch_memory_free(arm_2d_scratch_mem_t *ptMemory);

void arm_2dp_filter_iir_blur_mode_set(arm_2d_filter_iir_blur_descriptor_t *ptOP,
                                      uint_fast8_t chModeMask);

__WEAK void __arm_2d_impl_rgb565_filter_iir_blur(
    uint16_t *__restrict phwTarget,
    int16_t iTargetStride,
    arm_2d_region_t *__restrict ptValidRegionOnVirtualScreen,
    arm_2d_region_t *ptTargetRegionOnVirtualScreen,
    uint8_t chBlurDegree,
    arm_2d_filter_iir_blur_descriptor_t *ptThis);

#ifdef   __cplusplus
}
#endif


#endif /* __ARM_2D_FILTER_IIR_BLUR_H__ */