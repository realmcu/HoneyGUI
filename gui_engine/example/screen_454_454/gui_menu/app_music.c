/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file app_music.c
  * @brief music player
  * @author triton_yu@realsil.com.cn
  * @date 2024/09/24
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_switch.h"
#include "../root_image/ui_resource.h"
#include "gui_canvas_rect.h"
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

#define SCREEN_W ((int)gui_get_screen_width())
#define SCREEN_H ((int)gui_get_screen_height())
#define SWITCH_W (160)
#define SWITCH_H (120)
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


static void switch_on_cb(void *null1, void *null2, void *param)
{
    gui_music_play("gui_engine\\example\\screen_454_454\\root_image\\root\\music_player\\sample-3s.mp3");
}
static void switch_off_cb(void)
{
    gui_music_stop();
}
static void on_completion(void *p, void *this_widget, gui_animate_t *animate)
{
    if (gui_music_completion_status())
    {
        GUI_API(gui_switch_t).turn_off_no_event(this_widget);
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void app_music_ui_design(gui_obj_t *parent)
{
    gui_canvas_rect_create(parent, 0, 0, 0, SCREEN_W, SCREEN_H, APP_COLOR_SILVER);
    gui_switch_t *sw = gui_switch_create(parent, SCREEN_W / 2 - SWITCH_W / 2,
                                         SCREEN_H / 2 - SWITCH_H / 2, SWITCH_W, SWITCH_H, STARTBUTTON_BIN, PAUSEBUTTON_BIN);
    GUI_API(gui_switch_t).on_turn_on(sw, switch_on_cb, 0);
    GUI_API(gui_switch_t).on_turn_off(sw, switch_off_cb, 0);
    gui_img_set_mode(sw->switch_picture, IMG_SRC_OVER_MODE);
    GUI_API(gui_switch_t).animate(sw, 1000, -1, on_completion, 0);
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
