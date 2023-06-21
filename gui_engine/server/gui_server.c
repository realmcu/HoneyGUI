/*
 * File      : gui_server.c
 */

#include <guidef.h>
#include <draw_img.h>
#include <draw_font.h>
#include <gui_fb.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <string.h>
#include <gui_app.h>
#include "acc_engine.h"

static void (*gui_debug_hook)(void) = NULL;
static void *gui_server_handle = NULL;
static void *gui_server_mq = NULL;

void gui_debug_sethook(void (*hook)(void))
{
    gui_debug_hook = hook;
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
/**
 * rtgui server thread's entry
 */

//static int32_t daemon_left_ms = 5000;

static uint32_t daemon_start_ms = 0;
static uint32_t daemon_cnt = 0;
static void rtgui_server_entry(void *parameter)
{
    gui_server_mq = gui_mq_create("gui_svr_mq", sizeof(rtgui_msg_t), 16);
    while (1)
    {
        gui_app_t *app = gui_current_app();
        while (app == NULL)
        {
            gui_thread_mdelay(1000);
            app = gui_current_app();
            gui_log("!!! GUI APP NOT READY! \n");
        }

        gui_obj_t *screen = &app->screen;
        GUI_ASSERT(screen != NULL);

        // rtgui_msg_t msg;
#if 0
        if (true == gui_mq_recv(gui_server_mq, &msg, sizeof(rtgui_msg_t), 0))
        {
        }
        else
        {
        }
#endif


        gui_fb_disp(screen);
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

        if ((gui_ms_get() - daemon_start_ms) > app->active_ms)
        {
            gui_log("daemon_start_ms time = %dms, current = %dms, app->active_ms = %dms \n", daemon_start_ms,
                    gui_ms_get(), app->active_ms);
            //gui_mq_recv(gui_server_mq, &msg, sizeof(rtgui_msg_t), 0x0FFFFFFF);
            daemon_cnt = 0;
        }

    }
}

void gui_server_suspend(void)
{
    gui_log("!Suspend GUI Server and wait receive message! \n");
    gui_thread_suspend(gui_server_handle);
}

void gui_server_resume(void)
{
    gui_thread_resume(gui_server_handle);
}


int rtgui_server_init(void)
{
    extern void gui_port_dc_init(void);
    extern void gui_port_indev_init(void);
    extern void gui_port_os_init(void);
    extern void gui_port_fs_init(void);
    gui_port_os_init();
    gui_port_indev_init();
    gui_port_dc_init();
#ifdef RTK_GUI_IMAGE_FS
    gui_port_fs_init();
#endif

    gui_acc_init();

    rtgui_system_image_init();
    rtgui_system_font_init();
    struct gui_indev *indev = gui_get_indev();

    if (gui_debug_hook != 0)
    {
        gui_debug_hook();
        gui_log("GUI Debug Mode!!");
        while (1);
    }
#if defined RTK_GUI_SCRIPT_APP
    extern void js_init(void);
    js_init();
#endif
    gui_server_handle = gui_thread_create(GUI_SERVER_THREAD_NAME,
                                          rtgui_server_entry, NULL,
                                          1024 * 10,
                                          15);
    return 0;
}



