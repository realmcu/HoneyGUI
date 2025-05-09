/**
 * @file acc_init.c
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stdio.h>
//#include "acc_sw_rle.h"
#include "acc_init.h"


extern void sw_acc_init(void);
extern void sw_acc_blit(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);

/**
 * Represents the absolute bounds of an image.
 * The `rect` parameter defines the rectangular area that specifies
 * the absolute range of the image in terms of its position and size.
 *
 * @param rect A structure or object that defines the absolute range
 *             of the image, typically including properties such as
 *             x, y, width, and height.
 */
void gui_acc_blit_to_dc(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect)
{
    gui_get_acc()->blit(image, dc, rect);
}
