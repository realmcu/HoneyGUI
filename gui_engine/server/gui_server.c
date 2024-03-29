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
#include "acc_engine.h"
#include "gui_win.h"

#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
#include "js_extern_io.h"
#endif

#define MAX_MSG_EVENT_CNT   32
static void *gui_server_mq = NULL;
static void (*gui_task_ext_execution_hook)(void) = NULL;
bool GUI_SERVER_ALLOW_DLPS = false;
bool force_display_off = false;
static uint32_t daemon_start_ms = 0;
static uint32_t daemon_cnt = 0;

static gui_event_t server_event_code;
static gui_event_cb_t msg_event_cb[MAX_MSG_EVENT_CNT];
static gui_event_t msg_event_code[MAX_MSG_EVENT_CNT];
static void *msg_event_obj[MAX_MSG_EVENT_CNT];
static uint32_t msg_event_cnt = 0;

bool gui_server_dlps_check(void)
{
    return GUI_SERVER_ALLOW_DLPS;
}

static void gui_server_event_handler(gui_obj_t *obj)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (server_event_code != GUI_EVENT_INVALIDE)
        {
            for (uint32_t i = 0; i < obj->event_dsc_cnt; i++)
            {
                gui_event_dsc_t *event_dsc = obj->event_dsc + i;
                if (event_dsc->filter == server_event_code)
                {
                    msg_event_cb[msg_event_cnt] = event_dsc->event_cb;
                    msg_event_code[msg_event_cnt] = server_event_code;
                    msg_event_obj[msg_event_cnt] = obj;
                    msg_event_cnt++;
                    if (msg_event_cnt > MAX_MSG_EVENT_CNT)
                    {
                        GUI_ASSERT(NULL != NULL);
                    }
                }
            }
        }
        gui_server_event_handler(obj);
    }
}

static void gui_server_msg_handler(gui_obj_t *screen, gui_msg_t *msg)
{
    bool event_handle = true;

    if (msg->type == GUI_EVENT_DISPLAY_ON)
    {
        if (GUI_SERVER_ALLOW_DLPS)
        {
            GUI_SERVER_ALLOW_DLPS = false;
            gui_display_on();
            gui_fb_change();
            daemon_cnt = 0;
            daemon_start_ms = 0;
        }
        event_handle = false;
    }
    else if (msg->type == GUI_EVENT_DISPLAY_OFF)
    {
        force_display_off = true;
        event_handle = false;
    }
#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    else if (msg->type == GUI_EVENT_EXTERN_IO_JS)
    {
        gui_extern_event_js_handler(msg);
        event_handle = false;
    }
#endif
    else
    {
        server_event_code = (gui_event_t)msg->type;
    }

    if (event_handle)
    {
        gui_server_event_handler(screen);
        for (uint32_t i = 0; i < msg_event_cnt; i++)
        {
            msg_event_cb[i](msg_event_obj[i], msg_event_code[i]);
        }
        msg_event_cnt = 0;
        server_event_code = GUI_EVENT_INVALIDE;
    }
}

void gui_task_ext_execution_sethook(void (*hook)(void))
{
    gui_task_ext_execution_hook = hook;
}


bool send_msg_to_gui_server(gui_msg_t *msg)
{
    if (gui_server_mq != NULL)
    {
        if (msg == NULL)
        {
            gui_log("msg == NULL\n");
        }

        gui_mq_send(gui_server_mq, msg, sizeof(gui_msg_t), 0);
        return true;
    }
    else
    {
        return false;
    }
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
    gui_mq_create(&gui_server_mq, "gui_svr_mq", sizeof(gui_msg_t), 16);
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

        wheel_algo_process(wheel_get_data());

        if (app->lvgl == true)
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
                    gui_list_remove(&(app->screen.brother_list));
                    app->screen.parent = 0;
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
        daemon_cnt++;
        if (daemon_cnt == 1)
        {
            daemon_start_ms = gui_ms_get();
        }

        touch_info_t *tp = tp_get_info();

        if (tp->pressed == true || tp->pressing == true)
        {
            daemon_start_ms = gui_ms_get();
        }

        gui_msg_t msg;
#if defined __WIN32
        while (true == gui_mq_recv(gui_server_mq, &msg, sizeof(gui_msg_t), 0))
        {
            gui_server_msg_handler(screen, &msg);
        }
        continue;
#endif
        if ((((gui_ms_get() - daemon_start_ms) > app->active_ms) && (app->active_ms != (uint32_t) - 1)) ||
            force_display_off)
        {
            gui_log("line %d, aemon_start_ms time = %dms, current = %dms, app->active_ms = %dms \n",
                    __LINE__, daemon_start_ms, gui_ms_get(), app->active_ms);

            gui_display_off();
            GUI_SERVER_ALLOW_DLPS = true;
            force_display_off = false;

            kb_algo_process(kb_get_data());
            kb_info_t *kb = kb_get_info();
            if (kb->pressed)
            {
                msg.type = GUI_EVENT_DISPLAY_ON;
                send_msg_to_gui_server(&msg);
            }

            while (GUI_SERVER_ALLOW_DLPS)
            {
                if (true == gui_mq_recv(gui_server_mq, &msg, sizeof(gui_msg_t), 0xFFFFFFFF))
                {
                    gui_server_msg_handler(screen, &msg);//screen on event set GUI_SERVER_ALLOW_DLPS false
                }
            }
        }
        while (true == gui_mq_recv(gui_server_mq, &msg, sizeof(gui_msg_t), 0))
        {
            gui_server_msg_handler(screen, &msg);
        }
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
    gui_port_os_init();
    gui_port_indev_init();
    gui_port_dc_init();
    gui_port_fs_init();

    gui_acc_init();

    gui_fb_change();

    gui_thread_create(GUI_SERVER_THREAD_NAME,
                      gui_server_entry, NULL,
                      1024 * 10,
                      15);
    return 0;
}



