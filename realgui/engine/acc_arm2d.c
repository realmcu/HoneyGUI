#include "__arm_2d_filter_iir_blur.h"
#include "guidef.h"
#include "gui_api.h"
#include "gui_post_process.h"
#include "gui_matrix.h"

void arm_2d_blur(struct gui_dispdev *dc, gui_rect_t *rect, uint8_t blur_degree, void *cache_mem)
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
    arm_2d_region_t valid, target;
    valid.tLocation.iX = blur_rect.x1;
    valid.tLocation.iY = blur_rect.y1;
    valid.tSize.iWidth = blur_rect.x2 - blur_rect.x1 + 1;
    valid.tSize.iHeight = blur_rect.y2 - blur_rect.y1 + 1;
    target.tLocation.iX = target_rect.x1;
    target.tLocation.iY = target_rect.y1;
    target.tSize.iWidth = target_rect.x2 - target_rect.x1 + 1;
    target.tSize.iHeight = target_rect.y2 - target_rect.y1 + 1;
    arm_2d_scratch_mem_t local_scratch_mem = {0};
    arm_2d_scratch_mem_t *mem_for_blur = (arm_2d_scratch_mem_t *)cache_mem;
    if (cache_mem == NULL)
    {
        mem_for_blur = &local_scratch_mem;
    }
    arm_2d_filter_iir_blur_descriptor_t dsc = {0};
    dsc.tScratchMemory = *mem_for_blur;
    arm_2dp_filter_iir_blur_mode_set(&dsc, ARM_IIR_BLUR_MODE_DEFAULT);
    __arm_2d_impl_rgb565_filter_iir_blur((uint16_t *)buffer, dc->fb_width, &valid, &target, blur_degree,
                                         &dsc);
}

void arm_2d_create(gui_rect_t *rect, void **mem)
{
    if (*mem != NULL)
    {
        return;
    }
    uint16_t w = rect->x2 - rect->x1 + 1;
    uint16_t h = rect->y2 - rect->y1 + 1;
    arm_2d_scratch_mem_t *scratch_mem = gui_malloc(sizeof(arm_2d_scratch_mem_t));

    if (NULL ==  arm_2d_scratch_memory_new(
            scratch_mem,
            sizeof(__arm_2d_iir_blur_acc_rgb565_t),
            (w + h
#if __ARM_2D_HAS_HELIUM__
             + 14
#endif
            ),
            __alignof__(__arm_2d_iir_blur_acc_rgb565_t),
            ARM_2D_MEM_TYPE_FAST))
    {
        memset(scratch_mem, 0, sizeof(arm_2d_scratch_mem_t));
    }
    *mem = (void *)scratch_mem;
}

void arm_2d_depose(void **mem)
{
    if (*mem != NULL)
    {
        arm_2d_scratch_memory_free((arm_2d_scratch_mem_t *)*mem);
    }
    gui_free(*mem);
    *mem = NULL;
}

void arm_2d_pointer_init(void)
{
    blur_depose = arm_2d_depose;
    blur_prepare = arm_2d_create;
}