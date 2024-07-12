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
#include "gui_obj.h"
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
    uint32_t start_ms;             //!< screen shut dowm delay
    void *thread_id;                //!< thread handle(optional)
    void (* thread_entry)(void *_this); //!< thread entry
    void (* ctor)(void *_this);      //!< constructor
    void (* dtor)(void *_this);      //!< destructor
    void (* ui_design)(gui_app_t *); //!< ui create entry
    bool lvgl;
    bool arm2d;
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
#define GUI_APP_DEFINE(APP_NAME,UI_DESIGN)\
    \
    static void UI_DESIGN(gui_app_t*);\
    static gui_app_t _app_##APP_NAME =\
                                      {\
                                       .screen =\
                                                {\
                                                 .name = #APP_NAME,\
                                                 .type = SCREEN,\
                                                },\
                                       .ui_design = UI_DESIGN,\
                                       .active_ms = 1000000,\
                                      };\
    \
    gui_app_t *_get_app_##APP_NAME##_handle(void)\
    {\
        return &_app_##APP_NAME;\
    }

#define GUI_APP_HANDLE(APP_NAME) _get_app_##APP_NAME##_handle()
#define GUI_APP_SHUTDOWM(APP_NAME) extern gui_app_t *_get_app_##APP_NAME##_handle(void);gui_app_shutdown(_get_app_##APP_NAME##_handle());
#define GUI_APP_STARTUP(APP_NAME) extern gui_app_t *_get_app_##APP_NAME##_handle(void);gui_app_startup(_get_app_##APP_NAME##_handle());
#define GUI_APP_SWAP(APP_NAME, APP_NAME_NEXT) gui_switch_app(_get_app_##APP_NAME##_handle(), _get_app_##APP_NAME_NEXT##_handle());
#define GUI_APP_ROOT_SCREEN &(app->screen)
#define GUI_APP_SWAP_HANDLE(HANDLE_FUNC, HANDLE_NEXT_FUNC)     extern gui_app_t *HANDLE_FUNC;\
    extern gui_app_t *HANDLE_NEXT_FUNC;\
    gui_switch_app(HANDLE_FUNC, HANDLE_NEXT_FUNC);
#define GUI_APP_DEFINE_NAME(APP_NAME)\
    \
    static void _##APP_NAME##_ui_design(gui_app_t*);\
    static gui_app_t _app_##APP_NAME =\
                                      {\
                                       .screen =\
                                                {\
                                                 .name = #APP_NAME,\
                                                },\
                                       .ui_design = _##APP_NAME##_ui_design,\
                                       .active_ms = 1000000,\
                                      };\
    \
    gui_app_t *_get_app_##APP_NAME##_handle(void)\
    {\
        return &_app_##APP_NAME;\
    }
#define GUI_APP_ENTRY(APP_NAME) static void _##APP_NAME##_ui_design(gui_app_t* app)
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

void gui_app_install(gui_app_t *app, void *ui_design, void *gui_app_entry);

void gui_app_startup(gui_app_t *app);

void gui_app_shutdown(gui_app_t *app);

gui_app_t *gui_app_create(const char *app_name, void *ui_design, void *gui_app_entry);

/**
 * @brief switch app from A to B
 *
 * @param from A pointer
 * @param to B pointer
 */
void gui_switch_app(gui_app_t *from, gui_app_t *to);
/**
 * @brief set next app top layer
 *
 */
void gui_app_layer_top(void);
/**
 * @brief set next app button layer
 *
 */
void gui_app_layer_buttom(void);
/**
 * @brief get next app layer
 *
 * @return true top layer
 * @return false button layer
 */
bool gui_app_get_layer(void);
/**
 * @brief set app active ms time
 *
 * @param app app set
 * @param active_ms active ms times
 */
void gui_set_app_active_time(gui_app_t *app, uint32_t active_ms);
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















