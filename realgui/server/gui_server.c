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
#include <gui_app.h>
#include "gui_win.h"

#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
#include "js_extern_io.h"
#endif

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
 * @brief Terminates the given application.
 *
 * This function ensures that the given application is properly terminated by calling its destructor,
 * deleting its thread, and resetting its close flag.
 *
 * @param current_app Double pointer to the application to be terminated.
 * @return true if the application was successfully terminated.
 * @return false if the current_app is NULL.
 */
static bool terminate_app(gui_app_t **current_app)
{
    if (*current_app == NULL)
    {
        gui_log("Error: current_app is NULL\n");
        return false;
    }

    if ((*current_app)->dtor != NULL)
    {
        (*current_app)->dtor(*current_app);
    }

    if ((*current_app)->thread_entry != NULL)
    {
        gui_thread_delete((*current_app)->thread_id);
    }

    (*current_app)->close = false;
    return true;
}

/**
 * @brief Handles the transition between layered applications.
 *
 * @param app Pointer to the current application.
 * @param app_next Pointer to the next application.
 * @param screen Pointer to the screen object.
 */
static void handle_layered_app_transition(gui_app_t **app, gui_app_t **app_next, gui_obj_t *screen)
{
    if ((*app)->close)
    {
        extern gui_app_t *next_app;
        extern gui_app_t *current_app;

        if (!terminate_app(&current_app)) { return; }

        if (next_app != NULL)
        {
            gui_list_remove(&(next_app->screen.brother_list));
            next_app->screen.parent = 0;
            gui_obj_tree_free(&((*app)->screen));
            current_app = next_app;
            next_app = NULL;
        }
        else
        {
            gui_log("!!! next_app is null");
            return;
        }
        *app = current_app;
    }
    else
    {
        gui_list_insert_before(&(screen->child_list), &((*app_next)->screen.brother_list));
        (*app_next)->screen.parent = screen;
    }
}

/**
 * @brief Handles the transition between non-layered applications.
 *
 * @param app Pointer to the current application.
 * @param app_next Pointer to the next application.
 * @param screen Pointer to the screen object.
 */
static void handle_non_layered_app_transition(gui_app_t **app, gui_app_t **app_next,
                                              gui_obj_t *screen)
{
    if ((*app)->close)
    {
        extern gui_app_t *next_app;
        extern gui_app_t *current_app;

        if (!terminate_app(&current_app)) { return; }

        gui_list_remove(&((*app)->screen.brother_list));
        (*app)->screen.parent = 0;
        gui_obj_tree_free(&((*app)->screen));
        current_app = next_app;
        next_app = 0;
        *app = current_app;
    }
    else
    {
        gui_list_insert_before(&((*app_next)->screen.child_list), &(screen->brother_list));
        screen->parent = &((*app_next)->screen);
    }
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
        if (layer && app_next && (&(app_next->screen)))
        {
            handle_layered_app_transition(&app, &app_next, screen);
        }
        else if (app_next && (&(app_next->screen)))
        {
            handle_non_layered_app_transition(&app, &app_next, screen);
        }

        gui_fb_disp(screen, true);
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
                if (&(app->screen.brother_list) != NULL)
                {
                    gui_list_remove(&(app->screen.brother_list));
                }
            }
        }


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
