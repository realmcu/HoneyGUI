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
#ifndef __GUI_APP_H__
#define __GUI_APP_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>

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

/** @brief  APP structure */
typedef struct gui_app gui_app_t;
struct gui_app
{
    gui_obj_t screen;               //!< widget tree root
    const char *xml;                //!< widget tree design file
    uint32_t active_ms;             //!< screen shut dowm delay
    void *thread_id;                //!< thread handle(optional)
    void (* thread_entry)(void *this); //!< thread entry
    void (* ctor)(void *this);      //!< constructor
    void (* dtor)(void *this);      //!< destructor
    void (* ui_design)(gui_app_t *); //!< ui create entry
    bool lvgl;
    bool close;
    bool next;
    bool close_sync;

};
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
gui_app_t *gui_current_app(void);
gui_app_t *gui_next_app(void);
void gui_app_exec(gui_app_t *app);

void gui_app_install(gui_app_t *app, void *ui_design, void *gui_app_entry);

void gui_app_startup(gui_app_t *app);

void gui_app_shutdown(gui_app_t *app);

void gui_app_uninstall(gui_app_t *app);

gui_app_t *gui_app_create(const char *app_name, void *ui_design, void *gui_app_entry);

void gui_app_delete(gui_app_t *app);
/**
 * @brief switch app from A to B
 *
 * @param from A pointer
 * @param to B pointer
 */
void gui_switch_app(gui_app_t *from, gui_app_t *to);

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















