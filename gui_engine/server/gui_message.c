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



static void *gui_server_mq = NULL;


void gui_server_msg_init(void)
{
    gui_mq_create(&gui_server_mq, "gui_svr_mq", sizeof(gui_msg_t), 16);
}

void gui_recv_msg_to_server(void)
{
    gui_msg_t msg;
    if (true == gui_mq_recv(gui_server_mq, &msg, sizeof(gui_msg_t), 0xFFFFFFFF))
    {
        gui_server_msg_handler(&msg);
    }
}
bool gui_send_msg_to_server(gui_msg_t *msg)
{
    if (gui_server_mq != NULL)
    {
        GUI_ASSERT(msg != NULL);
        gui_mq_send(gui_server_mq, msg, sizeof(gui_msg_t), 0);
        return true;
    }
    else
    {
        return false;
    }
}

void gui_server_exec_cb(gui_msg_cb cb)
{
    gui_msg_t msg = {.event = GUI_EVENT_USER_DEFINE, .cb = cb};
    gui_send_msg_to_server(&msg);
}


void gui_server_msg_handler(gui_msg_t *msg)
{
    gui_app_t *app = gui_current_app();
    gui_obj_t *screen = &app->screen;

    uint16_t event = msg->event;
    switch (event)
    {
    case GUI_EVENT_DISPLAY_ON:
        {
            gui_display_on();
            gui_fb_change();
            break;
        }
    case GUI_EVENT_DISPLAY_OFF:
        {
            break;
        }
    case GUI_EVENT_FREE_ALL:
        {
            gui_obj_tree_free(screen);
            gui_mem_debug();
            break;
        }
    case GUI_EVENT_EXTERN_IO_JS:
        {
#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
            gui_extern_event_js_handler(msg);
            event_handle = false;
#endif
            break;
        }
    case GUI_EVENT_USER_DEFINE:
        {
            msg->cb(msg->payload);
            break;
        }
    default:

        break;
    }

}





