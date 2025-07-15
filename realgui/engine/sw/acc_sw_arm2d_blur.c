#include "guidef.h"
#include "gui_api.h"
#include "gui_post_process.h"
#include "gui_matrix.h"

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

void *arm2d_local_allocate_scratch_memory(uint32_t wSize,
                                          uint_fast8_t nAlign)
{
    GUI_UNUSED(nAlign);

    /* ensure nAlign is 2^n */
    GUI_ASSERT((((~nAlign) + 1) & nAlign) == nAlign);

    void *pBuff = malloc(wSize);
    GUI_ASSERT(0 == ((uintptr_t)pBuff & (nAlign - 1)));

    return pBuff;
}

void arm2d_local_free_scratch_memory(void *pBuff)
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
    GUI_ASSERT(NULL != ptRGB);

    arm_2d_color_rgb565_t tOutput =
    {
        .BGR.u5B = (uint16_t)(ptRGB->B >> 3),
        .BGR.u6G = (uint16_t)(ptRGB->G >> 2),
        .BGR.u5R = (uint16_t)(ptRGB->R >> 3),
    };
    return tOutput.tValue;
}

static inline void arm2d_local_rgb565_unpack_comp(uint16_t hwColor,
                                                  arm2d_local_color_fast_rgb_t *ptRGB)
{
    GUI_ASSERT(NULL != ptRGB);

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

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

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

                tAcc.hwB += (tPixel.B - tAcc.hwB) * hwRatio >> 8;  tPixel.B = tAcc.hwB;
                tAcc.hwG += (tPixel.G - tAcc.hwG) * hwRatio >> 8;  tPixel.G = tAcc.hwG;
                tAcc.hwR += (tPixel.R - tAcc.hwR) * hwRatio >> 8;  tPixel.R = tAcc.hwR;

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
}

void sw_arm_2d_blur(struct gui_dispdev *dc, gui_rect_t *rect, uint8_t blur_degree, void *cache_mem)
{
    gui_rect_t blur_rect = {0};
    gui_rect_t target_rect = {.x1 = 0, .x2 = dc->screen_width - 1, .y1 = 0, .y2 = dc->screen_height - 1};
    if (!rect_intersect(&blur_rect, rect, &dc->section))
    {
        return;
    }
    if (!rect_intersect(&target_rect, &target_rect, rect))
    {
        return;
    }
    uint16_t *buffer = (uint16_t *)(dc->frame_buf + \
                                    ((blur_rect.y1 - dc->section.y1) * dc->fb_width + blur_rect.x1) * dc->bit_depth / 8);
    arm2d_local_region_t valid, target;
    valid.tLocation.iX = blur_rect.x1;
    valid.tLocation.iY = blur_rect.y1;
    valid.tSize.iWidth = blur_rect.x2 - blur_rect.x1 + 1;
    valid.tSize.iHeight = blur_rect.y2 - blur_rect.y1 + 1;
    target.tLocation.iX = target_rect.x1;
    target.tLocation.iY = target_rect.y1;
    target.tSize.iWidth = target_rect.x2 - target_rect.x1 + 1;
    target.tSize.iHeight = target_rect.y2 - target_rect.y1 + 1;
    arm2d_local_scratch_mem_t local_scratch_mem = {0};
    arm2d_local_scratch_mem_t *mem_for_blur = (arm2d_local_scratch_mem_t *)cache_mem;
    if (cache_mem == NULL)
    {
        mem_for_blur = &local_scratch_mem;
    }
    arm2d_local_filter_iir_blur_descriptor_t dsc = {0};
    dsc.tScratchMemory = *mem_for_blur;
    dsc.dir.bForwardHorizontal = 1;
    dsc.dir.bForwardVertical = 1;
    dsc.dir.bReverseHorizontal = 0;
    dsc.dir.bReverseVertical = 0;
    arm2d_local_rgb565_filter_iir_blur((uint16_t *)buffer, dc->fb_width, &valid, &target, blur_degree,
                                       &dsc);
}

void sw_arm_2d_create(gui_rect_t *rect, void **mem)
{
    if (*mem != NULL)
    {
        return;
    }
    uint16_t w = rect->x2 - rect->x1 + 1;
    uint16_t h = rect->y2 - rect->y1 + 1;
    arm2d_local_scratch_mem_t *scratch_mem = gui_malloc(sizeof(arm2d_local_scratch_mem_t));

    if (NULL ==  arm2d_local_scratch_memory_new(
            scratch_mem,
            sizeof(arm2d_color_rgb565_t),
            (w + h),
            __alignof__(arm2d_color_rgb565_t)))
    {
        memset(scratch_mem, 0, sizeof(arm2d_local_scratch_mem_t));
    }
    *mem = (void *)scratch_mem;
}

void sw_arm_2d_depose(void **mem)
{
    if (*mem != NULL)
    {
        arm2d_local_scratch_memory_free((arm2d_local_scratch_mem_t *)*mem);
    }
    gui_free(*mem);
    *mem = NULL;
}

void sw_arm2d_blur_init(void)
{
    blur_depose = sw_arm_2d_depose;
    blur_prepare = sw_arm_2d_create;
}