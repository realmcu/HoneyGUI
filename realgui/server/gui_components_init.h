/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_components_init.h
  * @brief app api
  * @details application startup and close
  * @author howie_wang@realsil.com.cn
  * @date
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_COMPONENTS_H__
#define __GUI_COMPONENTS_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef int (*gui_init_fn_t)(void);


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_APP_USED                     __attribute__((used))
#define GUI_APP_SECTION(x)                  __attribute__((section(x)))


#define GUI_APP_INIT_EXPORT(fn, level)                                                       \
    GUI_APP_USED const gui_init_fn_t __ui_init_##fn GUI_APP_SECTION(".uii_fn." level) = fn

/* board init routines will be called in board_init() function */
#define GUI_INIT_BOARD_EXPORT(fn)           GUI_APP_INIT_EXPORT(fn, "1")

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initilization) */
#define GUI_INIT_PREV_EXPORT(fn)            GUI_APP_INIT_EXPORT(fn, "2")
/* device initialization */
#define GUI_INIT_DEVICE_EXPORT(fn)          GUI_APP_INIT_EXPORT(fn, "3")
/* components initialization (dfs, lwip, ...) */
#define GUI_INIT_VIEW_DESCRIPTOR_REGISTER(fn)         GUI_APP_INIT_EXPORT(fn, "4")
/* environment initialization (mount disk, ...) */
#define GUI_INIT_VIEW_DESCRIPTOR_GET(fn)            GUI_APP_INIT_EXPORT(fn, "5")

#define GUI_INIT_VIEW_CREATE(fn)            GUI_APP_INIT_EXPORT(fn, "6")
/* appliation initialization (rtgui application etc ...) */
#define GUI_INIT_APP_EXPORT(fn)             GUI_APP_INIT_EXPORT(fn, "7")


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/


#ifdef __cplusplus
}
#endif
#endif















