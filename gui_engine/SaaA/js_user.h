/**
 * @file js_user.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-11-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __JS_USER_H__
#define __JS_USER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jerry_util.h>
#include <jerryscript.h>
#include <jerry_util.h>
#include "../3rd/js/js-port/config.h"
#include <gui_app.h>
#include <gui_obj.h>
#include <gui_text.h>
#include "gui_server.h"
#include "gui_win.h"
#include "gui_button.h"
#include "gui_progressbar.h"
#include "ezxml.h"
#include "gui_tabview.h"
#include "gui_server.h"
#include "gui_switch.h"
#include "gui_img.h"

#ifdef RTL87x2G
#include "wdt.h"
#include "rtl_wdt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct ezjs
{
    int a;
    int b;
};
typedef struct cb_arg
{
    jerry_value_t func;
    jerry_value_t *args_p;
    jerry_size_t args_count;
} cb_arg_t;
void js_gpio_init();
void wifi_init();
#ifdef __cplusplus
}
#endif

#endif

