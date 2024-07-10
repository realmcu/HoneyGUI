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

#ifndef __GUI_AUTO_TEST_CMD_H__
#define __GUI_AUTO_TEST_CMD_H__
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
#include "shell.h"

#include "gui_app.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_page.h"
#include "gui_curtain.h"
#include "gui_card.h"
#include "gui_tab.h"

#include "gui_text.h"
#include "gui_scroll_text.h"

#include "gui_gallery.h"
#include "gui_cube.h"
#include "gui_canvas.h"

#include "gui_button.h"
#include "gui_img_live.h"
#include "gui_img_scope.h"
#include "gui_img_stb.h"
#include "gui_switch.h"
#include "gui_wheel_list.h"
#include "gui_turn_table.h"
#include "gui_canvas.h"
#include "gui_canvas_arc.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "gui_colorwheel.h"
#include "gui_grid.h"
#include "gui_keyboard.h"
#include "gui_map.h"
#include "gui_menu_cellular.h"
#include "gui_multi_level.h"
#include "gui_pagelist.h"
#include "gui_pagelistview.h"
#include "gui_perspective.h"
#include "gui_progressbar.h"
#include "gui_qbcode.h"
#include "gui_return.h"
#include "gui_scroll_wheel.h"
#include "gui_simple_img.h"
#include "gui_svg.h"
#include "gui_switch.h"
#include "gui_watch_gradient_spot.h"
#include "gui_wave.h"

#define GUI_EXPORT_CMD(cmd, func_name) \
    SHELL_EXPORT_CMD( \
                      SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, \
                      cmd, func_name, no_use);


#define AUTO_TEST_LOG(fmt, ...) \
    gui_log("[GUI_AUTO_TEST] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define MAKE_TYPE_NAME(prefix) gui_##prefix##_t

#define LOG_SIZEOF(prefix) AUTO_TEST_LOG("name: " #prefix " size: %lu", sizeof(MAKE_TYPE_NAME(prefix)))

#define GUI_IMG_T_SIZE 176 //2024/7/9 luke

#ifdef __cplusplus
}
#endif

#endif

