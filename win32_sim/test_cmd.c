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

#include "test_cmd.h"

void cmd_free_all(void)
{
    gui_msg_t msg = {.event = GUI_EVENT_FREE_ALL};
    gui_send_msg_to_server(&msg);
}
GUI_EXPORT_CMD(free_all, cmd_free_all);


void do_calculate(void *msg)
{
    gui_app_t *app = gui_current_app();
    gui_mem_debug();
}

void cmd_calculate(void)
{
    gui_server_exec_cb(do_calculate);
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




