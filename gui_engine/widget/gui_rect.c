/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_rect_create.c
  * @brief image rect widget
  * @details image rect widget is used to show rect on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_img.h"
#include "gui_rect.h"


/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
gui_img_t *gui_rect_create(gui_obj_t   *parent,
                           int          x,
                           int          y,
                           int          w,
                           int          h,
                           gui_color_t  color)
{
    gui_rgb_data_head_t *head;
    gui_img_t *img;
    gui_rect_file_head_t *array;

    array = gui_malloc(sizeof(gui_rect_file_head_t));
    memset(array, 0, sizeof(gui_rect_file_head_t));

    head = &(array->head);
    head->type = RGBA8888;
    head->w = w;
    head->h = h;

    array->color.color.rgba_full = color.color.rgba_full;
    img = gui_img_create_from_mem(parent, "RECT", array, x, y, w, h);
    img->blend_mode = IMG_RECT;

    return img;
}

gui_img_t *gui_rect_round_create(gui_obj_t   *parent,
                                 int          x,
                                 int          y,
                                 int          w,
                                 int          h,
                                 gui_color_t  color,
                                 uint32_t    *image_file_addr)
{
    int16_t rx, ry,  rect_w_1, rect_y_1, rect_h_1, rect_x_1,
            rect_w_2, rect_y_2, rect_h_2, rect_x_2,
            rect_w_3, rect_y_3, rect_h_3, rect_x_3,
            x2, y2, x3, y3, x4, y4;

    gui_img_t *img_left_top = gui_img_create_from_mem(parent, "leftup", image_file_addr, x, y, 0, 0);
    gui_img_set_mode(img_left_top, IMG_SRC_OVER_MODE);

    ry = gui_img_get_height(img_left_top);
    rx = gui_img_get_width(img_left_top);
    rect_w_1 = w - rx * 2;
    rect_y_1 = 0;
    rect_h_1 = ry;
    rect_x_1 = rx;
    rect_w_2 = w;
    rect_y_2 = ry;
    rect_h_2 = h - 2 * ry;
    rect_x_2 = 0;
    rect_w_3 = rect_w_1;
    rect_y_3 = ry + rect_h_2;
    rect_h_3 = rect_h_1;
    rect_x_3 = rect_x_1;
    x2 = 0;
    y2 = ry + rect_h_2;
    x3 = rx + rect_w_1;
    y3 = 0;
    x4 = x3;
    y4 = y2;

    gui_rect_create((void *)img_left_top, rect_x_1, rect_y_1, rect_w_1, rect_h_1, color);
    gui_rect_create((void *)img_left_top, rect_x_2, rect_y_2, rect_w_2, rect_h_2, color);
    gui_rect_create((void *)img_left_top, rect_x_3, rect_y_3, rect_w_3, rect_h_3, color);

    //left buttom
    gui_img_t *img_left_buttom = gui_img_create_from_mem(img_left_top, "left buttom", image_file_addr,
                                                         x2, y2, 0,
                                                         0);
    gui_img_set_mode(img_left_buttom, IMG_SRC_OVER_MODE);
    gui_img_translate(img_left_buttom, rx / 2, ry / 2);
    gui_img_rotation(img_left_buttom, -90, rx / 2, ry / 2);

    //right top
    gui_img_t *img_right_top = gui_img_create_from_mem(img_left_top, "right top", image_file_addr, x3,
                                                       y3, 0, 0);
    gui_img_set_mode(img_right_top, IMG_SRC_OVER_MODE);
    gui_img_translate(img_right_top, rx / 2, ry / 2);
    gui_img_rotation(img_right_top, 90, rx / 2, ry / 2);

    //right buttom
    gui_img_t *img_right_buttom = gui_img_create_from_mem(img_left_top, "right buttom", image_file_addr,
                                                          x4, y4, 0,
                                                          0);
    gui_img_set_mode(img_right_buttom, IMG_SRC_OVER_MODE);
    gui_img_translate(img_right_buttom, rx / 2, ry / 2);
    gui_img_rotation(img_right_buttom, -180, rx / 2, ry / 2);

    return img_left_top;
}

void gui_rect_set_size(gui_img_t *this, int w, int h)
{
    gui_rect_file_head_t *array = (gui_rect_file_head_t *)this->draw_img->data;
    array->head.w = w;
    array->head.h = h;
    this->base.w = w;
    this->base.h = h;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
