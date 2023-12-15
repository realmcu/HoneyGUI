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
#include <gui_kb.h>
#include <string.h>
#include <gui_app.h>
#include "acc_engine.h"

static void *gui_server_mq = NULL;
static void (*gui_task_ext_execution_hook)(void) = NULL;


static void rtgui_server_msg_handler(rtgui_msg_t *msg)
{

}
void gui_task_ext_execution_sethook(void (*hook)(void))
{
    gui_task_ext_execution_hook = hook;
}

bool send_msg_to_gui_server(rtgui_msg_t *msg)
{
    if (gui_server_mq != NULL)
    {
        if (msg == NULL)
        {
            gui_log("msg == NULL\n");
        }

        gui_mq_send(gui_server_mq, msg, sizeof(rtgui_msg_t), 0);
        return true;
    }
    else
    {
        return false;
    }
}

static uint32_t daemon_start_ms = 0;
static uint32_t daemon_cnt = 0;

/**
 * @brief
 *
 * @param parameter
 */
static void rtgui_server_entry(void *parameter)
{
#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    extern void js_init(void);
    js_init();
#endif
    gui_mq_create(&gui_server_mq, "gui_svr_mq", sizeof(rtgui_msg_t), 16);
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

        tp_algo_process(touchpad_get_data());

        kb_algo_process(kb_get_data());

        if (app->lvgl == true)
        {
            continue;
        }
        if (app_next && (&(app_next->screen)))
        {
            if (app->close)
            {
                extern gui_app_t *next_app;
                extern gui_app_t *current_app;
                current_app->close = false;
                gui_list_remove(&(app->screen.brother_list));
                current_app = next_app;
                next_app = 0;
            }
            else
            {
                gui_list_insert_before(&((screen)->child_list), &(app_next->screen.brother_list));
                app_next->screen.parent = screen;
            }
        }
        screen = &app->screen;
        gui_fb_disp(screen);
        app_next = gui_next_app();
        if (app_next && (&(app_next->screen)))
        {
            gui_list_remove(&(app_next->screen.brother_list));
        }
#ifdef _WIN32
        gui_thread_mdelay(17);
#endif
        daemon_cnt++;
        if (daemon_cnt == 1)
        {
            daemon_start_ms = gui_ms_get();
        }

        touch_info_t *tp = tp_get_info();

        if (tp->pressed == true)
        {
            daemon_start_ms = gui_ms_get();
        }
#if defined __WIN32
        continue;
#endif

        if ((gui_ms_get() - daemon_start_ms) > app->active_ms)
        {
            gui_log("line %d, aemon_start_ms time = %dms, current = %dms, app->active_ms = %dms \n",
                    __LINE__, daemon_start_ms, gui_ms_get(), app->active_ms);
            rtgui_msg_t msg;
            if (true == gui_mq_recv(gui_server_mq, &msg, sizeof(rtgui_msg_t), 0xFFFFFFFF))
            {
                rtgui_server_msg_handler(&msg);
            }
            daemon_cnt = 0;
            daemon_start_ms = 0;
        }
    }
}


/**
 * @brief
 *
 * @return int
 */
int rtgui_server_init(void)
{
    extern void gui_port_dc_init(void);
    extern void gui_port_indev_init(void);
    extern void gui_port_os_init(void);
    extern void gui_port_fs_init(void);
    gui_port_os_init();
    gui_port_indev_init();
    gui_port_dc_init();
    gui_port_fs_init();

    gui_acc_init();

    rtgui_system_image_init();
    rtgui_system_font_init();
    gui_fb_change();

    gui_thread_create(GUI_SERVER_THREAD_NAME,
                      rtgui_server_entry, NULL,
                      1024 * 10,
                      15);
    return 0;
}



