/**
 * @file test_cmd.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-05-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gui_server.h"
#include "unistd.h"
#include "time.h"
#include "gui_components_init.h"
#include "shell.h"

void debug_hook(void)
{
    gui_app_t *app = gui_current_app();
    gui_obj_tree_free(&app->screen);
    gui_mem_debug();
}
void dump_mem_status(void)
{
    gui_msg_t msg = {.event = GUI_EVENT_FREE_ALL};
    gui_send_msg_to_server(&msg);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    mem_status, dump_mem_status, status);



