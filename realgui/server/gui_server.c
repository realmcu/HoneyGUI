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
#include <gui_fb.h>
#include <gui_server.h>
#include <gui_obj.h>
#include <tp_algo.h>
#include <kb_algo.h>
#include <wheel_algo.h>
#include <string.h>
#include "gui_win.h"
#include "gui_components_init.h"
#include "gui_version.h"
#include "gui_server.h"



extern void gui_components_init(void);

/**
 * @brief External execution hook for GUI task
 */
static void (*gui_task_ext_execution_hook)(void) = NULL;

/**
 * @brief Sets the external execution hook for the GUI task.
 *
 * @param hook Function pointer to the external execution hook.
 */
void gui_task_ext_execution_sethook(void (*hook)(void))
{
    gui_task_ext_execution_hook = hook;
}

/**
 * @brief Entry point for the GUI server.
 *
 * This function initializes the GUI server, handles external execution hooks, processes input data,
 * and manages application transitions and screen updates.
 *
 * @param parameter Unused parameter.
 */
static void gui_server_entry(void *parameter)
{
    GUI_UNUSED(parameter);
    gui_server_msg_init();
    gui_components_init();
    while (1)
    {

        /* execute some app action, like kick watchdog */
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


        gui_fb_disp(gui_obj_get_root(), true);

#ifdef _WIN32
        gui_thread_mdelay(17);
#endif
        gui_recv_msg_to_server();
    }
}


/**
 * @brief Initializes the GUI server.
 *
 * This function initializes various components required for the GUI server and creates the server thread.
 *
 * @return int Returns 0 on successful initialization.
 */
int gui_server_init(void)
{
    static bool inited = false;
    if (inited)
    {
        return 0;
    }
    inited = true;
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

    gui_log("GUI branch Version: %s\n", VERSION_BRANCH);
    gui_log("GUI commit Version: %s\n", VERSION_COMMIT);
    gui_log("GUI build Date: %s\n", VERSION_BUILD_DATE);
    gui_log("GUI repo Status: %s\n", VERSION_REPO_STATUS);

    gui_thread_create(GUI_SERVER_THREAD_NAME,
                      gui_server_entry, NULL,
                      1024 * 10,
                      15);
    return 0;
}

