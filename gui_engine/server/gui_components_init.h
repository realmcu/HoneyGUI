/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_app.h
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


/** @defgroup APP APP
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup APP_Exported_Types APP Exported Types
  * @brief
  * @{
  */

typedef int (*gui_init_fn_t)(void);

/** End of APP_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup APP_Exported_Constants APP Exported Constants
  * @brief
  * @{
  */


/** End of APP_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup APP_Exported_Macros APP Exported Macros
  * @brief
  * @{
  */

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
#define GUI_INIT_COMPONENT_EXPORT(fn)       GUI_APP_INIT_EXPORT(fn, "4")
/* environment initialization (mount disk, ...) */
#define GUI_INIT_ENV_EXPORT(fn)             GUI_APP_INIT_EXPORT(fn, "5")
/* appliation initialization (rtgui application etc ...) */
#define GUI_INIT_APP_EXPORT(fn)             GUI_APP_INIT_EXPORT(fn, "6")


/** End of APP_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup APP_Exported_Variables APP Exported Variables
  * @brief
  * @{
  */


/** End of APP_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup APP_Exported_Functions APP Exported Functions
  * @brief
  * @{
  */

/**
 * @brief
 *
 */
void gui_components_init(void);

/** End of APP_Exported_Functions
  * @}
  */

/** End of APP
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif















