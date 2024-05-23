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
#include "gui_img.h"
#include "shell.h"

#define GUI_EXPORT_CMD(cmd, func_name) \
    SHELL_EXPORT_CMD( \
                      SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, \
                      cmd, func_name, no_use);



void cmd_free_all(void)
{
    gui_msg_t msg = {.event = GUI_EVENT_FREE_ALL};
    gui_send_msg_to_server(&msg);
}
GUI_EXPORT_CMD(free_all, cmd_free_all);

void cmd_calculate(void)
{
    gui_app_t *app = gui_current_app();
    gui_mem_debug();

}
GUI_EXPORT_CMD(calculate, cmd_calculate);

void cmd_gui_print_all(void)
{
    gui_app_t *app = gui_current_app();
    gui_obj_tree_print(&app->screen);
}
GUI_EXPORT_CMD(gui_print_all, cmd_gui_print_all);

void cmd_check_img_widget(void)
{
    gui_log("sizeof(gui_obj_t) = %d \n", sizeof(gui_obj_t));
    gui_log("sizeof(gui_img_t) = %d \n", sizeof(gui_img_t));
    gui_log("sizeof(draw_img_t) = %d \n", sizeof(draw_img_t));
}
GUI_EXPORT_CMD(check_img_widget, cmd_check_img_widget);




