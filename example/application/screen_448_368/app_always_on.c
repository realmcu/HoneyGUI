/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file app_always_on.c
  * @brief always on effect in app
  * @details will dispaly without any operation for a while
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/18
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gui_tabview.h>
#include <gui_obj.h>
#include <gui_win.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_perspective.h"
#include "gui_perspective.h"


/** @defgroup EXAMPLE EXAMPLE
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup EXAMPLE_Exported_Types EXAMPLE Exported Types
  * @{
  */


/** End of EXAMPLE_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup EXAMPLE_Exported_Constants EXAMPLE Exported Constants
  * @{
  */


/** End of EXAMPLE_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup EXAMPLE_Exported_Macros EXAMPLE Exported Macros
  * @{
  */


/** End of EXAMPLE_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup EXAMPLE_Exported_Variables EXAMPLE Exported Variables
  * @{
  */
gui_win_t *win_always_on;
gui_perspective_t *per;
extern gui_tabview_t *tv;
static bool always_on_flag;

/** End of EXAMPLE_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup EXAMPLE_Exported_Functions EXAMPLE Exported Functions
  * @{
  */

/**
 * @brief Animation callback. If animation ends, always-on shows.
 * @param win_always_on the window widget which has the animation effect
 */
static void always_on_animate_cb(gui_win_t *win_always_on)
{
    if (win_always_on->animate->progress_percent == 1)
    {
        GET_BASE(per)->not_show = false;
        GET_BASE(tv)->not_show = true;
        always_on_flag = true;
    }
}
/**
 * @brief window press gesture callback. IF pressed, set the animate invalid. The always-on will not show until released.
 *
 */
static void callback_always_on()
{
    gui_app_t *app = (gui_app_t *)get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    if (always_on_flag)
    {
        GET_BASE(per)->not_show = true;
        GET_BASE(tv)->not_show = false;
        always_on_flag = false;
        memset(win_always_on->animate, 0, sizeof(gui_animate_t));
        gui_win_set_animate(win_always_on, 8000, 0, always_on_animate_cb, win_always_on);
    }
    else
    {
        memset(win_always_on->animate, 0, sizeof(gui_animate_t));
    }

}
/**
 * @brief window release gesture callback. IF released, set the animate starting. It is a timer.
 *
 */
static void callback_always_on_release()
{
    gui_app_t *app = (gui_app_t *)get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    if (!always_on_flag)
    {
        memset(win_always_on->animate, 0, sizeof(gui_animate_t));
        gui_win_set_animate(win_always_on, 8000, 0, always_on_animate_cb, win_always_on);
    }

}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief create the always-on effect nested in a parent widget
 *
 * @param parent parent widget
 */
void always_on_ui_design(gui_obj_t *parent)
{
    win_always_on = gui_win_create(parent, "always_on", 0, 0, 368, 448);
    gui_perspective_imgfile_t imgfile =
    {
        .src_mode[0] = IMG_SRC_MEMADDR, .src_mode[1] = IMG_SRC_MEMADDR, .src_mode[2] = IMG_SRC_MEMADDR,
        .src_mode[3] = IMG_SRC_MEMADDR, .src_mode[4] = IMG_SRC_MEMADDR, .src_mode[5] = IMG_SRC_MEMADDR,
        .data_addr[0] = ACTIVITY_BIN,
        .data_addr[1] = WEATHER_BIN,
        .data_addr[2] = HEARTRATE_BIN,
        .data_addr[3] = CLOCKN_BIN,
        .data_addr[4] = MUSIC_BIN,
        .data_addr[5] = QUICKCARD_BIN
    };
    per = gui_perspective_create(win_always_on, "test", &imgfile, 0, 0);

    GET_BASE(per)->not_show = true;
    gui_obj_add_event_cb(win_always_on, (gui_event_cb_t)callback_always_on, GUI_EVENT_TOUCH_PRESSED,
                         NULL);
    gui_obj_add_event_cb(win_always_on, (gui_event_cb_t)callback_always_on_release,
                         GUI_EVENT_TOUCH_RELEASED,
                         NULL);
    gui_win_set_animate(win_always_on, 8000, 0, always_on_animate_cb, win_always_on);

}

/** End of EXAMPLE_Exported_Functions
  * @}
  */

/** End of EXAMPLE
  * @}
  */




