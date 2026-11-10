/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "draw_img.h"
#include "gui_fb.h"
#include "gui_server.h"
#include "gui_obj.h"
#include <string.h>



static void *gui_server_mq = NULL;
static uint32_t keep_active_time = 5000; //ms
static uint32_t reset_time = 0;

void gui_set_keep_active_time(uint32_t active_time)
{
    keep_active_time = active_time;
}

void gui_server_msg_init(void)
{
    gui_mq_create(&gui_server_mq, "gui_svr_mq", sizeof(gui_msg_t), 16);
}

void gui_recv_msg_to_server(void)
{
    gui_msg_t msg;


    while (true == gui_mq_recv(gui_server_mq, &msg, sizeof(gui_msg_t), 0))
    {
        gui_server_msg_handler(&msg);
    }
#ifndef _HONEYGUI_SIMULATOR_
    if ((gui_ms_get() - reset_time) > keep_active_time)
    {
        gui_sleep_cb();
        if (true == gui_mq_recv(gui_server_mq, &msg, sizeof(gui_msg_t), 0xFFFFFFFF))
        {
            gui_server_msg_handler(&msg);
        }
    }
#endif


}
bool gui_send_msg_to_server(gui_msg_t *msg)
{
    if (gui_server_mq != NULL)
    {
        GUI_ASSERT(msg != NULL);
        if (gui_mq_send(gui_server_mq, msg, sizeof(gui_msg_t), 0))
        {
            return true;
        }
        else
        {
            return false;
        }
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

    uint16_t event = msg->event;
    switch (event)
    {
    case GUI_EVENT_DISPLAY_ON:
        {
            reset_time = gui_ms_get();
            gui_display_on();
            gui_fb_change();
            break;
        }
    case GUI_EVENT_DISPLAY_OFF:
        {
            // also need to clear all messages
            break;
        }
    case GUI_EVENT_FREE_ALL:
        {
            gui_obj_tree_free(gui_obj_get_root());
            gui_mem_debug();
            break;
        }
    case GUI_EVENT_USER_DEFINE:
        {
            msg->cb(msg);
            break;
        }
    case GUI_EVENT_RESET_ACTIVE_TIME:
        {
            reset_time = gui_ms_get();
            break;
        }
    case GUI_EVENT_SUBSCRIBE_TOPIC:
        {
            msg->cb(msg->payload);
            break;
        }
    default:

        break;
    }

}





