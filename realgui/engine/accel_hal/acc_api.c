/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include "acc_api.h"
#include "acc_sw.h"


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
    if (gui_get_acc()->blit != NULL)
    {
        gui_get_acc()->blit(image, dc, rect);
    }
    else
    {
        // Fallback to software rendering if no hardware acceleration is available
        sw_acc_blit(image, dc, rect);
    }
}
