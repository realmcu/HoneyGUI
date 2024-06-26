/**
 * @file auto_test_cmd.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-26
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

#define AUTO_TEST_LOG(fmt, ...) \
    gui_log("[GUI_AUTO_TEST] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

void print_pass(void *msg)
{
    AUTO_TEST_LOG("Pass!");
}
void cmd_print_pass(void)
{
    gui_server_exec_cb(print_pass);
}
GUI_EXPORT_CMD(pass, cmd_print_pass);
GUI_INIT_APP_EXPORT(cmd_print_pass);


void print_fail(void *msg)
{
    AUTO_TEST_LOG("Fail!");
}
void cmd_print_fail(void)
{
    gui_server_exec_cb(print_fail);
}
GUI_EXPORT_CMD(fail, cmd_print_fail);
GUI_INIT_APP_EXPORT(cmd_print_fail);


