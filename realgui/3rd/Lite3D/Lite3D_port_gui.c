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



#include "gui_api.h"
#include "def_file.h"
#include "draw_img.h"
#include "acc_api.h"

void *l3_port_malloc(size_t size)
{
    return gui_malloc(size);
}
void l3_port_free(void *ptr)
{
    gui_free(ptr);
}

void l3_port_draw_rect_img_to_canvas(l3_draw_rect_img_t *image, l3_canvas_t *dc,
                                     l3_rect_t *rect)
{
    gui_dispdev_t *gui_dc = gui_get_dc();
    draw_img_t *draw_image = (draw_img_t *)image;
    draw_img_cache(draw_image, IMG_SRC_MEMADDR);
    gui_acc_blit_to_dc(draw_image, gui_dc, (gui_rect_t *)rect);
    draw_img_free(draw_image, IMG_SRC_MEMADDR);
}

/* Please call this function in a source C file when this file in gui.lib to make sure compiler link this file. */
void l3_port_init(void)
{
    gui_log("Lite3D port initialized\n");
}


