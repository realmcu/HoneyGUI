/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      gui_task.c
   * @brief     Routines to create App task and handle events & messages
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
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_common_menu.h"

__attribute__((section(".app.flash.gui_menu.0")))  __attribute__((used)) const UI_MenuTypeDef
GUIStrartMenu =
{
    .name = (const char *)"GUIStrartMenu",
};
__attribute__((section(".app.flash.gui_menu.2")))  __attribute__((used)) const UI_MenuTypeDef
GUIEndMenu =
{
    .name = (const char *)"GUIStrartMenu",
};

const size_t UI_MenuTypeDef_size = sizeof(UI_MenuTypeDef);


/** @addtogroup  GUI_TASK
    * @{
    */

/** @defgroup  GUI_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/

#define UI_TASK_PRIORITY          1        //!< Task priorities
#define UI_TASK_STACK_SIZE        256 * 4  //!<  Task stack size


/*============================================================================*
 *                              Variables
 *============================================================================*/

void *gui_task_handle = NULL;
void *gui_queue_handle = NULL;

void    *gui_task_dynamic_timer     = NULL;
void    *gui_task_display_timer     = NULL;
void    *gui_task_page_switch_timer = NULL;
void    *gui_task_roll_switch_timer = NULL;


/*============================================================================*
 *                              Functions
 *============================================================================*/
void gui_task(void *p_param);

/**
 * @brief  Initialize App task
 * @return void
 */
int gui_task_init(void)
{
    gui_core_init();
    gui_server_init();
    gui_task_handle = gui_thread_create("legacy-gui",
                                        gui_task, NULL,
                                        1024 * 10,
                                        15);
}

GUI_INIT_APP_EXPORT(gui_task_init);

bool post_event_to_gui_task(T_RTK_GUI_EVENT *p_msg, uint32_t line)
{
    if ((gui_queue_handle == NULL) || (gui_task_handle == NULL))
    {
        gui_log("GUI TASK NOT Ready! line = %d \n", line);
        return false;
    }
    if (gui_mq_send(gui_queue_handle, p_msg, sizeof(T_RTK_GUI_EVENT), 0) == false)
    {
        gui_log("send_io_msg_to_gui task fail io queue line = %d \n", line);
        return false;
    }
    return true;
}

static void rtl_gui_timer_display_callback(void)
{
    T_RTK_GUI_EVENT p_msg;
    p_msg.type = GUI_EVENT_TIMER;
    p_msg.subtype = GUI_TIMER_DISPLAY_CB;
    post_event_to_gui_task(&p_msg, __LINE__);
}

static void rtl_gui_timer_dynamic_callback(void)
{
    T_RTK_GUI_EVENT p_msg;
    p_msg.type = GUI_EVENT_TIMER;
    p_msg.subtype = GUI_TIMER_DYNAMIC_CB;
    post_event_to_gui_task(&p_msg, __LINE__);
}
static void rtl_gui_timer_slide_callback(void)
{
    T_RTK_GUI_EVENT p_msg;
    p_msg.type = GUI_EVENT_TIMER;
    p_msg.subtype = GUI_TIMER_SLIDE_CB;
    post_event_to_gui_task(&p_msg, __LINE__);
}
static void rtl_gui_timer_roll_callback(void)
{
    T_RTK_GUI_EVENT p_msg;
    p_msg.type = GUI_EVENT_TIMER;
    p_msg.subtype = GUI_TIMER_ROLL_CB;
    post_event_to_gui_task(&p_msg, __LINE__);
}

UI_MenuTypeDef *gui_get_menu_addr_by_name(const char *name)
{

    uintptr_t start_addr = (uintptr_t)&GUIStrartMenu;
    uintptr_t stop_addr = (uintptr_t)&GUIEndMenu;
    for (uintptr_t i = start_addr; i < stop_addr; i = i + sizeof(UI_MenuTypeDef))
    {
        UI_MenuTypeDef *menu = (UI_MenuTypeDef *)i;
        if (strcmp(name, menu->name) == 0)
        {
            gui_log("[pRet] %s \n", menu->name);
            return menu;
        }
    }
    gui_log("!!!!!!gui_get_menu_addr_by_name find NULL!!!!!! \n");
    return NULL;
}

void gui_test_menu(void)
{
    T_RTK_GUI_EVENT p_msg;
    p_msg.type = GUI_EVENT_NEW_VIEW;
    p_msg.u.param = (uint32_t)gui_get_menu_addr_by_name("GUIDemoMenu");
    post_event_to_gui_task(&p_msg, __LINE__);

}


/**
 * @brief        sensor hub task
 * @param[in]    p_params    Parameters sending to the task
 * @return       void
 */

void gui_task(void *pvParameters)
{
    T_RTK_GUI_EVENT event;


    gui_mq_create(&gui_queue_handle, "gui_svr_mq", sizeof(T_RTK_GUI_EVENT), 16);

//    os_timer_create(&gui_task_display_timer, "display picture timer", 1,
//                    GUI_TASK_DISPLAY_TIMER_INTERVAL, false,
//                    rtl_gui_timer_display_callback);
//    os_timer_create(&gui_task_dynamic_timer, "dynamic picture timer", 2,
//                    GUI_TASK_DYNAMIC_TIMER_INTERVAL, true,
//                    rtl_gui_timer_dynamic_callback);
//    os_timer_create(&gui_task_page_switch_timer, "page switch timer", 3, PAGE_SWITCH_TIMER_INTERVAL,
//                    false,
//                    rtl_gui_timer_slide_callback);
//    os_timer_create(&gui_task_roll_switch_timer, "page switch timer", 4, PAGE_SWITCH_TIMER_INTERVAL,
//                    false,
//                    rtl_gui_timer_roll_callback);

    gui_log("[RTK GUI TASK] task test  \n");

    gui_test_menu();

    while (true)
    {
        if (gui_mq_recv(gui_queue_handle, &event, sizeof(T_RTK_GUI_EVENT), 0xFFFFFFFF) == true)
        {
            gui_task_handle_event(&event);
        }
    }
}


void gui_task_handle_event(T_RTK_GUI_EVENT *event)
{
    switch (event->type)
    {
    case GUI_EVENT_NEW_VIEW:
        {
            gui_event_new_view_handler(event);
            break;
        }
    case GUI_EVENT_FRAME_SYNC:
        {
            gui_event_frame_sync_handler(event);
            break;
        }
    case GUI_EVENT_TIMER:
        {
            gui_event_timer_handler(event);
            break;
        }
    case GUI_EVENT_TOUCH:
        {
            gui_event_touch_handler(event);
            break;
        }
    case GUI_EVENT_BUTTON:
        {
            gui_event_button_handler(event);
            break;
        }
    default:
        break;
    }
}

/** @} */ /* End of group IO_TASK */
/** @} */ /* End of group PERIPH_DEMO */

