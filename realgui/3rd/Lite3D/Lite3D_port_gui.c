/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file Lite3D_port.c
  * @brief Custom configuration
  * @details Custom configuration
  * @author sienna_shen@realsil.com.cn
  * @date 2025/4/11
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */


#include <stdlib.h>
#include "l3.h"


#if LITE3D_PORT_GUI == 1
#include "gui_api.h"
#include "def_file.h"
#include "draw_img.h"

void *l3_custom_malloc(size_t size)
{
    return gui_malloc(size);
}
void l3_custom_free(void *ptr)
{
    gui_free(ptr);
}

void l3_draw_rect_img_to_canvas(l3_draw_img_t *image, l3_canvas_t *dc,
                                l3_rect_t *rect)
{
    extern void hw_acc_blit(draw_img_t *image, struct gui_dispdev * dc, struct gui_rect * rect);
    gui_dispdev_t *gui_dc = gui_get_dc();
    draw_img_t *draw_image = (draw_img_t *)image;
    hw_acc_blit(draw_image, gui_dc, (gui_rect_t *)rect);
}


#endif
