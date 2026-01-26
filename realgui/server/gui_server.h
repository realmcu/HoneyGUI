/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * File      : gui_server.h
 */
#ifndef __GUI_SERVER_H__
#define __GUI_SERVER_H__


#include "guidef.h"
#include "gui_obj.h"
#include "gui_message.h"
#ifdef __cplusplus
extern "C" {
#endif
#define GUI_SERVER_THREAD_NAME "gui_s"
typedef void (* touch_sync_cb)(void);
int gui_server_init(void);
void gui_debug_sethook(void (*hook)(void));
bool gui_server_dlps_check(void);


void gui_server_exec_cb(gui_msg_cb cb);
void gui_task_ext_execution_sethook(void (*hook)(void));

#ifdef __cplusplus
}
#endif

#endif
