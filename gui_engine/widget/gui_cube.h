/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_cube.h
  * @brief cube widget
  * @details cube widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_CUBE_H__
#define __GUI_CUBE_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include "gui_img.h"
#include "gui_perspective.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  CUBE widget structure */
typedef struct gui_cube
{
    gui_obj_t base;                 //!< base structure
    draw_img_t draw_img_front;
    draw_img_t draw_img_back;
    draw_img_t draw_img_up;
    draw_img_t draw_img_down;
    draw_img_t draw_img_left;
    draw_img_t draw_img_right;
    float c_x;
    float c_y;
    float cbsize;
    //float scale_x;
    //float scale_y;
    //float t_x;
    //float t_y;
} gui_cube_t;


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define RAD(d)        (d*3.1415926f/180.0f) //!< angle to rad


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief cube create
 *
 * @param parent parent widget
 * @param name  widget name
 * @param addr
 * @param x  left
 * @param y top
 * @param w  width
 * @param h hight
 * @return gui_cube_t* widget pointer
 * <b>Example usage</b>
 * \code{.c}
 *{
 *    uint8_t *array[6] = {ACTIVITY_BIN, WEATHER_BIN, HEARTRATE_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};
 *
 *
 *    gui_cube_t *cube4 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
 *    gui_cube_set_size(cube4, 100);
 *    gui_cube_set_center(cube4, 200, 200);
 *}
 * \endcode
 */
gui_cube_t *gui_cube_create(void *parent,  const char *name, void *addr,
                            int16_t x, int16_t y, int16_t w, int16_t h);
/**
 * @brief set center
 *
 * @param this widget pointer
 * @param c_x center x
 * @param c_y center y
 */
void gui_cube_set_center(gui_cube_t *this, float c_x, float c_y);
/**
 * @brief set size
 *
 * @param this widget pointer
 * @param size scale size
 */
void gui_cube_set_size(gui_cube_t *this, float size);



#ifdef __cplusplus
}
#endif

#endif

