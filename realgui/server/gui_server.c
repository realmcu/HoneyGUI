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
#include "gui_components_init.h"
#include "gui_version.h"
#include "gui_server.h"

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

static void handle_binary_app_transition(gui_app_t **app, gui_app_t *app_next, gui_obj_t **screen,
                                         bool next_app_layer);
static void handle_binary_app_delete(gui_app_t **app_next, gui_app_t *app, bool next_app_layer);
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
        gui_app_t *app_current = gui_current_app();
        gui_app_t *app_next = gui_next_app();
        while (app_current == NULL)
        {
            gui_thread_mdelay(1000);
            app_current = gui_current_app();
            gui_log("!!! GUI APP NOT READY! \n");
        }

        gui_obj_t *screen = &app_current->screen;
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

        if ((app_current->lvgl == true) || (app_current->arm2d == true))
        {
            continue;
        }

        if (app_current->server_hook)
        {
            app_current->server_hook(app_current);
        }

        bool next_app_top_layer = gui_app_get_layer();

        handle_binary_app_transition(&app_current, app_next, &screen, next_app_top_layer);
        gui_fb_disp(screen, true);
        handle_binary_app_delete(&app_next, app_current, next_app_top_layer);


#ifdef _WIN32
        gui_thread_mdelay(17);
#endif
        gui_recv_msg_to_server();
    }
}
static void handle_binary_app_transition(gui_app_t **app, gui_app_t *app_next, gui_obj_t **screen,
                                         bool next_app_layer)
{

    if (next_app_layer)
    {
        if (app_next && (&(app_next->screen)))
        {
            if ((*app)->close)
            {
                extern gui_app_t *next_app;
                extern gui_app_t *current_app;
                if (current_app != NULL)
                {
                    if (current_app->dtor != NULL)
                    {
                        current_app->dtor(current_app);
                    }

                    if (current_app->thread_entry != NULL)
                    {
                        gui_thread_delete(current_app->thread_id);
                    }

                    current_app->close = false;
                }
                else
                {
                    gui_log("!!! current_app is null");
                    return;
                }
                if (next_app != NULL)
                {
                    gui_list_remove(&(next_app->screen.brother_list));
                    next_app->screen.parent = 0;
                    gui_obj_tree_free(&((gui_app_t *)(*app))->screen);
                    current_app = next_app;
                    next_app = NULL;
                }
                else
                {
                    gui_log("!!! next_app is null");
                    return;
                }
                (*app) = current_app;
            }
            else
            {
                gui_list_insert_before(&((*screen)->child_list), &(app_next->screen.brother_list));
                app_next->screen.parent = *screen;
            }
        }
        *screen = &((*app)->screen);
    }
    else
    {
        if (app_next && (&(app_next->screen)))
        {
            if ((*app)->close)
            {
                extern gui_app_t *next_app;
                extern gui_app_t *current_app;
                if (current_app == NULL)
                {
                    gui_log("Error: current_app is NULL\n");
                    return;
                }
                if (current_app->dtor != NULL)
                {
                    current_app->dtor(current_app);
                }
                if (current_app->thread_entry != NULL)
                {
                    gui_thread_delete(current_app->thread_id);
                }
                current_app->close = false;
                gui_list_remove(&((*app)->screen.brother_list));
                (*app)->screen.parent = 0;
                gui_obj_tree_free(&((gui_app_t *)(*app))->screen);
                current_app = next_app;
                next_app = 0;
                (*app) = current_app;
            }
            else
            {

                gui_list_insert_before(&((app_next->screen).child_list), &((*screen)->brother_list));
                (*screen)->parent = &(app_next->screen);
                *screen = &(app_next->screen);
            }
        }

    }
}
static void handle_binary_app_delete(gui_app_t **app_next, gui_app_t *app, bool next_app_layer)
{
    (*app_next) = gui_next_app();
    if (next_app_layer)
    {
        if ((*app_next) && (&((*app_next)->screen)))
        {
            gui_list_remove(&((*app_next)->screen.brother_list));
        }
    }
    else
    {
        if ((*app_next) && (&((*app_next)->screen)))
        {
            if (&(app->screen.brother_list) != NULL)
            {
                gui_list_remove(&(app->screen.brother_list));
            }
        }
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

GUI_INIT_PREV_EXPORT(gui_server_init);
