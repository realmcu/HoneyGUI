/**
 * @file gui_server.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-12-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <guidef.h>
#include <draw_img.h>
#include <draw_font.h>
#include <gui_fb.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <kb_algo.h>
#include <wheel_algo.h>
#include <string.h>
#include <gui_app.h>
#include "gui_win.h"

#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
#include "js_extern_io.h"
#endif


static void (*gui_task_ext_execution_hook)(void) = NULL;


void gui_task_ext_execution_sethook(void (*hook)(void))
{
    gui_task_ext_execution_hook = hook;
}





/**
 * @brief
 *
 * @param parameter
 */
static void gui_server_entry(void *parameter)
{
#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    extern void js_init(void);
    js_init();
#endif
    gui_server_msg_init();
    while (1)
    {
        gui_app_t *app = gui_current_app();
        gui_app_t *app_next = gui_next_app();
        while (app == NULL)
        {
            gui_thread_mdelay(1000);
            app = gui_current_app();
            gui_log("!!! GUI APP NOT READY! \n");
        }

        gui_obj_t *screen = &app->screen;
        GUI_ASSERT(screen != NULL);

        /*exe some app action, like kick watchdog*/
        if (gui_task_ext_execution_hook != NULL)
        {
            gui_task_ext_execution_hook();
        }

        touch_info_t *tp = tp_algo_process(touchpad_get_data());
        GUI_UNUSED(tp);

        kb_info_t *kb = kb_algo_process(kb_get_data());
        GUI_UNUSED(kb);

        touch_info_t *wheel = wheel_algo_process(wheel_get_data());
        GUI_UNUSED(wheel);

        if ((tp->pressing == true) || (kb->pressing == true) || wheel->type == WHEEL_ING)
        {
            gui_msg_t msg = {.event = GUI_EVENT_RESET_ACTIVE_TIME};
            gui_send_msg_to_server(&msg);
        }

        if ((app->lvgl == true) || (app->arm2d == true))
        {
            continue;
        }
        bool layer = gui_app_get_layer();
        if (layer)
        {
            if (app_next && (&(app_next->screen)))
            {
                if (app->close)
                {
                    extern gui_app_t *next_app;
                    extern gui_app_t *current_app;
                    current_app->close = false;
                    gui_list_remove(&(next_app->screen.brother_list));
                    next_app->screen.parent = 0;
                    gui_obj_tree_free(&((gui_app_t *)app)->screen);
                    current_app = next_app;
                    next_app = 0;
                    app = current_app;
                }
                else
                {
                    gui_list_insert_before(&((screen)->child_list), &(app_next->screen.brother_list));
                    app_next->screen.parent = screen;
                }
            }
            screen = &app->screen;
        }
        else
        {
            if (app_next && (&(app_next->screen)))
            {
                if (app->close)
                {
                    extern gui_app_t *next_app;
                    extern gui_app_t *current_app;
                    if (current_app == NULL)
                    {
                        gui_log("Error: current_app is NULL\n");
                        return;
                    }
                    current_app->close = false;
                    gui_list_remove(&(app->screen.brother_list));
                    app->screen.parent = 0;
                    gui_obj_tree_free(&((gui_app_t *)app)->screen);
                    current_app = next_app;
                    next_app = 0;
                    app = current_app;
                }
                else
                {

                    gui_list_insert_before(&((app_next->screen).child_list), &(screen->brother_list));
                    screen->parent = &(app_next->screen);
                    screen = &(app_next->screen);
                }
            }

        }

        gui_fb_disp(screen);
        app_next = gui_next_app();
        if (layer)
        {
            if (app_next && (&(app_next->screen)))
            {
                gui_list_remove(&(app_next->screen.brother_list));
            }
        }
        else
        {
            if (app_next && (&(app_next->screen)))
            {
                gui_list_remove(&(app->screen.brother_list));
            }
        }


#ifdef _WIN32
        gui_thread_mdelay(17);
#endif
        gui_recv_msg_to_server();
    }
}

/**
 * @brief
 *
 * @return int
 */
int gui_server_init(void)
{
    extern void gui_port_dc_init(void);
    extern void gui_port_indev_init(void);
    extern void gui_port_os_init(void);
    extern void gui_port_fs_init(void);
    extern void gui_port_ftl_init(void);
    extern void gui_port_acc_init(void);
    gui_port_os_init();
    gui_port_indev_init();
    gui_port_dc_init();
    gui_port_fs_init();
    gui_port_ftl_init();
    gui_port_acc_init();

    gui_fb_change();

    gui_thread_create(GUI_SERVER_THREAD_NAME,
                      gui_server_entry, NULL,
                      1024 * 10,
                      15);
    return 0;
}



