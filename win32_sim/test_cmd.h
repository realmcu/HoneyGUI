/**
 * @file auto_test_cmd.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __GUI_TEST_CMD_H__
#define __GUI_TEST_CMD_H__
#ifdef __cplusplus
extern "C" {
#endif

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


#ifdef __cplusplus
}
#endif

#endif

