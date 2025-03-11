/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_indev.h
  * @brief Application Programming Interface for UI
  * @details Input Device
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_INDEV_H__
#define __GUI_API_INDEV_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <guidef.h>

/*============================================================================*
 *                         Macros
 *============================================================================*/



/*============================================================================*
 *                         Functions
 *============================================================================*/

void gui_indev_info_register(struct gui_indev *info);

struct gui_indev *gui_get_indev(void);

void ext_button_set_indicate(void (*callback)(void));

gui_touch_port_data_t *touchpad_get_data(void);

gui_kb_port_data_t *kb_get_data(void);

gui_wheel_port_data_t *wheel_get_data(void);

#ifdef __cplusplus
}
#endif
#endif
