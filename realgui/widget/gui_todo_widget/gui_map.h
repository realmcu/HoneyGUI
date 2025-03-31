/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_map.h
  * @brief map.
  * @details map
  * @author triton_yu@realsil.com.cn
  * @date 2024/04/22
  * @version 0.1
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2024 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
\endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_MAP_H__
#define __GUI_MAP_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>
#include "gui_win.h"
#include "gui_img_stb.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct map_tile
{
    int x;
    int y;
    gui_stb_img_t *img;
} map_tile_t;

/** @brief  map structure */
typedef struct gui_map
{
    gui_win_t base; //!< base structure
    map_tile_t tile[3][3];
    void *button_data[3];
    float longitude;
    float latitude;
    int16_t start_x, start_y;
    int16_t zoom;
    bool press, release;
} gui_map_t;


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Create a MAP,
 *
 * @param parent the father widget nested in
 * @return gui_map_t*
 */
gui_map_t *gui_map_create(void *parent);


#ifdef __cplusplus
}
#endif

#endif
