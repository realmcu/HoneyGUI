/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      gui_handler.c
   * @brief
   * @author    howie
   * @date      2019-12-26
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <gui_task.h>
#include "string.h"
#include "gui_handler.h"
#include "gui_common_menu.h"


UI_MenuTypeDef       CacheCurrentView;
UI_MenuTypeDef       *pTaskCurrentView = NULL;
static uint32_t      MenuTimeMs = 0;
extern void          *gui_task_dynamic_timer;
extern void          *gui_task_display_timer;
extern void          *gui_task_page_switch_timer;
extern void          *gui_task_roll_switch_timer;
extern UI_MenuTypeDef g_cur_menu;
extern UI_MenuTypeDef g_sub_menu;
extern UI_MenuTypeDef g_par_menu;





void gui_event_new_view_handler(T_RTK_GUI_EVENT *msg)
{
    gui_log("[RTK GUI HANDLER] new view \n");

    if (msg->u.param == 0)
    {
        gui_log("NEW MENU POINT NULL!!!! \n");
        gui_log("[!!!GUI ERROR!!!] \n");
        while (1);
    }

    memcpy(&CacheCurrentView, (UI_MenuTypeDef *)msg->u.param, sizeof(UI_MenuTypeDef));
    pTaskCurrentView = &CacheCurrentView;


    if ((pTaskCurrentView != NULL) && (pTaskCurrentView->constructor_cb != NULL))
    {
        pTaskCurrentView->pMenuTime = &MenuTimeMs;
        *(pTaskCurrentView->pMenuTime) = 0;
        pTaskCurrentView->psram_has_up_cache = false;
        pTaskCurrentView->psram_has_down_cache = false;
        pTaskCurrentView->psram_has_left_cache = false;
        pTaskCurrentView->psram_has_right_cache = false;
        pTaskCurrentView->psram_has_center_cache = false;
        pTaskCurrentView->constructor_cb(pTaskCurrentView, NULL, NULL);
    }

}

void gui_event_timer_handler(T_RTK_GUI_EVENT *msg)
{
    gui_log("[RTK GUI HANDLER] timer sub = %d \n", msg->subtype);
    switch (msg->subtype)
    {
    case GUI_TIMER_DISPLAY_CB:
        {
            if ((pTaskCurrentView != NULL) && (pTaskCurrentView->display_cb != NULL))
            {
                pTaskCurrentView->display_cb(pTaskCurrentView, &g_cur_menu, NULL);
            }

            pTaskCurrentView = NULL;// we also can clear queue here , todo by howie
            break;
        }
    case GUI_TIMER_DYNAMIC_CB:
        {
            if ((pTaskCurrentView != NULL) && (pTaskCurrentView->dynamic_cb != NULL))
            {
                *(pTaskCurrentView->pMenuTime) = *(pTaskCurrentView->pMenuTime) + GUI_TASK_DYNAMIC_TIMER_INTERVAL;
                pTaskCurrentView->dynamic_cb(pTaskCurrentView, &g_cur_menu, NULL);
            }
            break;
        }
    case GUI_TIMER_SLIDE_CB:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    case GUI_TIMER_ROLL_CB:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    case GUI_TIMER_DISPLAY_START:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    case GUI_TIMER_DYNAMIC_START:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    case GUI_TIMER_SLIDE_START:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    case GUI_TIMER_ROLL_START:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    case GUI_TIMER_DISPLAY_STOP:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    case GUI_TIMER_DYNAMIC_STOP:
        {
            GUI_ASSERT(NULL != NULL);
            break;
        }
    default:
        break;
    }
}


void gui_event_frame_sync_handler(T_RTK_GUI_EVENT *msg)
{
    gui_log("[RTK GUI HANDLER] frame sync \n");
    rtl_gui_lcd_update_internal();
}


void gui_event_touch_handler(T_RTK_GUI_EVENT *msg)
{
    GUI_ASSERT(NULL != NULL);
}
void gui_event_button_handler(T_RTK_GUI_EVENT *msg)
{
    GUI_ASSERT(NULL != NULL);
}


/** @} */ /* End of group IO_TASK */
/** @} */ /* End of group PERIPH_DEMO */

