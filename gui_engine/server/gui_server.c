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

void gui_debug_sethook(void (*hook)(void))
{
    gui_debug_hook = hook;
}

/**
 * rtgui server thread's entry
 */

static void rtgui_server_entry(void *parameter)
{
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

        gui_fb_disp(screen);

        if (app->actived == false)
        {
            gui_log("!Suspend GUI Server and wait receive message! \n");
            gui_thread_suspend(gui_server_handle);
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

    gui_server_handle = gui_thread_create(GUI_SERVER_THREAD_NAME,
                                          rtgui_server_entry, NULL,
                                          1024 * 10,
                                          15);
    return 0;
}



